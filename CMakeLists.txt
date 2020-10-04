cmake_minimum_required(VERSION 3.17)
project(untitled C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_COMPILER "gcc")

add_executable(untitled os-hw1/lib_hw1/main.c os-hw1/lib_hw1/operation.h os-hw1/lib_hw1/list.h os-hw1/lib_hw1/list.c os-hw1/lib_hw1/hash.h os-hw1/lib_hw1/hash.c os-hw1/lib_hw1/bitmap.h os-hw1/lib_hw1/bitmap.c os-hw1/lib_hw1/hex_dump.h os-hw1/lib_hw1/hex_dump.c os-hw1/lib_hw1/round.h os-hw1/lib_hw1/debug.h os-hw1/lib_hw1/debug.c os-hw1/lib_hw1/limits.h)