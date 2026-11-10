cmake_minimum_required(VERSION 4.3)
project(GBEmulator C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(GBEmulator
        cpu.c
        cpu.h
        main.c
        main.h
        memory.c
        memory.h)
