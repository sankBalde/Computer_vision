project(tp1)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


include_directories(eigen-3.4.0 src)

add_executable(tp1_step1_test_kdtree
    src/step1_test_kdtree.cpp
    src/kdtree.cpp)

add_executable(tp1_step2_filter
    src/step2_filter.cpp
    src/kdtree.cpp
    src/obj.cpp)

add_executable(tp1_step3_smooth
    src/step3_smooth.cpp
    src/kdtree.cpp
    src/obj.cpp)

add_executable(tp1_step4_subsample
    src/step4_subsample.cpp
    src/kdtree.cpp
    src/obj.cpp)
