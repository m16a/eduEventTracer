cmake_minimum_required(VERSION 2.8.9)
project(eduEventTracer)

set(CMAKE_BUILD_TYPE  Debug)

add_subdirectory(protocol)
add_subdirectory(server)
add_subdirectory(client)
add_subdirectory(example)
