cmake_minimum_required(VERSION 3.6.3)
project(server)

set (CMAKE_CXX_STANDARD 14)

#Bring the headers, such as Student.h into the project
#include_directories(include)

#Can manually add the sources using the set command as follows:
#set(SOURCES src/mainapp.cpp src/Student.cpp)

#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "*.cpp")

add_executable(sv_run ${SOURCES})

find_library(PROTOCOL_LINK_LIB protocol ${CMAKE_SOURCE_DIR}/protocol/)
include_directories(${CMAKE_SOURCE_DIR}/protocol/include/)
target_link_libraries(sv_run ${PROTOCOL_LINK_LIB})
