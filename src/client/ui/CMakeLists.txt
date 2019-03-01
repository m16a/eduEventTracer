cmake_minimum_required(VERSION 3.6.3)
project(client_ui)

set (CMAKE_CXX_STANDARD 14)

file(GLOB SOURCES "*.cpp")
set(SOURCES main.cpp 
	${CMAKE_SOURCE_DIR}/3rd/imgui-master/examples/imgui_impl_glfw.cpp
	${CMAKE_SOURCE_DIR}/3rd/imgui-master/examples/imgui_impl_opengl3.cpp
	${CMAKE_SOURCE_DIR}/3rd/imgui-master/imgui.cpp
	${CMAKE_SOURCE_DIR}/3rd/imgui-master/imgui_demo.cpp
	${CMAKE_SOURCE_DIR}/3rd/imgui-master/imgui_draw.cpp
	${CMAKE_SOURCE_DIR}/3rd/imgui-master/examples/libs/gl3w/GL/gl3w.c
		TimeLine.cpp TimeLine.h App.cpp App.h Timer.h)

add_executable(client_ui ${SOURCES})

include_directories(
	${CMAKE_SOURCE_DIR}/3rd/imgui-master/
 	${CMAKE_SOURCE_DIR}/3rd/imgui-master/examples/ 
	${CMAKE_SOURCE_DIR}/3rd/imgui-master/examples/libs/gl3w/
	)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW3 REQUIRED glfw3)
target_include_directories(client_ui PUBLIC ${GLFW3_INCLUDE_DIRS})

find_package(OpenGL)

find_library(CLIENT_LINK_LIB server ${CMAKE_SOURCE_DIR}/src/client/lib/)
include_directories(${CMAKE_SOURCE_DIR}/src/client/lib/include/)
include_directories(${CMAKE_SOURCE_DIR}/src/protocol/include/)

find_library(PROTOCOL_LINK_LIB Protocol ${CMAKE_SOURCE_DIR}/src/protocol/)


target_link_libraries(client_ui ${GLFW3_LIBRARIES} ${OPENGL_LIBRARIES} ${CLIENT_LINK_LIB} ${PROTOCOL_LINK_LIB} dl pthread)

#findnk_libraries(testapp ${SDL2_LIBRARIES})
#include_directories(${CMAKE_SOURCE_DIR}/server/)
#message("found lib"  ${CLIENT_LINK_LIB})

#target_link_libraries(sv_run ${CLIENT_LINK_LIB})

#formating
add_custom_command(TARGET client_ui
		PRE_BUILD
		COMMAND ${CMAKE_SOURCE_DIR}/src/format.sh
		COMMENT "Formating...")
