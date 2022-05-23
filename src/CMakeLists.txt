cmake_minimum_required(VERSION 3.8)
project(variance)

add_subdirectory(./mylib)
add_executable(bddmake bddmake.cpp)
add_executable(main main.cpp)
add_executable(naive naive.cpp)
add_executable(deldeg1 deldeg1.cpp)

target_link_libraries(bddmake variancelib)
target_link_libraries(main variancelib)
target_link_libraries(naive variancelib)
target_link_libraries(deldeg1 variancelib)
