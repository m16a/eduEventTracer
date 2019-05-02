cmake_minimum_required(VERSION 3.6.3)

set (CMAKE_CXX_STANDARD 14)
#Bring the headers, such as Student.h into the project
include_directories(include)

file(GLOB SOURCES *.cc *.cpp)

add_library(Protocol ${SOURCES})

find_package(Protobuf)
target_link_libraries(Protocol ${PROTOBUF_LIBRARIES})

#formating
add_custom_command(TARGET Protocol
        PRE_BUILD
        COMMAND ${CMAKE_SOURCE_DIR}/src/format.sh
        COMMENT "Formating...")
