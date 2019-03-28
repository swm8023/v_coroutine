project(coexample)

cmake_minimum_required(VERSION 3.0)

add_executable(main main.cc)
target_link_libraries(main vco)
