#!/bin/bash

PROJECT_DIR=$(pwd)

##################################################
# Initialize modules
##################################################
# Install boost 1.66.0 for hot
wget --directory-prefix=hot/third-party/ https://dl.bintray.com/boostorg/release/1.66.0/source/boost_1_66_0.tar.gz
tar -xvf hot/third-party/boost_1_66_0.tar.gz -C hot/third-party/
mkdir hot/third-party/boost_install
cd hot/third-party/boost_1_66_0
./bootstrap.sh --prefix=./boost_install
./b2 install
cd ${PROJECT_DIR}
# Add boost to include path
export LD_LIBRARY_PATH=${PROJECT_DIR}/hot/third-party/boost_install/lib:$LD_LIBRARY_PATH

git submodule update --init --recursive

###################################################
# Generate worklaods
##################################################
# Download YCSB if the directory not exists
cd workload_gen
chmod 744 ./ycsb_download.sh
[ ! -d "./YCSB" ] && ./ycsb_download.sh

./gen_workload.sh

###################################################
# Build Project
##################################################
cd ${PROJECT_DIR}
mkdir build
cd build
cmake ..
make -j
cd ${PROJECT_DIR}
##################################################
# Run experiments
##################################################

# Experiment Arguments
run_alm=1
repeat_times=$1

run_microbench=1

run_surf=1
run_art=1
run_btree=1
run_hot=1
PYTHON=python

function remove_old_results() {
    if [ $1 == 1 ]
    then
        rm -r $2
    fi
}

function run_experiment() {
    if [ $1 == 1 ]
    then
        eval $2
    fi
}

remove_old_results ${run_microbench} "results/microbench/cpr_latency/"
remove_old_results ${run_surf} "results/SuRF"
remove_old_results ${run_art} "results/ART"
remove_old_results ${run_hot} "results/hot"
remove_old_results ${run_btree} "results/btree"

./create_dir.sh

cnt=0
while [ ${cnt} -lt ${repeat_times} ]
do
    run_experiment ${run_microbench} "./build/bench/microbench 1 ${run_alm}" 
    run_experiment ${run_surf} "./build/SuRF/bench/bench_surf 0 ${run_alm}"
    run_experiment ${run_surf} "./build/SuRF/bench/bench_surf 1 ${run_alm}"
    run_experiment ${run_art} "./build/ART/bench/bench_art 0 ${run_alm}"
    run_experiment ${run_art} "./build/ART/bench/bench_art 1 ${run_alm}"
    run_experiment ${run_hot} "./build/hot/bench_hot 0 ${run_alm}"
    run_experiment ${run_hot} "./build/hot/bench_hot 1 ${run_alm}"
    run_experiment ${run_btree} "./build/btree/bench_btree 0 ${run_alm}"
    run_experiment ${run_btree} "./build/btree/bench_btree 1 ${run_alm}"

    let "cnt+=1"
done

# Get the average result
${PYTHON} generate_result.py
echo "===========Finish Generating results============"
#################################################
# Generate plots
################################################
./plot.sh ${run_microbench} ${run_surf} ${run_art} ${run_hot} ${run_btree}
