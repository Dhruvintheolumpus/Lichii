# NEXUS OS implementation and test status

This document distinguishes features that are present in source from features that still need implementation or verification. A feature is not considered tested until a Windows build succeeds and the behavior is exercised on a real Windows PC.

## Current integrated test-build scope

- NEXUS HOME is embedded in the existing Lichii Qt desktop frontend.
- HOME reads the existing Lichii game-list model. It displays detected game titles and their existing icon/cover image data in a horizontal, keyboard-focusable row.
- Selecting a HOME game calls the existing Lichii game-launch path; it does not create a second emulator or a fake launcher.
- HOME search filters the displayed game cards.
- The local NEXUS display name is editable and saved in QSettings on this PC. This is a display label, not a separate Nintendo account or a replacement for Lichii's emulator user profiles.
- The clock updates every second.
- Game Library opens Lichii's existing library.
- Settings opens Lichii's existing configuration dialog.
- Store opens the public Nintendo Store website in the system browser.
- Screenshot calls Lichii's existing screenshot action.
- Quick Settings is wired to Lichii's existing fullscreen and volume/mute actions.
- Ctrl+H returns to NEXUS HOME when no game is starting or running.
- The HOME screen contains an independent-project disclaimer.

## Build pipeline

- Workflow: .github/workflows/nexus-os-windows.yml
- Manual trigger: GitHub Actions -> NEXUS OS Windows Build -> Run workflow.
- Automatic trigger: pushes to nexus-os; pull requests targeting nx2.
- Windows runner: Windows Server 2022, MSVC environment, pinned LLVM/clang-cl, Qt 6.8.3, Vulkan SDK, Ninja, CPM cache, sccache.
- Artifact: NEXUS-OS-Windows-x64-<run number>, containing the portable build/release/bin folder.
- Expected GUI executable: lichii.exe. Keep the whole artifact folder together; the executable depends on adjacent Qt/runtime files.
- The workflow checks for lichii.exe, yuzu-cmd.exe, and lichii-updater.exe before uploading.

A workflow definition and a successful build are different things. The source has not been declared build-verified until GitHub Actions reports success and the artifact exists.

## First Windows test checklist

1. Download and extract the full NEXUS OS artifact ZIP from a successful Actions run.
2. Keep the extracted folder structure intact.
3. Launch lichii.exe.
4. Confirm NEXUS HOME appears and the clock updates.
5. Confirm the profile display name can be changed and remains after restarting.
6. Confirm existing configured game directories populate the HOME row after Lichii finishes scanning.
7. Confirm game titles and cover icons appear without duplicate entries or crashes.
8. Search for a game, clear the search, and confirm the row updates.
9. Launch a legally owned game from a HOME card and verify the existing Lichii boot flow works.
10. Close the game and verify the emulator returns to a usable library/window state.
11. Test Game Library, Settings, Store, Screenshot, fullscreen, volume up/down, mute, and Ctrl+H.
12. Check the Windows console/logs if the application crashes or any action does nothing; attach the log and the exact Actions run link to the bug report.

## Remaining work before calling NEXUS OS feature-complete

The following items are not yet implemented as complete NEXUS features. Some related emulator primitives already exist in Lichii, but they still need a deliberate HOME integration and testing.

- Proper multiple local NEXUS profiles with avatar selection, per-profile recent games, favorites, and statistics.
- Accurate per-game session history and weekly/monthly summaries, integrated with Lichii's play-time manager rather than inventing duplicate timers.
- Full controller-first navigation across HOME, menus, quick settings, and dialogs, including D-pad/analog and confirm/back/context mappings.
- Dedicated Controller Management entry point and controller status view.
- Real Album browser for screenshots, with safe access to Lichii's existing screenshot storage.
- News page using a public, clearly attributed source or browser link.
- Theme engine: light/dark/automatic, accent colors, wallpaper, scale, and animation preferences.
- Original NEXUS ambient music and UI sounds, volume controls, import/preview, and reliable fade/pause/resume around game sessions. No Nintendo sound recordings should be bundled.
- Complete quick-settings overlay and power/sleep actions with safe Windows behavior.
- Full settings pages for display, audio, network, storage, accessibility, profiles, and NEXUS appearance, reusing existing Lichii settings where possible.
- Startup/profile flow, robust launch/return transitions, accessibility review, and release packaging/signing decisions.
- Automated UI smoke tests and manual testing on different Windows display scales, GPUs, and controllers.

## Project boundaries

NEXUS OS is an independent community interface layered over Lichii. It is not Nintendo Horizon OS, does not use Nintendo proprietary code/assets/sounds, and does not provide Nintendo account or online services. Use original or appropriately licensed artwork/audio and game files you are legally entitled to use.
