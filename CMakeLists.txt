cmake_minimum_required(VERSION 3.16)

set(FLAGS -std=c++17 -O3 -Wall -g)

set(CMAKE_CXX_COMPILER g++)

add_compile_options(${FLAGS})
project(PROIEKT VERSION 0.1 LANGUAGES CXX)

add_executable(cp_miner.out cp_miner.cc)
