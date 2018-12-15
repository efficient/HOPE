find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIR})

function (add_unit_test file_name)
  add_executable(${file_name} ${file_name}.cpp)
  target_link_libraries(${file_name} /usr/local/lib/libgtest.a /usr/local/lib/libgtest_main.a)
  add_test(NAME ${file_name}
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${file_name}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endfunction()

add_unit_test(test_ngram_ss)
add_unit_test(test_heuristic_ss)
add_unit_test(test_blend_trie)
add_unit_test(test_hu_tucker_cg)
add_unit_test(test_single_char_encoder)
add_unit_test(test_double_char_encoder)
add_unit_test(test_ngram_encoder)

