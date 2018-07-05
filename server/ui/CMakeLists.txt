cmake_minimum_required(VERSION 3.6.3)
project(server_ui)

set (CMAKE_CXX_STANDARD 14)

file(GLOB SOURCES "*.cpp")
set(SOURCES main.cpp 
	libs/imgui-master/examples/imgui_impl_glfw.cpp
	libs/imgui-master/examples/imgui_impl_opengl3.cpp
	libs/imgui-master/imgui.cpp
	libs/imgui-master/imgui_demo.cpp
	libs/imgui-master/imgui_draw.cpp
	libs/imgui-master/examples/libs/gl3w/GL/gl3w.c
	)

add_executable(sv_ui ${SOURCES})

include_directories(
	libs/imgui-master/
 	libs/imgui-master/examples/ 
	libs/imgui-master/examples/libs/gl3w/
	)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW3 REQUIRED glfw3)
target_include_directories(sv_ui PUBLIC ${GLFW3_INCLUDE_DIRS})

find_package(OpenGL)

find_library(SERVER_LINK_LIB server ${CMAKE_SOURCE_DIR}/server/lib/)
include_directories(${CMAKE_SOURCE_DIR}/server/lib/include/)
include_directories(${CMAKE_SOURCE_DIR}/protocol/include/)

find_library(PROTOCOL_LINK_LIB Protocol ${CMAKE_SOURCE_DIR}/protocol/)


target_link_libraries(sv_ui ${GLFW3_LIBRARIES} ${OPENGL_LIBRARIES} ${SERVER_LINK_LIB} ${PROTOCOL_LINK_LIB} dl)

#findnk_libraries(testapp ${SDL2_LIBRARIES})
#include_directories(${CMAKE_SOURCE_DIR}/server/)
#message("found lib"  ${SERVER_LINK_LIB})

#target_link_libraries(sv_run ${SERVER_LINK_LIB})
