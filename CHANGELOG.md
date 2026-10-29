# Changelog

All notable user-facing changes are documented here. Hypredging follows
semantic versioning, while compatibility remains tied to exact Hyprland
releases because Hyprland's plugin ABI is not stable across releases.

## 0.7.0 - 2026-10-29

### Added

- Support for Hyprland 0.57.0 at commit
  `0aa6745f705f2c4b0a5004eb019ef76e5bbdb426`.
- A compatibility matrix documenting the supported Hypredging release for each
  tested Hyprland release.

### Changed

- Ported workspace lookup and creation to Hyprland 0.57.0's typed workspace
  identifiers.
- Ported workspace IPC publication to Hyprland 0.57.0's Socket2 API.
- Updated new-workspace initialization for Hyprland 0.57.0's explicit
  workspace lifecycle.

### Tested

- Release build with GCC 16.2.1 and matching Hyprland 0.57.0 headers.
- Live edge dragging across numbered workspaces, including preserved pointer
  attachment, workspace transitions, and repeated crossings.

## 0.6.4 - 2026-10-17

- Initial public release for Hyprland 0.56.2 at commit
  `efb50993780079460b0cbed1363e2166a2de1d9f`.
- Added edge-triggered workspace travel, native workspace animation, pointer
  wrapping, configurable delay, and entry-edge hysteresis.
