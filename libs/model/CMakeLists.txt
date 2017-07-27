set(LIB_NAME Model)
set(LIB_SRC editableitem.cpp
            editableitemfactory.cpp
            editableitemmanager.cpp
            editableItemsManagers/jsoneditableitemmanager.cpp
            model_global.h
            ressources/ressources.qrc
            editableItems/personnage.cpp
            editableItems/place.cpp
            editableItems/folder.cpp)

add_library(${LIB_NAME} ${LIB_SRC})

target_compile_definitions(${LIB_NAME}
  PRIVATE CATHIA_MODEL_LIBRARY)

QT5_USE_MODULES(${LIB_NAME} Core)
