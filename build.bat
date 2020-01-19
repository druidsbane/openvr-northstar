git checkout --recurse-submodules
git submodule update --init --recursive
cmake -B build .
cmake --build ./build -- /t:restore,Build
