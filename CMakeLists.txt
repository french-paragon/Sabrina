cmake_minimum_required(VERSION 2.8.9)
project(Sabrina)

#global options

option(VERSION_WARNINGS "display warnings to the user when the app is tagged alpha or beta." OFF)
option(LICENSE_WARNINGS "trigger the user to accept the license if it hasn't already." OFF)

if(VERSION_WARNINGS)
    add_definitions(-DVERSION_WARNINGS_ON)
endif(VERSION_WARNINGS)

if(LICENSE_WARNINGS)
    add_definitions(-DLICENSE_WARNINGS_ON)
endif(LICENSE_WARNINGS)

#cross compilation
unset(MXE_CROSS_COMPILE)
if (CMAKE_CROSS_COMPILING AND CMAKE_SYSTEM_NAME STREQUAL Windows)
    set(MXE_CROSS_COMPILE 1)
    add_definitions(-DCROSS_COMPILE_MXE)
endif (CMAKE_CROSS_COMPILING AND CMAKE_SYSTEM_NAME STREQUAL Windows)

#set (CMAKE_CXX_STANDARD 11) # we use C++ version 11
set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")

#Qt

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5QuickWidgets REQUIRED)
find_package(Qt5Test REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

#libaries

set(BUILD_SHARED_LIBS ON)

#prog outputs
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

#include dirs

include_directories(${CMAKE_SOURCE_DIR}/libs)
include_directories(${CMAKE_SOURCE_DIR}/modules)

#libs directory

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/libs)
link_directories(${CMAKE_BINARY_DIR}/libs)

#sub dirs

add_subdirectory(modules)
add_subdirectory(libs)
add_subdirectory(prog)
add_subdirectory(tests)

#copy important files

configure_file(${CMAKE_SOURCE_DIR}/LICENSE ${CMAKE_BINARY_DIR}/LICENSE COPYONLY)
