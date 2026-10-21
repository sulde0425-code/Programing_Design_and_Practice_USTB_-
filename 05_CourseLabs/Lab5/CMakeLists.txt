cmake_minimum_required(VERSION 4.1)
project(Lab5_2)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(Lab5_2
        HistoryManager.cpp
        HistoryManager.h
        Lab5.cpp)
