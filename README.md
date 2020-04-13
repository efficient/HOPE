# High-speed Order-Preserving Encoder (HOPE)

**HOPE** is a fast dictionary-based compressor that encodes
arbitrary byte-strings while preserving their order.
It is optimized for compressing database index keys.
Detailed description can be found in our
[SIGMOD paper](https://arxiv.org/pdf/2003.02391.pdf).

##  Install Dependencies
    sudo apt-get install build-essential cmake libgtest.dev
    cd /usr/src/gtest
    sudo cmake CMakeLists.txt
    sudo make
    sudo cp *.a /usr/lib

## Build
    mkdir build
    cd build
    cmake ..
    make -j

## Usage Example
A simple example can be found [here](https://github.com/efficient/OPE/blob/master/example.cpp). To run the example:
```
cd build
./example
```

## Unit Tests
    make test

## Benchmark
Go to project root directory, run
```
bash ./scripts/run_experiment.sh 2
```
2 here means running experiment twice and getting the average as the final result. You can control which benchmark (ART, SuRF, BTree, PrefixBTree) to run by changing arguments in ```scripts/run_experiment.sh```.
Results will be in results/ and figures will be in figures/

## License
Copyright 2020, Carnegie Mellon University

Licensed under the [GNU General Public License v3.0](https://github.com/efficient/OPE/blob/master/LICENSE).