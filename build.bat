REM git clone --depth 1 https://github.com/ValveSoftware/openvr.git ../openvr
git checkout --recurse-submodules
git submodule update --init --recursive
cmake -B build .
cmake --build ./build -- /t:restore,Build
