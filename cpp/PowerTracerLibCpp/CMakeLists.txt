cmake_minimum_required(VERSION 3.28)
project(PowerTracerLibCpp)

set(CMAKE_CXX_COMPILER "clang-15")
set(CMAKE_CXX_STANDARD 20)

add_library(PowerTracerLibCpp SHARED library.cpp
        SerialPort.cpp
        SerialPort.h
        RiscurePowerTracerIntefaces.h
        RiscurePowerTracerConstants.h)
