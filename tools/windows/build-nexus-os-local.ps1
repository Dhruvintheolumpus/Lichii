# SPDX-License-Identifier: GPL-3.0-or-later
<#
.SYNOPSIS
  Configure, build, and package the NEXUS OS / Lichii Windows desktop app.
.DESCRIPTION
  Run this from an x64 Visual Studio Developer PowerShell after installing the
  prerequisites listed in docs/NEXUS-OS-Windows-Build.md.
.PARAMETER Jobs
  Number of simultaneous compiler jobs. Defaults to 8 for 16 GB RAM systems.
#>
[CmdletBinding()]
param(
    [ValidateRange(1, 24)]
    [int]$Jobs = 8,
    [switch]$Clean
)

$ErrorActionPreference = 'Stop'
$ProgressPreference = 'SilentlyContinue'

function Require-Command([string]$Name) {
    if (-not (Get-Command $Name -ErrorAction SilentlyContinue)) {
        throw "Required command '$Name' was not found. Follow docs/NEXUS-OS-Windows-Build.md, then reopen an x64 Developer PowerShell."
    }
}

$repo = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
Set-Location $repo

Write-Host '=== NEXUS OS local Windows build ===' -ForegroundColor Cyan
Require-Command 'cmake'
Require-Command 'ninja'
Require-Command 'cl'
Require-Command 'glslangValidator'

if (-not $env:VULKAN_SDK) {
    throw 'VULKAN_SDK is not set. Install the Vulkan SDK and reopen this terminal.'
}
if (-not (Test-Path (Join-Path $env:VULKAN_SDK 'Bin/glslangValidator.exe'))) {
    throw "Vulkan SDK tools were not found under $env:VULKAN_SDK."
}

$qtRoot = $env:QT_ROOT_DIR
if (-not $qtRoot) { $qtRoot = $env:QTDIR }
if (-not $qtRoot) {
    $qtCandidates = @(
        'C:/Qt/6.8.3/msvc2022_64',
        'C:/Qt/6.8.3/msvc2019_64'
    )
    $qtRoot = $qtCandidates | Where-Object { Test-Path (Join-Path $_ 'bin/windeployqt.exe') } | Select-Object -First 1
}
if (-not $qtRoot -or -not (Test-Path (Join-Path $qtRoot 'bin/windeployqt.exe'))) {
    throw 'Qt 6.8.3 for MSVC (including Qt Charts) was not found. See docs/NEXUS-OS-Windows-Build.md.'
}

$env:CMAKE_PREFIX_PATH = $qtRoot
$buildDir = Join-Path $repo 'build/release'
$binDir = Join-Path $buildDir 'bin'

if ($Clean -and (Test-Path $buildDir)) {
    Write-Host 'Removing previous build directory...'
    Remove-Item -Recurse -Force $buildDir
}

Write-Host "Source: $repo"
Write-Host "Qt:     $qtRoot"
Write-Host "Build:  $buildDir"
Write-Host "Jobs:   $Jobs"
Write-Host 'Configuring Release build...'

$configureArgs = @(
    '-S', '.', '-B', $buildDir, '-G', 'Ninja',
    '-DCMAKE_BUILD_TYPE=Release',
    '-DBUILD_TESTING=OFF',
    '-DYUZU_TESTS=OFF',
    '-DYUZU_CMD=ON',
    '-DYUZU_ROOM=OFF',
    '-DYUZU_ROOM_STANDALONE=OFF',
    '-DYUZU_USE_BUNDLED_QT=OFF',
    '-DYUZU_USE_BUNDLED_SDL3=ON',
    '-DENABLE_UPDATE_CHECKER=ON',
    '-DLICHII_BUILD_TAG=nexus-os-local',
    '-DLICHII_BUILD_PLATFORM=windows-x86_64',
    '-DUSE_DISCORD_PRESENCE=OFF'
)
& cmake @configureArgs
if ($LASTEXITCODE -ne 0) { throw "CMake configure failed with exit code $LASTEXITCODE." }

Write-Host "Compiling with $Jobs parallel jobs..." -ForegroundColor Cyan
& cmake --build $buildDir --target yuzu yuzu-cmd lichii-updater --parallel $Jobs
if ($LASTEXITCODE -ne 0) { throw "Compilation failed with exit code $LASTEXITCODE." }

if (-not (Test-Path (Join-Path $binDir 'lichii.exe'))) {
    throw "Build completed but lichii.exe was not found in $binDir."
}

$windeployqt = Join-Path $qtRoot 'bin/windeployqt.exe'
& $windeployqt --release --no-translations (Join-Path $binDir 'lichii.exe')
if ($LASTEXITCODE -ne 0) { throw "windeployqt failed with exit code $LASTEXITCODE." }

@"
NEXUS OS local Windows build
Built: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss zzz')
Configuration: Release
Parallel jobs: $Jobs
"@ | Set-Content (Join-Path $binDir 'README-NEXUS-OS.txt') -Encoding UTF8

Write-Host ''
Write-Host 'BUILD SUCCEEDED' -ForegroundColor Green
Write-Host "Run: $binDir\lichii.exe" -ForegroundColor Green
Write-Host 'Keep the entire bin folder together when moving the build.'
