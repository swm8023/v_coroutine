project(coexample)

cmake_minimum_required(VERSION 3.0)

add_executable(test_create test_create.cc)
target_link_libraries(test_create vco)
