# gbemu
Game Boy emulator

# Requirements
* CMake
* C++17 compiler
* Qt5

# Build
```
git clone --recurse-submodules -j8 git@github.com:kamil157/gbemu.git
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH=~/path/to/Qt/5.12.2/gcc_64/lib/cmake ../gbemu
make
```

# Run
```
./emulator ~/path/to/rom.gb
```
