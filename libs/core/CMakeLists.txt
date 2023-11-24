set(LIB_NAME Core)
set(LIB_SRC app.cpp
            core_global.h)

add_library(${LIB_NAME} ${LIB_SRC})

target_compile_definitions(${LIB_NAME}
  PRIVATE CATHIA_CORE_LIBRARY)

target_link_libraries(${LIB_NAME} Qt5::Core)
target_link_libraries(${LIB_NAME} Qt5::Widgets)
target_link_libraries(${LIB_NAME} Qt5::Gui)

target_link_libraries(${LIB_NAME} Gui Model Utils)
target_link_libraries(${LIB_NAME} Aline::Aline)
