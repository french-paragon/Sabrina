set(LIB_NAME Model)
set(LIB_SRC editableitem.cpp
            editableitemmanager.cpp
            model_global.h
            ressources/ressources.qrc)

add_library(${LIB_NAME} ${LIB_SRC})

target_compile_definitions(${LIB_NAME}
  PRIVATE CATHIA_MODEL_LIBRARY)

QT5_USE_MODULES(${LIB_NAME} Core)
