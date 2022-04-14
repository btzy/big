set(TEST_LIST
  test_best_int.cpp)
  
foreach (TEST ${TEST_LIST})
  string(REPLACE "." "_" CLEAN_TEST_NAME ${TEST})
  add_executable(${CLEAN_TEST_NAME}
    ${TEST})
  target_link_libraries(${CLEAN_TEST_NAME}
    PRIVATE big)
  target_link_libraries(${CLEAN_TEST_NAME}
    PRIVATE Catch2::Catch2WithMain)
endforeach()