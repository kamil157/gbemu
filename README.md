# gbemu
Game Boy emulator

[![Build Status](https://travis-ci.com/kamil157/gbemu.svg?branch=master)](https://travis-ci.com/kamil157/gbemu)

# Requirements
* CMake
* C++17 compiler
* Qt5
* qtbase5-dev

# Build
```
git clone --recurse-submodules -j8 git@github.com:kamil157/gbemu.git
mkdir build
cd build
cmake ../gbemu
make
```

# Run
```
./emulator ~/path/to/rom.gb
```
