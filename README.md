## High-speed Order-Preserving Encoder (HOPE)

##  Install Dependencies
    sudo apt-get install build-essential cmake libgtest.dev
    cd /usr/src/gtest
    sudo cmake CMakeLists.txt
    sudo make
    sudo cp *.a /usr/lib

## Build
    mkdir build
    cd build
    make ..
    make -j

## Run Unit Tests
    make test

## Benchmark
Go to project root directory, run
```
bash ./scripts/run_experiment.sh 2
```
2 here means running experiment twice and getting the average as the final result. You can control which benchmark (ART, SuRF, BTree, PrefixBTree) to run by changing arguments in ```scripts/run_experiment.sh```.
Results will be in results/ and figures will be in figures/
