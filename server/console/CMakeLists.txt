cmake_minimum_required(VERSION 3.6.3)
project(run)

set (CMAKE_CXX_STANDARD 14)

file(GLOB SOURCES "*.cpp")

add_executable(run ${SOURCES})

find_library(SERVER_LINK_LIB server ${CMAKE_SOURCE_DIR}/server/lib/)
include_directories(${CMAKE_SOURCE_DIR}/server/lib/include/)
include_directories(${CMAKE_SOURCE_DIR}/protocol/include/)

find_library(PROTOCOL_LINK_LIB Protocol ${CMAKE_SOURCE_DIR}/protocol/)

message("found lib"  ${SERVER_LINK_LIB})

target_link_libraries(run ${SERVER_LINK_LIB})
target_link_libraries(run ${PROTOCOL_LINK_LIB})

