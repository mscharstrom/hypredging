# hypredging agent notes

Read `STATUS.md` before working. Target the installed Hyprland release exactly; do not assume plugin ABI compatibility across releases.

Keep the implementation minimal. Build and static checks are not live compositor tests. Ask before loading or unloading the plugin in the user's running Hyprland session.

After meaningful progress, update `STATUS.md` with decisions, failed approaches, build/test results, remaining problems, and one exact next step.
