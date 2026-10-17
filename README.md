# hypredging

**Taking your windows to the edge**

Hypredging is a native Hyprland plugin that carries a window into the adjacent
numbered workspace when you drag it to the left or right monitor edge. The
window remains attached to the pointer, the destination uses Hyprland's native
workspace animation, and entry-edge hysteresis prevents accidental bounce.

## Compatibility

Runtime-tested only with **Hyprland 0.56.2**, commit
`efb50993780079460b0cbed1363e2166a2de1d9f` (ABI
`efb50993780079460b0cbed1363e2166a2de1d9f_aq_0.15_hu_0.14_hg_0.5_hc_0.1_hlg_0.6`).

Hyprland plugins are ABI-sensitive. No other Hyprland version is currently
claimed as supported.

## Dependencies

- Hyprland 0.56.2 and its matching development headers
- CMake 3.19 or newer
- `pkg-config`
- A C++23 compiler (GCC 16.2.1 was used for the tested build)

HyprPM prepares matching Hyprland headers automatically. A manual build needs
the matching `hyprland.pc` available to `pkg-config`.

## Install with HyprPM

After this repository is published at the planned URL:

```sh
hyprpm add https://github.com/mscharstrom/hypredging
hyprpm enable hypredging
hyprpm reload
```

## Configuration

Both settings are optional:

| Setting | Default | Range | Meaning |
| --- | ---: | ---: | --- |
| `edge_delay_ms` | `0` | 0–10000 | Time at an edge before switching workspaces |
| `rearm_delay_ms` | `350` | 0–2000 | Minimum time before the entry edge can switch back |

For a standard Hyprland configuration:

```hyprlang
plugin {
    hypredging {
        edge_delay_ms = 0
        rearm_delay_ms = 350
    }
}
```

Or, for a Hyprland 0.56 Lua configuration:

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
substitute for a live compositor test.

## Uninstall

```sh
hyprpm disable hypredging
hyprpm remove hypredging
hyprpm reload
```

## License

BSD-3-Clause. See [LICENSE](LICENSE) and
[THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).
