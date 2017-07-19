#config module aline
set(CMAKE_CXX_FLAGS_OLD ${CMAKE_CXX_FLAGS})
add_subdirectory(aline)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS_OLD})
