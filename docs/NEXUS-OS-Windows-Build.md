# NEXUS OS: local Windows build

This guide builds the NEXUS OS branch on your own Windows PC. The script compiles the existing Lichii desktop app and packages its Qt runtime files; it does not modify your saves or install the emulator.

## 1. Install the prerequisites (one time)

Install these tools:

1. **Visual Studio 2022 Community** (or Build Tools) with the **Desktop development with C++** workload. Include the MSVC v143 x64/x86 build tools, Windows 10/11 SDK, and C++ CMake tools.
2. **CMake** and **Ninja**. They are often included with Visual Studio's C++ workload; otherwise install current Windows versions and make sure they are on PATH.
3. **Qt 6.8.3 for MSVC 2022 64-bit**, including the **Qt Charts** module. The expected default folder is `C:\Qt\6.8.3\msvc2022_64`.
4. **Vulkan SDK 1.4.341.1** (or a compatible newer SDK). Keep the installer's default location so it defines `VULKAN_SDK`.

The Qt kit must match MSVC, not MinGW. The script expects the desktop Qt kit and Qt Charts.

## 2. Get the NEXUS OS source

Open PowerShell and run:

```powershell
git clone --branch nexus-os https://github.com/Dhruvintheolumpus/Lichii.git
cd Lichii
```

If you already have the fork cloned, open that folder and switch to the branch:

```powershell
git fetch origin
git switch nexus-os
git pull
```

## 3. Run the build

Open **x64 Native Tools PowerShell for VS 2022** (or **Developer PowerShell for VS 2022**) from the Start menu. Change directory to your Lichii folder, then run:

```powershell
Set-ExecutionPolicy -Scope Process Bypass
.\tools\windows\build-nexus-os-local.ps1
```

For a 16 GB RAM PC, the script defaults to **8 parallel compile jobs**. You can override it, for example:

```powershell
.\tools\windows\build-nexus-os-local.ps1 -Jobs 6
```

To delete the existing build folder and perform a clean rebuild:

```powershell
.\tools\windows\build-nexus-os-local.ps1 -Clean
```

Do not use `-Clean` for normal updates; incremental builds are much faster.

## 4. Run the result

When the script prints **BUILD SUCCEEDED**, launch:

```text
build\release\bin\lichii.exe
```

Keep the entire `build\release\bin` folder together. Do not copy only the EXE elsewhere. The first configure/build downloads and compiles many dependencies and can take substantially longer than later incremental builds.

## Troubleshooting

- **'cl' not found:** you did not open an x64 Visual Studio Developer PowerShell.
- **Qt not found:** install Qt 6.8.3 `msvc2022_64` plus Qt Charts, or set `QT_ROOT_DIR` to that kit folder before running the script.
- **Vulkan tools not found:** install the Vulkan SDK, then close and reopen Developer PowerShell.
- **Out of memory:** rerun with `-Jobs 6` or `-Jobs 4`.
- **CMake configure error:** read the first CMake error above the final failure message; do not delete the CPM cache unless the error points to a corrupted download.

This is an independent community build of Lichii/NEXUS OS, not an official Nintendo product. Use only game files and system files you are legally entitled to use.
