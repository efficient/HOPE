cmake_minimum_required (VERSION 2.6)
project (OPE)

message(STATUS "Configuring..." ${CMAKE_PROJECT_NAME})

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -Wall -Werror -march=native -mavx -pthread -std=c++14")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -Wall -Werror -march=native -mavx -pthread -std=c++14")

#option(COVERALLS "Generate coveralls data" ON)

if (COVERALLS)
  include("${CMAKE_CURRENT_SOURCE_DIR}/CodeCoverage.cmake")
  append_coverage_compiler_flags()
  set(COVERAGE_EXCLUDES '*/bench/*' '*/ART/*' '*/SuRF/*' '*/test/*' '/usr/*' '/lib/*')
  setup_target_for_coverage(
    NAME coverage
    EXECUTABLE make test
    )
endif()

enable_testing()

include_directories("/usr/local/include")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include/symbol_selectors")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include/code_assigners")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include/dictionaries")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include/encoders")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include/art_dic")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include/succinct_binary_trie")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/SuRF/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ART/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/btree/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/PrefixBTree/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/libcuckoo/libcuckoo")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/bench/")

find_library(JemallocLib jemalloc)
find_library(TbbLib tbb)
find_package (Threads)

add_library(ART ART/src/Tree.cpp)
target_link_libraries(ART ${TbbLib} ${JemallocLib} ${CMAKE_THREAD_LIBS_INIT} atomic)

add_subdirectory(test)
add_subdirectory(bench)
add_subdirectory(PrefixBTree/bench)
add_subdirectory(PrefixBTree/test)
add_subdirectory(ART/test)
add_subdirectory(ART/bench)
add_subdirectory(SuRF/test)
add_subdirectory(SuRF/bench)
add_subdirectory(btree)

add_executable(example example.cpp)
target_link_libraries(example)

option(BUILD_HOT "Build HOT" OFF)
if (BUILD_HOT)
  add_subdirectory(hot)
endif()
