set(PROG_NAME ${CMAKE_PROJECT_NAME})
set(PROG_SRC main.cpp)

add_executable(${PROG_NAME} ${PROG_SRC})

target_link_libraries(${PROG_NAME} Qt5::Core)
target_link_libraries(${PROG_NAME} Qt5::Widgets)
target_link_libraries(${PROG_NAME} Qt5::Gui)

target_link_libraries(${PROG_NAME} Model Gui Core Aline)
