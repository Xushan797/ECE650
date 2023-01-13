cmake_minimum_required(VERSION 3.21)
project(my_malloc C)

set(CMAKE_C_STANDARD 99)

add_library(my_malloc my_malloc.c)
