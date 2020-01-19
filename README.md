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
- Run the following
```
git clone --recurse-submodules https://github.com/druidsbane/openvr-northstar.git
cd openvr-northstar
build.bat
```
- Once it is built navigate to the `bin/win64` directory and launch `NorthstarLauncher.exe`
