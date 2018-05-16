set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/libs)
link_directories(${CMAKE_BINARY_DIR}/libs)

add_subdirectory(model)
add_subdirectory(core)
add_subdirectory(gui)
add_subdirectory(utils)
