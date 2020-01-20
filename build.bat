git checkout --recurse-submodules
git submodule update --init --recursive
cmake -B build .
cmake --build ./build --target ALL_BUILD --config Release -- /t:restore,Build
