cmake_minimum_required(VERSION 3.6.3)

set (CMAKE_CXX_STANDARD 14)
#Bring the headers, such as Student.h into the project
include_directories(include)

#Can manually add the sources using the set command as follows:
#set(SOURCES src/mainapp.cpp src/Student.cpp)

file(GLOB SOURCES "*.cpp")

add_library(Protocol ${SOURCES})

