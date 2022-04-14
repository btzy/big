cmake_minimum_required(VERSION 3.2)
project(big)

# Use latest C++ by default (should be the best one), but let user override it
if(NOT DEFINED CMAKE_CXX_STANDARD)
  set (CMAKE_CXX_STANDARD 23)
endif()

add_subdirectory(src)

option(BIG_BUILD_TESTS "Build tests for BIG" OFF)
if(${BIG_BUILD_TESTS})
  add_subdirectory(vendor/catch2)
  add_subdirectory(test)
endif()
