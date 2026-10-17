// SPDX-License-Identifier: BSD-3-Clause
//
// Workspace transition synchronization below is adapted from Hyprland 0.56.2's
// CMonitor::changeWorkspace. See THIRD_PARTY_NOTICES.md.

#include <chrono>
#include <stdexcept>
#include <string>

#include <hyprland/src/animation/WorkspaceAnimationController.hpp>
#include <hyprland/src/desktop/state/GlobalWindowController.hpp>
#include <hyprland/src/desktop/view/Window.hpp>
#include <hyprland/src/event/EventBus.hpp>
#include <hyprland/src/config/values/types/IntValue.hpp>
#include <hyprland/src/layout/LayoutManager.hpp>
#include <hyprland/src/managers/EventManager.hpp>
#include <hyprland/src/managers/eventLoop/EventLoopManager.hpp>
#include <hyprland/src/output/Monitor.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/pointer/PointerController.hpp>
#include <hyprland/src/state/MonitorState.hpp>
#include <hyprland/src/state/WorkspaceState.hpp>

namespace {
    enum class Edge { NONE, LEFT, RIGHT };

    constexpr float EDGE_ZONE_PX = 24.F;
    constexpr float WRAP_INSET_PX = 32.F;
    constexpr float REARM_DISTANCE_PX = 96.F;

    HANDLE g_handle = nullptr;
    CHyprSignalListener g_mouseMoveListener;
    SP<Config::Values::CIntValue> g_delayMs;
    SP<Config::Values::CIntValue> g_rearmDelayMs;
    SP<CEventLoopTimer> g_edgeTimer;
    Edge g_activeEdge = Edge::NONE;
    Edge g_blockedEdge = Edge::NONE;
    std::chrono::steady_clock::time_point g_blockedSince{};

    Edge edgeAt(const Vector2D& cursor) {
        const auto monitor = State::monitorState()->query().vec(cursor).run();
        if (!monitor)
            return Edge::NONE;

        if (cursor.x <= monitor->m_position.x + EDGE_ZONE_PX)
            return Edge::LEFT;
        if (cursor.x >= monitor->m_position.x + monitor->m_size.x - EDGE_ZONE_PX)
            return Edge::RIGHT;
        return Edge::NONE;
    }

    bool canRearmBlockedEdge(const Vector2D& cursor) {
        const auto monitor = State::monitorState()->query().vec(cursor).run();
        if (!monitor || g_blockedEdge == Edge::NONE)
            return false;

        const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - g_blockedSince).count();
        if (elapsed < g_rearmDelayMs->value())
            return false;

