cmake_minimum_required(VERSION 3.12)
project(TEC_Media_File_System)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

add_executable(TEC_Media_File_System main.cpp Client.cpp Client.h)