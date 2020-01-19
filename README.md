# OpenVR Northstar

This is a community driver to support the North Star headset on OpenVR.  It is unafiliated with Leap Motion/Ultra Leap.

## Build Instructions

# Dependencies
- Visual Studio 2019 CE (https://visualstudio.microsoft.com/vs/). Under workloads make sure to select:
  - Desktop development with C++
  - .Net desktop development
- cmake (https://cmake.org/download/)
  - making sure to add it at least to the current user's system path
- Git (https://git-scm.com/download/win/)

# Installation
- Launch PowerShell
- Run checkout the source and run `build.bat` from the root directory or run the following commands:
```
git clone --recurse-submodules https://github.com/druidsbane/openvr-northstar.git
cd openvr-northstar
cmake -B build .
cmake --build ./build -- /t:restore,Build
```
- Once it is built navigate to the `bin/win64` directory and launch `NorthstarLauncher.exe`