        return g_blockedEdge == Edge::LEFT ? cursor.x >= monitor->m_position.x + REARM_DISTANCE_PX :
                                             cursor.x <= monitor->m_position.x + monitor->m_size.x - REARM_DISTANCE_PX;
    }

    bool moveToAdjacentWorkspace(Edge edge) {
        const auto& drag = g_layoutManager->dragController();
        const auto target = drag ? drag->target() : nullptr;
        const auto window = target ? target->window() : nullptr;
        const auto monitor = window ? window->m_monitor.lock() : nullptr;
        const auto current = monitor ? monitor->m_activeWorkspace : nullptr;
        if (!window || !monitor || !current || current->m_id <= 0)
            return false;

        const WORKSPACEID nextID = current->m_id + (edge == Edge::RIGHT ? 1 : -1);
        if (nextID <= 0)
            return false;

        auto next = State::workspaceState()->query().id(nextID).run();
        if (!next)
            next = State::workspaceState()->create(nextID, monitor->m_id, std::to_string(nextID));
        if (!next)
            return false;

        Desktop::globalWindowController()->moveWindowToWorkspace(window, next);

        // The normal 0.56.2 path releases held mouse buttons. Internal switching
        // keeps the active drag grab. Reproduce its render-state synchronization.
        monitor->changeWorkspace(next, true, true, true);
        const bool animateLeft = edge == Edge::RIGHT;
        const auto animationStyle = next->m_animationStyle;
        Animation::Workspace::startAnimation(current, Animation::Workspace::ANIMATION_TYPE_OUT, animateLeft, false, animationStyle);
        Animation::Workspace::startAnimation(next, Animation::Workspace::ANIMATION_TYPE_IN, animateLeft, false, animationStyle);
        g_layoutManager->recalculateMonitor(monitor, Layout::CLayoutManager::RECALCULATE_MONITOR_REASON_WORKSPACE_CHANGE);
        g_pEventManager->postEvent(SHyprIPCEvent{"workspace", next->m_name});
        g_pEventManager->postEvent(SHyprIPCEvent{"workspacev2", std::format("{},{}", next->m_id, next->m_name)});
        Event::bus()->m_events.workspace.active.emit(next);

        const auto cursor = g_pInputManager->getMouseCoordsInternal();
        const auto wrappedX = edge == Edge::RIGHT ? monitor->m_position.x + WRAP_INSET_PX : monitor->m_position.x + monitor->m_size.x - WRAP_INSET_PX;
        g_blockedEdge = edge == Edge::RIGHT ? Edge::LEFT : Edge::RIGHT;
        g_blockedSince = std::chrono::steady_clock::now();
        Pointer::pointerController()->warpTo({wrappedX, cursor.y}, true);
        g_pInputManager->simulateMouseMovement();
        return true;
    }

    void onMouseMove(Vector2D cursor, Event::SCallbackInfo&) {
        const auto& drag = g_layoutManager->dragController();
        const auto target = drag ? drag->target() : nullptr;
        if (!target || drag->mode() != MBIND_MOVE) {
            g_edgeTimer->updateTimeout(std::nullopt);
            g_activeEdge = Edge::NONE;
            g_blockedEdge = Edge::NONE;
            return;
        }

        const auto edge = edgeAt(cursor);
        if (canRearmBlockedEdge(cursor)) {
            g_blockedEdge = Edge::NONE;
            g_activeEdge = Edge::NONE;
        } else if (edge == g_blockedEdge) {
            return;
        }

        if (edge == Edge::NONE) {
            g_edgeTimer->updateTimeout(std::nullopt);
            g_activeEdge = Edge::NONE;
            return;
        }

        if (edge == g_activeEdge)
            return;

        g_activeEdge = edge;
        if (g_delayMs->value() == 0)
            moveToAdjacentWorkspace(edge);
        else
            g_edgeTimer->updateTimeout(std::chrono::milliseconds{g_delayMs->value()});
    }
}

APICALL EXPORT std::string PLUGIN_API_VERSION() {
    return HYPRLAND_API_VERSION;
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
    g_handle = handle;
    if (std::string{__hyprland_api_get_hash()} != __hyprland_api_get_client_hash())
        throw std::runtime_error("hypredging: Hyprland ABI mismatch");

    g_delayMs = makeShared<Config::Values::CIntValue>(
        "plugin:hypredging:edge_delay_ms", "delay before an edge switches workspace", 0,
        Config::Values::SIntValueOptions{.min = 0, .max = 10000});
    if (!HyprlandAPI::addConfigValueV2(g_handle, g_delayMs))
        throw std::runtime_error("hypredging: failed to register edge_delay_ms");

    g_rearmDelayMs = makeShared<Config::Values::CIntValue>(
        "plugin:hypredging:rearm_delay_ms", "minimum cooldown before the entry edge can switch back", 350,
        Config::Values::SIntValueOptions{.min = 0, .max = 2000});
    if (!HyprlandAPI::addConfigValueV2(g_handle, g_rearmDelayMs))
        throw std::runtime_error("hypredging: failed to register rearm_delay_ms");

    g_edgeTimer = makeShared<CEventLoopTimer>(std::nullopt, [](SP<CEventLoopTimer>, void*) {
        if (g_activeEdge != Edge::NONE)
            moveToAdjacentWorkspace(g_activeEdge);
    }, nullptr);
    g_pEventLoopManager->addTimer(g_edgeTimer);

    g_mouseMoveListener = Event::bus()->m_events.input.mouse.move.listen(onMouseMove);
    if (!g_mouseMoveListener)
        throw std::runtime_error("hypredging: failed to register mouse listener");

    return {"hypredging", "Move dragged windows across workspace edges", "mscharstrom", "0.6.4"};
}

APICALL EXPORT void PLUGIN_EXIT() {
    g_mouseMoveListener.reset();
    if (g_edgeTimer) {
        g_edgeTimer->cancel();
        g_pEventLoopManager->removeTimer(g_edgeTimer);
        g_edgeTimer.reset();
    }
    g_delayMs.reset();
    g_rearmDelayMs.reset();
    g_activeEdge = Edge::NONE;
    g_blockedEdge = Edge::NONE;
    g_handle = nullptr;
}
