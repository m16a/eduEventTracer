cmake_minimum_required(VERSION 2.8.9)
project(eduEventTracer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE  Debug)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_DEBUG" )

add_subdirectory(src)
