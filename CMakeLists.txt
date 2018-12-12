cmake_minimum_required (VERSION 2.6)
project (OPE)

message(STATUS "Configuring..." ${CMAKE_PROJECT_NAME})

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -Wall -march=native -mavx -pthread -std=c++14")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -g -O3 -Wall -Werror -march=native -mavx -pthread -std=c++14")

enable_testing()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include/symbol_selectors")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include/code_generators")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include/dictionaries")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include/encoders")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/SuRF/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ART/include")

find_library(JemallocLib jemalloc)
find_library(TbbLib tbb)
find_package (Threads)

add_library(ART ART/src/Tree.cpp)
target_link_libraries(ART ${TbbLib} ${JemallocLib} ${CMAKE_THREAD_LIBS_INIT})

add_subdirectory(test)
add_subdirectory(bench)
add_subdirectory(SuRF/test)
add_subdirectory(SuRF/bench)
add_subdirectory(ART/test)
add_subdirectory(ART/bench)
