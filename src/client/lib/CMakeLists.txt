cmake_minimum_required(VERSION 3.6.3)
project(client)

set (CMAKE_CXX_STANDARD 14)

file(GLOB SOURCES "*.cpp")

add_library(client ${SOURCES})

find_library(PROTOCOL_LINK_LIB Protocol ${CMAKE_SOURCE_DIR}/src/protocol/)
include_directories(${CMAKE_SOURCE_DIR}/src/protocol/include/)
include_directories(include)

message("found lib"  ${PROTOCOL_LINK_LIB})

target_link_libraries(client ${PROTOCOL_LINK_LIB})


#formating
add_custom_command(TARGET client
        PRE_BUILD
        COMMAND ${CMAKE_SOURCE_DIR}/src/format.sh
        COMMENT "Formating...")
