cmake_minimum_required(VERSION 3.8)
project(variancelib)

set(SRCS graph.cpp bdd.cpp bddvariance.cpp)

add_library(variancelib STATIC ${SRCS})

target_compile_features(variancelib PUBLIC cxx_std_11)
#target_include_directories(variancelib INTERFACE .)