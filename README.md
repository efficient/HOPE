# High-speed Order-Preserving Encoder (HOPE)
[![Build Status](https://travis-ci.org/efficient/HOPE.svg?branch=master)](https://travis-ci.org/efficient/HOPE)
[![Coverage Status](https://coveralls.io/repos/github/efficient/HOPE/badge.svg?branch=master)](https://coveralls.io/github/efficient/HOPE?branch=master)

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
```
./scripts/run_experiment.sh [OPTION]
```

We included a sample of the Wiki and URL datasets in this repository. To reproduce the results in our paper, please download the full datasets (download links are in the paper) to replace the samples. Our Email dataset is private. You need to provide your own email list (email.txt) to run the corresponding experiments. Below are options to facilitate running a subset of the full benchmark:
```
Options
  -r, --repeat_times=N
    Run each experiment N times and report the average measurements. Default: 1.
  --email, --wiki, --url
    Run the benchmark using the Email/Wiki/URL dataset.
    If unspecified, the scripts includes the Wiki and URL experiments.
  --alldatasets
    Include benchmarks for all three datasets.
  --alm
    Include the alm-based encoders. The other encoders (Single, Double, 3-gram, 4-gram) are enabled by default.
  --surf, --art, --hot, --btree, --prefixbtree
    Run the SuRF/ART/HOT/B+tree/prefix B+tree benchmark suite.
  --all
    Run the full benchmark. If unspecified, the script only runs the microbenchmarks for Wiki and URL.
```

The above script will record benchmark measurements under "results/". The master plotting script is under "scripts/". The individual scripts are under "plots/". Generated figures will be under "figures/". Make sure you run the benchmark with the --alm option on before using the plotting scripts.

## License
Copyright 2020, Carnegie Mellon University

Licensed under the [Apache License 2.0](https://github.com/efficient/OPE/blob/master/LICENSE).
