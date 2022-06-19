cmake_minimum_required(VERSION 3.16)
project(uff_ed_isam C)

set(CMAKE_C_STANDARD 99)

add_executable(uff_ed_isam TF.c func.c isam.c main.c)