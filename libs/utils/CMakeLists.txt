set(LIB_NAME Utils)
set(LIB_SRC stringutils.cpp
            utils_global.h)


add_library(${LIB_NAME} ${LIB_SRC})

target_compile_definitions(${LIB_NAME}
  PRIVATE CATHIA_UTILS_LIBRARY)

QT5_USE_MODULES(${LIB_NAME} Core)
