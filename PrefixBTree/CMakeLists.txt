cmake_minimum_required(VERSION 3.9)
project(CompressedBTree)

set(CMAKE_CXX_STANDARD 14)

add_executable(bench_cpsbtree bench_cpsbtree.cpp include/CPSBTreeOLC.h include/BTreeOLC.h include/tlx/btree.hpp include/tlx/core.hpp include/tlx/core.cpp include/tlx/btree_map.hpp)



