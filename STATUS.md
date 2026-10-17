# Status

## Release candidate

- Plugin version: `0.6.4`.
- Exact runtime-tested compositor: Hyprland 0.56.2, commit
  `efb50993780079460b0cbed1363e2166a2de1d9f`.
- Tested ABI:
  `efb50993780079460b0cbed1363e2166a2de1d9f_aq_0.15_hu_0.14_hg_0.5_hc_0.1_hlg_0.6`.
- Build toolchain tested: GCC 16.2.1, CMake, and pkg-config `hyprland` 0.56.2.
- Current live result (2026-10-17): the user confirmed the 0.6.4 behavior is
  working as intended.

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
- Use Hyprland 0.56.2's typed mouse EventBus and event-loop timer.
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
- A README demonstration recording was deferred at the user's request and can
  be added in a later documentation-only release.
- Published 2026-10-17 to `https://github.com/mscharstrom/hypredging`.
  The `main` branch and annotated `v0.6.4` tag were pushed successfully.
  Public HTTP checks returned 200 for both, and the published `hyprpm.toml`
  and `LICENSE` hashes exactly matched the local release files.

## Exact next step

Add a real demonstration recording to the README when a privacy-safe capture is
available.
