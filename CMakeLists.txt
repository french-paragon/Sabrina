cmake_minimum_required(VERSION 2.8.9)
project(Sabrina)

#cross compilation
unset(MXE_CROSS_COMPILE)
if (CMAKE_CROSS_COMPILING AND CMAKE_SYSTEM_NAME STREQUAL Windows)
    set(MXE_CROSS_COMPILE 1)
    add_definitions(-DCROSS_COMPILE_MXE)
endif (CMAKE_CROSS_COMPILING AND CMAKE_SYSTEM_NAME STREQUAL Windows)

#set (CMAKE_CXX_STANDARD 11) # we use C++ version 11
set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")

#Qt

#find_package(Qt5Core REQUIRED)
#find_package(Qt5Gui REQUIRED)
#find_package(Qt5Widgets REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTOUIC ON)

#libaries

set(BUILD_SHARED_LIBS ON)

#include dirs

include_directories(${CMAKE_SOURCE_DIR}/libs)
include_directories(${CMAKE_SOURCE_DIR}/modules)

#sub dirs

add_subdirectory(modules)
add_subdirectory(libs)
add_subdirectory(prog)
