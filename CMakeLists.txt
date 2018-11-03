cmake_minimum_required (VERSION 2.6)
project (OPE)

message(STATUS "Configuring..." ${CMAKE_PROJECT_NAME})

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -Wall -march=native -mavx -pthread -std=c++11")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -g -O3 -Wall -Werror -march=native -mavx -pthread -std=c++11")

enable_testing()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include/symbol_selectors")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include/code_generators")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include/dictionaries")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include/encoders")

add_subdirectory(test)
add_subdirectory(bench)
