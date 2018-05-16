set(LIB_NAME Core)
set(LIB_SRC app.cpp
            core_global.h)

add_library(${LIB_NAME} ${LIB_SRC})

target_compile_definitions(${LIB_NAME}
  PRIVATE CATHIA_CORE_LIBRARY)

QT5_USE_MODULES(${LIB_NAME} Core)
QT5_USE_MODULES(${LIB_NAME} Widgets)
QT5_USE_MODULES(${LIB_NAME} Gui)

target_link_libraries(${LIB_NAME} Gui Model Utils)
target_link_libraries(${LIB_NAME} Aline)
