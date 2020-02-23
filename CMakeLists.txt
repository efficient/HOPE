cmake_minimum_required (VERSION 2.6)
project (OPE)

message(STATUS "Configuring..." ${CMAKE_PROJECT_NAME})

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -Wall -Werror -march=native -mavx -pthread -std=c++14")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -Wall -Werror -march=native -mavx -pthread -std=c++14")

option(COVERALLS "Generate coveralls data" ON)

if (COVERALLS)
  include("${CMAKE_CURRENT_SOURCE_DIR}/CodeCoverage.cmake")
  append_coverage_compiler_flags()
  set(COVERAGE_EXCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/ART/include/*" "PrefixBTree/*" "SuRF/*" "btree/*" "datasets/*" "hot/*" "bench/*" "test/*" "/usr/*" "/lib/*")
  setup_target_for_coverage(
    NAME coverage
    EXECUTABLE make test
    )
else()
  add_definitions(-DNDEBUG)
endif()

enable_testing()

include_directories("/usr/local/include")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include/symbol_selectors")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include/code_generators")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include/dictionaries")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include/encoders")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include/art_dic_new")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include/succinct_binary_trie")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/SuRF/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ART/include")
#include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ART_single/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/btree/include")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/libcuckoo/libcuckoo")

find_library(JemallocLib jemalloc)
find_library(TbbLib tbb)
find_package (Threads)

add_library(ART ART/src/Tree.cpp)
target_link_libraries(ART ${TbbLib} ${JemallocLib} ${CMAKE_THREAD_LIBS_INIT} atomic)

#add_library(ART_single ART_single/src/Tree.cpp)
#target_link_libraries(ART_single ${TbbLib} ${JemallocLib} ${CMAKE_THREAD_LIBS_INIT})

add_subdirectory(test)
add_subdirectory(bench)
#add_subdirectory(SuRF/test)
#add_subdirectory(SuRF/bench)
#add_subdirectory(ART_single/test)
#add_subdirectory(ART_single/bench)
#add_subdirectory(ART/test)
#add_subdirectory(ART/bench)
#add_subdirectory(btree)
#add_subdirectory(hot)


#################################################################
# Format Related
################################################################
set(OPE_BUILD_SUPPORT_DIR "${CMAKE_SOURCE_DIR}/build_support")

# clang-format
find_program(CLANG_FORMAT_BIN
        NAMES clang-format clang-format-8
        HINTS "${OPE_CLANG_SEARCH_PATH}")
if ("${CLANG_FORMAT_BIN}" STREQUAL "CLANG_FORMAT_BIN-NOTFOUND")
    message(WARNING "couldn't find clang-format.")
else()
    message(STATUS "found clang-format at ${CLANG_FORMAT_BIN}")
endif()

# clang-tidy
find_program(CLANG_TIDY_BIN
        NAMES clang-tidy clang-tidy-8
        HINTS "${OPE_CLANG_SEARCH_PATH}")
if ("${CLANG_TIDY_BIN}" STREQUAL "CLANG_TIDY_BIN-NOTFOUND")
    message(WARNING "couldn't find clang-tidy.")
else()
    # Output compile_commands.json
    set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
    message(STATUS "found clang-tidy at ${CLANG_TIDY_BIN}")
endif()

# cpplint
find_program(CPPLINT_BIN
        NAMES cpplint cpplint.py
        HINTS "${OPE_BUILD_SUPPORT_DIR}")
if ("${CPPLINT_BIN}" STREQUAL "CPPLINT_BIN-NOTFOUND")
    message(WARNING "couldn't find cpplint.")
else()
    message(STATUS "found cpplint at ${CPPLINT_BIN}")
endif()


##########################################
# "make format"
# "make check-format"
##########################################

string(CONCAT OPE_FORMAT_DIRS
        "${CMAKE_CURRENT_SOURCE_DIR}/include,"
        "${CMAKE_CURRENT_SOURCE_DIR}/test,"
        )

# runs clang format and updates files in place.
add_custom_target(format ${OPE_BUILD_SUPPORT_DIR}/run_clang_format.py
        ${CLANG_FORMAT_BIN}
        ${OPE_BUILD_SUPPORT_DIR}/clang_format_exclusions.txt
        --source_dirs
        ${OPE_FORMAT_DIRS}
        --fix
        --quiet
        )

# runs clang format and exits with a non-zero exit code if any files need to be reformatted
add_custom_target(check-format ${OPE_BUILD_SUPPORT_DIR}/run_clang_format.py
        ${CLANG_FORMAT_BIN}
        ${OPE_BUILD_SUPPORT_DIR}/clang_format_exclusions.txt
        --source_dirs
        ${OPE_FORMAT_DIRS}
        --quiet
        )

##########################################
# "make check-lint"
##########################################

file(GLOB_RECURSE OPE_LINT_FILES
        #"${CMAKE_CURRENT_SOURCE_DIR}/include/*/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/art_dic_new/*.hpp"
        #"${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp"
        )

# Balancing act: cpplint.py takes a non-trivial time to launch,
# so process 12 files per invocation, while still ensuring parallelism
add_custom_target(check-lint echo '${OPE_LINT_FILES}' | xargs -n12 -P8
        ${CPPLINT_BIN}
        --verbose=2 --quiet
        --linelength=120
        --filter=-legal/copyright,-build/header_guard
        )

###########################################################
# "make check-clang-tidy" target
###########################################################
# runs clang-tidy and exits with a non-zero exit code if any errors are found.
# note that clang-tidy automatically looks for a .clang-tidy file in parent directories
add_custom_target(check-clang-tidy
        ${OPE_BUILD_SUPPORT_DIR}/run_clang_tidy.py                     # run LLVM's clang-tidy script
        -clang-tidy-binary ${CLANG_TIDY_BIN}                              # using our clang-tidy binary
        -p ${CMAKE_BINARY_DIR}                                            # using cmake's generated compile commands
        )
