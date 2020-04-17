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
You need an email dataset to run the benchmark. Make sure you include email.txt under the dataset folder. Go to project root directory, run
```
./scripts/run_experiment.sh [OPTION]

Description
  -r, --repeat_times=NUM
    repeat the experiment NUM times, get the average as the final result. Experiments will run once by default
  --alm
    run alm improved encoders. All benchmarks will only run single, double, 3 gram, 4 gram encoders by default
  --art
    run art benchmark
  --surf
    run surf benchmark
  --btree
    run btree benchmark
  --prefixbtree
    run prefix btree benchmark
  --hot
    run hot benchmark
  --all
    run all benchmarks (art, surf, btree, prefixbtree, hot)

Example
./scripts/run_experiment.sh -r=3 --surf --alm
run surf benchmark on all encoders (include alm improved) three times and get the average as the result
```
The script will record numbers of specified benchmarks. To generate corresponding figures, make sure you include the --alm flag. Results will be in results/ and figures will be in figures/.

## License
Copyright 2020, Carnegie Mellon University

Licensed under the [GNU General Public License v3.0](https://github.com/efficient/OPE/blob/master/LICENSE).
