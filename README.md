# hypredging

**Taking your windows to the edge**

[![Latest release](https://img.shields.io/github/v/release/mscharstrom/hypredging?display_name=tag&sort=semver)](https://github.com/mscharstrom/hypredging/releases/latest)
[![License](https://img.shields.io/github/license/mscharstrom/hypredging)](LICENSE)
[![GitHub stars](https://img.shields.io/github/stars/mscharstrom/hypredging?style=flat)](https://github.com/mscharstrom/hypredging/stargazers)

Hypredging is a native Hyprland plugin that carries a window into the adjacent
numbered workspace when you drag it to the left or right monitor edge. The
window remains attached to the pointer, the destination uses Hyprland's native
workspace animation, and entry-edge hysteresis prevents accidental bounce.

## Demo

https://github.com/user-attachments/assets/dddd3f40-aad4-4336-9d19-6a3b69e8db72

The demonstration shows a window crossing several workspaces without losing
the active drag.

## Compatibility

Hyprland plugins are ABI-sensitive. Use the Hypredging release that matches
your exact Hyprland release:

| Hyprland | Hypredging | Status |
| --- | --- | --- |
| 0.57.0 (`0aa6745f`) | [v0.7.0](https://github.com/mscharstrom/hypredging/releases/tag/v0.7.0) | Supported and runtime-tested |
| 0.56.2 (`efb50993`) | [v0.6.4](https://github.com/mscharstrom/hypredging/releases/tag/v0.6.4) | Supported legacy release |

Only the listed Hyprland commits are claimed as compatible. Even a patch
release can change the plugin ABI. HyprPM uses the repository's commit pins to
select the matching source revision automatically.

To check your installed version and ABI:

```sh
Hyprland --version
```

See [CHANGELOG.md](CHANGELOG.md) for release details.

## Dependencies

- A supported Hyprland release and its matching development headers
- CMake 3.19 or newer
- `pkg-config`
- A C++23 compiler (GCC 16.2.1 was used for the tested build)

HyprPM prepares matching Hyprland headers automatically. A manual build needs
the matching `hyprland.pc` available to `pkg-config`.

## Install with HyprPM

```sh
hyprpm add https://github.com/mscharstrom/hypredging
hyprpm enable hypredging
hyprpm reload
```

After upgrading Hyprland, update and rebuild the pinned plugin revision:

```sh
hyprpm update
hyprpm reload
```

## Configuration

Both settings are optional:

| Setting | Default | Range | Meaning |
| --- | ---: | ---: | --- |
| `edge_delay_ms` | `0` | 0–10000 | Time at an edge before switching workspaces |
| `rearm_delay_ms` | `350` | 0–2000 | Minimum time before the entry edge can switch back |

For a Hyprland Lua configuration:

```lua
hl.config({
    plugin = {
        hypredging = {
            edge_delay_ms = 0,
            rearm_delay_ms = 350,
        },
    },
})
```

Hyprland 0.56.2 users who still use the legacy configuration format can use:

```hyprlang
plugin {
    hypredging {
        edge_delay_ms = 0
        rearm_delay_ms = 350
    }
}
```

Re-arming the entry edge also requires 96 logical pixels of inward pointer
movement. The opposite edge remains available, so continuous dragging across
several workspaces still works.

Hypredging currently handles positive numbered workspaces only. It ignores
special workspaces and does nothing at workspace 1's left edge.

## Manual build

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

The plugin is written to `build/hypredging.so`. A successful build is not a
substitute for a live compositor test. If compilation fails after a Hyprland
upgrade, first confirm that the checked-out Hypredging release and installed
headers match the compatibility matrix.

## Troubleshooting

- `hyprpm update` refreshes and rebuilds plugins after a Hyprland upgrade.
- `hyprpm list` shows the installed and enabled plugin state.
- `hyprctl plugin list` shows plugins loaded in the current session.
- An ABI mismatch or missing internal header usually means the selected
  Hypredging revision does not match the installed Hyprland release.

If the matching pair from the matrix still fails, open an issue and include
the output of `Hyprland --version` and the complete build error.

## Uninstall

```sh
hyprpm disable hypredging
hyprpm remove hypredging
hyprpm reload
```

## License

BSD-3-Clause. See [LICENSE](LICENSE) and
[THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).
