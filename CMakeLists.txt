cmake_minimum_required(VERSION 3.5)
project(turkey)

find_package(Boost REQUIRED)
find_package(Threads REQUIRED)

add_subdirectory(src)
