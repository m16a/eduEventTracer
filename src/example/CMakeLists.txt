cmake_minimum_required(VERSION 3.6.3)
project(example)

set (CMAKE_CXX_STANDARD 14)

file(GLOB SOURCES "*.cpp")

add_executable(test_app ${SOURCES})

find_library(PROTOCOL_LINK_LIB Protocol ${CMAKE_SOURCE_DIR}/src/protocol/)

find_library(SERVER_LINK_LIB server ${CMAKE_SOURCE_DIR}/src/server/)
include_directories(${CMAKE_SOURCE_DIR}/src/server/include/)
include_directories(${CMAKE_SOURCE_DIR}/src/protocol/include/)

target_link_libraries(test_app ${SERVER_LINK_LIB} ${PROTOCOL_LINK_LIB})


#formating
add_custom_command(TARGET test_app
        PRE_BUILD
        COMMAND ${CMAKE_SOURCE_DIR}/src/format.sh
        COMMENT "Formating...")
