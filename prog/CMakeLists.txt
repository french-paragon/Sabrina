set(PROG_NAME ${CMAKE_PROJECT_NAME})
set(PROG_SRC main.cpp)

add_executable(${PROG_NAME} ${PROG_SRC})

QT5_USE_MODULES(${PROG_NAME} Core)
QT5_USE_MODULES(${PROG_NAME} Widgets)
QT5_USE_MODULES(${PROG_NAME} Gui)

target_link_libraries(${PROG_NAME} Model Gui Core)
