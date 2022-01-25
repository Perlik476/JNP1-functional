cmake_minimum_required(VERSION 3.21)
project(functional)

set(CMAKE_CXX_COMPILER "/home/matt/.GCC-11.2/bin/g++")

set(CMAKE_CXX_STANDARD 20)

add_executable(functional src/tri_list_concepts.h src/tri_list_example.cc src/tri_list.cc)
