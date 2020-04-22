#!/bin/bash

PROJECT_DIR="$(pwd)"

###################################################
# Parse Experiment Arguments
###################################################
run_microbench=0
run_small_experiment=0
run_alm=0
run_art=0
run_btree=0
run_hot=0
run_surf=0
run_prefixbtree=0
run_email=0
run_wiki=0
run_url=0
repeat_times=1

PYTHON=python

for i in "$@"
do
case $i in
  -r=*|--repeat=*)
  repeat_times="${i#*=}"
  shift # past argument=value
  ;;
  --microbench)
  run_microbench=1
  echo "Run microbench"
  shift
  ;;
  --alm)
  run_alm=1
  echo "Run ALM encoder"
  shift
  ;;
  --art)
  run_art=1
  echo "Run ART"
  shift
  ;;
  --btree)
  run_btree=1
  echo "Run BTree"
  shift
  ;;
  --hot)
  run_hot=1
  echo "Run HOT"
  shift
  ;;
  --surf)
  run_surf=1
  echo "Run SuRF"
  shift
  ;;
  --prefixbtree)
  run_prefixbtree=1
  echo "Run Prefix BTree"
  shift
  ;;
  --email)
  run_email=1
  echo "Run email dataset"
  shift
  ;;
  --wiki)
  run_wiki=1
  echo "Run wiki dataset"
  shift
  ;;
  --url)
  run_url=1
  echo "Run url dataset"
  shift
  ;;
  --alldatasets)
  echo "Run all datasets"
  run_email=1
  run_wiki=1
  run_url=1
  shift
  ;;
  --all)
  echo "Run All Benchmarks"
  run_microbench=1
  run_art=1
  run_btree=1
  run_hot=1
  run_surf=1
  run_prefixbtree=1
  shift
  ;;
  *)
  # unknown option
esac
done


function install_hot_dependeny() {
  cd ${PROJECT_DIR}
  git submodule update --init --recursive
}

if [[ ${run_hot} == 1 ]]
then
  echo "Intsall dependency for HOT"
  ##################################################
  # Initialize modules for HOT
  ##################################################
  install_hot_dependeny
fi

###################################################
# Generate worklaods
###################################################
# Download YCSB if the directory does not exist
cd workload_gen
chmod 744 ./ycsb_download.sh
[ ! -d "./YCSB" ] && ./ycsb_download.sh
[ ! -d "../workloads" ] && mkdir ../workloads && ./gen_workload.sh

###################################################
# Build Project
###################################################
cd ${PROJECT_DIR}
mkdir build
cd build
if [[ ${run_hot} == 1 ]]
then
 cmake .. -DBUILD_HOT=ON
else
 cmake .. -DBUILD_HOT=OFF
fi
make -j
cd ${PROJECT_DIR}

##################################################
# Run experiments
##################################################

function remove_old_results() {
    if [[ $1 == 1 ]]
    then
        rm -r $2
    fi
}

function run_experiment() {
    if [[ $1 == 1 ]]
    then
        eval $2
    fi
}

remove_old_results ${run_microbench} "results/microbench/cpr_latency/"
remove_old_results ${run_microbench} "figures/microbench/cpr_latency/"
remove_old_results ${run_surf} "results/SuRF"
remove_old_results ${run_surf} "figures/SuRF"
remove_old_results ${run_art} "results/ART"
remove_old_results ${run_art} "figures/ART"
remove_old_results ${run_hot} "results/hot"
remove_old_results ${run_hot} "figures/hot"
remove_old_results ${run_btree} "results/btree"
remove_old_results ${run_btree} "figures/btree"
remove_old_results ${run_prefixbtree} "results/prefixbtree"
remove_old_results ${run_prefixbtree} "figures/prefixbtree"

./scripts/create_dir.sh

cnt=0
while [[ ${cnt} -lt ${repeat_times} ]]
do
    run_experiment ${run_microbench} "build/bench/microbench 1 ${run_alm} ${run_email} ${run_wiki} ${run_url}"
    run_experiment ${run_surf} "build/SuRF/bench/bench_surf 0 ${run_alm} ${run_email} ${run_wiki} ${run_url}"
    run_experiment ${run_surf} "build/SuRF/bench/bench_surf 1 ${run_alm} ${run_email} ${run_wiki} ${run_url}"
    run_experiment ${run_art} "build/ART/bench/bench_art 0 ${run_alm} ${run_email} ${run_wiki} ${run_url}"
    run_experiment ${run_art} "build/ART/bench/bench_art 1 ${run_alm} ${run_email} ${run_wiki} ${run_url}"
    run_experiment ${run_hot} "build/hot/bench_hot 0 ${run_alm} ${run_email} ${run_wiki} ${run_url}"
    run_experiment ${run_hot} "build/hot/bench_hot 1 ${run_alm} ${run_email} ${run_wiki} ${run_url}"
    run_experiment ${run_btree} "build/btree/bench_btree 0 ${run_alm} ${run_email} ${run_wiki} ${run_url}"
    run_experiment ${run_btree} "build/btree/bench_btree 1 ${run_alm} ${run_email} ${run_wiki} ${run_url}"
    run_experiment ${run_prefixbtree} "build/PrefixBTree/bench/bench_prefix_btree 0 ${run_alm} ${run_email} ${run_wiki} ${run_url}"
    run_experiment ${run_prefixbtree} "build/PrefixBTree/bench/bench_prefix_btree 1 ${run_alm} ${run_email} ${run_wiki} ${run_url}"
    let "cnt+=1"
done

# Get the average result
${PYTHON} scripts/generate_result.py micro-tree

echo "===========Finish Generating Results============"
#################################################
# Generate plots
#################################################
if [[ ${run_microbench} == 1 ]] || [[ ${run_surf} == 1 ]] || [[ ${run_art} == 1 ]] || [[ ${run_hot} == 1 ]] || [[ ${run_btree} == 1 ]] || [[ ${run_prefixbtree} == 1 ]]
then
  if [[ ${run_alm} == 1 ]]
  then
    ./scripts/plot.sh ${run_microbench} ${run_surf} ${run_art} ${run_hot} ${run_btree} ${run_prefixbtree} ${run_small_experiment} ${run_email} ${run_wiki} ${run_url}
    echo "Finish generate plots"
  else
    echo "Did not generate plots. You must run ALM encoder to generate plots"
  fi
fi
