set(LIB_NAME Model)
set(LIB_SRC editableitem.cpp
	        editableitem.h
			editableitemmanager.cpp
			editableitemmanager.h
			editableItemsManagers/jsoneditableitemmanager.cpp
			editableItemsManagers/jsoneditableitemmanager.h
            model_global.h
			ressources/ressources.qrc
			editableItems/personnage.cpp
			editableItems/personnage.h
			editableItems/place.cpp
			editableItems/place.h
			editableItems/folder.cpp
			editableItems/folder.h
			editableItems/cartography.cpp
			editableItems/cartography.h
			editableItems/comicscript.cpp
			editableItems/comicscript.h
            notes/noteslist.cpp)

add_library(${LIB_NAME} ${LIB_SRC})

target_compile_definitions(${LIB_NAME}
  PRIVATE CATHIA_MODEL_LIBRARY)

target_link_libraries(${LIB_NAME} Qt5::Core)

target_link_libraries(${LIB_NAME} Aline)

target_link_libraries(${LIB_NAME} Utils)
