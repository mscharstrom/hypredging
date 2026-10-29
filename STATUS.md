# Status

## Published release

- Plugin version: `0.6.4`.
- Exact runtime-tested compositor: Hyprland 0.56.2, commit
  `efb50993780079460b0cbed1363e2166a2de1d9f`.
- Tested ABI:
  `efb50993780079460b0cbed1363e2166a2de1d9f_aq_0.15_hu_0.14_hg_0.5_hc_0.1_hlg_0.6`.
- Build toolchain tested: GCC 16.2.1, CMake, and pkg-config `hyprland` 0.56.2.
- Current live result (2026-10-17): the user confirmed the 0.6.4 behavior is
  working as intended.

## 0.7.0 release candidate

- Installed compositor/development headers: Hyprland 0.57.0, commit
  `0aa6745f705f2c4b0a5004eb019ef76e5bbdb426`.
- Installed ABI:
  `0aa6745f705f2c4b0a5004eb019ef76e5bbdb426_aq_0.15_hu_0.14_hg_0.5_hc_0.1_hlg_0.6`.
- The 0.6.4 source failed to compile because 0.57.0 moved `Window.hpp`, removed
  `EventManager.hpp`, and replaced integer workspace IDs with typed
  numbered/special workspace IDs and new query/creation APIs.
- The local port now uses the relocated window header, Socket2 IPC events, and
  the typed workspace APIs. It also calls `ready()` after a newly created
  workspace has been moved/focused, following 0.57.0's workspace lifecycle.
- A fresh Release build against the installed 0.57.0 headers succeeds with GCC
  16.2.1 and no compiler warnings. `git diff --check` also passes, and the
  shared object contains no GNU-unique symbols.
- Hyprland 0.57.0's `CMonitor::changeWorkspace` implementation was checked
  against the plugin's synchronization sequence. The normal path still releases
  held mouse buttons, while the internal path preserves the drag. IPC event
  names and payloads were updated to match the 0.57.0 implementation.
- With the user's permission, `/tmp/hypredging-build-057/hypredging.so` was
  loaded into the running Hyprland 0.57.0 session on 2026-10-29. Hyprland
  accepted it without an ABI rejection, and the user confirmed that edge
  dragging works. That binary still reported version 0.6.4 because the version
  bump had not yet been applied; its implementation matches the tested port.
- The release candidate is now versioned 0.7.0 in CMake and the plugin metadata.
  README compatibility and troubleshooting documentation, `CHANGELOG.md`, and
  the Hyprland 0.57.0 provenance notice have been prepared locally.
- A fresh 0.7.0 Release build succeeds. With the user's permission, the earlier
  test binary was unloaded and `/tmp/hypredging-release-070/hypredging.so` was
  loaded successfully. Hyprland reports version 0.7.0 without an ABI rejection;
  the functional implementation is identical to the user-confirmed test build.
- No HyprPM 0.57.0 pin, tag, release, commit, or push has been made.

## Confirmed runtime behavior

- `Super + left mouse` move-drags are detected for floating and tiled windows,
  including windows previously fullscreen.
- Entering a 24-logical-pixel left or right edge moves the dragged window to
  the adjacent positive numbered workspace and creates that workspace if needed.
- Existing destination windows and the dragged window stay visible.
- The drag remains attached to the pointer and releasing the mouse ends it normally.
- Native workspace IN/OUT animation plays in the travel direction.
- Waybar receives the normal workspace events and highlights only the destination.
- The pointer wraps 32 logical pixels inside the destination workspace.
- The entry edge is blocked until both the configurable cooldown (350 ms by
  default) and 96 logical pixels of inward movement have occurred. The opposite
  edge remains usable for continued travel.
- `edge_delay_ms = 0` is immediate; a nonzero delay also works while stationary.

## Decisions and constraints

- Target the exact tested Hyprland release; plugin ABI compatibility is not
  assumed across releases.
- Use Hyprland's typed mouse EventBus and event-loop timer.
- Preserve the active drag with the internal workspace-switch path, then perform
  the render-state synchronization, native animation, monitor recalculation,
  and IPC/EventBus publication skipped by that path.
- Support positive numbered workspaces only; ignore special workspaces.
- Compile with `-fno-gnu-unique` so unloading and reloading a rebuilt plugin does
  not retain stale DSO state.

## Failed approaches retained for reference

- Hyprland's normal/non-internal workspace switch releases held mouse buttons
  in 0.56.2 and therefore ends the drag.
- Internal switching alone made destination and dragged windows invisible until
  a normal workspace round trip; animation state and monitor recalculation fixed it.
- Internal switching alone also left Waybar's source workspace highlighted;
  publishing the normal workspace events fixed it.
- Exact-edge matching, extra drag-threshold gating, and function-hook diagnostics
  were unnecessary once the EventBus build was reliably reloaded.
- Percentage-based re-arm depth felt display-size dependent. Direction-specific
  time plus fixed logical-distance hysteresis produced the accepted behavior.

## Release preparation

- `hyprpm.toml` uses the verified CMake commands and `build/hypredging.so` output.
- Generated build trees and binaries are ignored.
- BSD-3-Clause is proposed, matching Hyprland 0.56.2 and the official Hyprland
  plugin collection licenses. Hyprland's upstream BSD notice is retained in
  `THIRD_PARTY_NOTICES.md` because its 0.56.2 implementation informed the
  internal workspace-transition sequence.
- A source-level provenance review found no reused code under an incompatible
  license. The adapted workspace synchronization sequence is marked in
  `src/main.cpp` and covered by the retained Hyprland BSD-3-Clause notice.
- A temporary Git repository was created from only the publication candidates,
  cloned into a separate clean directory, and built successfully in Release mode
  on 2026-10-17. The resulting artifact was `build/hypredging.so`; the build tree
  remained ignored and no GNU-unique symbols or embedded local paths were found.
- A publication scan found no home-directory paths, email addresses, private-key
  markers, common access-token formats, passwords, secrets, or local IP addresses.
  The public author identifier `mscharstrom` remains intentionally visible.
- HyprPM pins Hyprland 0.56.2 commit
  `efb50993780079460b0cbed1363e2166a2de1d9f` to the tested initial plugin
  commit `a19e08ab1df416312b4bf7a6dcce50c8808f19f6`.
- Added the user's eight-second demonstration recording and a 1280×720 clickable
  README preview. The recording was optimized to 1280×720 H.264 at 30 fps and
  1.27 MiB for GitHub preview compatibility. It has no audio and its container
  metadata contains no personal information.
- Published 2026-10-17 to `https://github.com/mscharstrom/hypredging`.
  The `main` branch and annotated `v0.6.4` tag were pushed successfully.
  Public HTTP checks returned 200 for both, and the published `hyprpm.toml`
  and `LICENSE` hashes exactly matched the local release files.

## Exact next step

Create the local 0.7.0 implementation/documentation commit, add its hash as the
Hyprland 0.57.0 HyprPM pin in a follow-up release commit, then present the final
history for explicit permission before tagging or pushing.
