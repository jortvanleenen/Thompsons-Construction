cmake_minimum_required(VERSION 3.10)

project(RegExp VERSION 1.0.0 DESCRIPTION "Pattern Recognition")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(RegExp src/main.cpp src/RegularExpression.cpp
        include/RegularExpression.h)

include_directories(include)
