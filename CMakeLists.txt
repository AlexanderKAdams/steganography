cmake_minimum_required(VERSION 3.20)
project(steganography C)

set(CMAKE_C_STANDARD 90)

add_executable(steg steg.c)
add_executable(desteg desteg.c)
