cmake_minimum_required(VERSION 3.27)
project(Traitement_Image)

set(CMAKE_CXX_STANDARD 20)

add_executable(Traitement_Image
        tifo_skel_tp/histogram.cpp
        tifo_skel_tp/main.cpp
        tifo_skel_tp/histogram.hh
        tifo_skel_tp/image.hh
        tifo_skel_tp/image.cpp
        tifo_skel_tp/image_io.cpp
        tifo_skel_tp/image_io.hh
)
