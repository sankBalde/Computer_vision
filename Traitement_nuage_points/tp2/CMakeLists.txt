project(tp2)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -O3")

include_directories(eigen-3.4.0 src)

add_executable(ball_pivoting
    src/main.cpp
    src/kdtree.cpp
    src/obj.cpp
    src/geometry.cpp
    src/pivot.cpp
    src/init.cpp)
