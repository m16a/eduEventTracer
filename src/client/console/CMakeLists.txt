cmake_minimum_required(VERSION 3.6.3)
project(run)

set (CMAKE_CXX_STANDARD 14)

file(GLOB SOURCES "*.cpp")

add_executable(run ${SOURCES})

include_directories(${CMAKE_SOURCE_DIR}/src/client/lib/include/)
include_directories(${CMAKE_SOURCE_DIR}/src/protocol/include/)

message("found lib "  ${CLIENT_LINK_LIB})

target_link_libraries(run Protocol client)


#formating
add_custom_command(TARGET run
        PRE_BUILD
        COMMAND ${CMAKE_SOURCE_DIR}/src/format.sh
        COMMENT "Formating...")

