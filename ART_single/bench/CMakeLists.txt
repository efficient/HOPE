add_executable(bench_art bench_art.cpp)
target_link_libraries(bench_art ART_single)

add_executable(test_art test_art.cpp)
target_link_libraries(test_art ART_single)
