set(LIB_NAME Model)
set(LIB_SRC editableitem.cpp
            editableitemmanager.cpp
            editableItemsManagers/jsoneditableitemmanager.cpp
            model_global.h
            ressources/ressources.qrc
            editableItems/personnage.cpp
            editableItems/place.cpp
            editableItems/folder.cpp
            editableItems/cartography.cpp
            labels/labelstree.cpp
            labels/label.cpp
            labels/labelselectionforitemproxymodel.cpp
            labels/projectfilterbylabelproxymodel.cpp
            notes/noteslist.cpp)

add_library(${LIB_NAME} ${LIB_SRC})

target_compile_definitions(${LIB_NAME}
  PRIVATE CATHIA_MODEL_LIBRARY)

QT5_USE_MODULES(${LIB_NAME} Core)

target_link_libraries(${LIB_NAME} Aline)

target_link_libraries(${LIB_NAME} Utils)
