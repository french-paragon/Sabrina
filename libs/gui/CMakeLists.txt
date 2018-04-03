set(LIB_NAME Gui)
set(LIB_SRC mainwindows.cpp
            gui_global.h
            actions/editableitemtypespecializedaction.cpp
            utilsDialogs/aboutdialog.cpp
            utilsDialogs/aboutdialog.ui
            dockWidgets/projecttreedockwidget.cpp
            dockWidgets/projecttreedockwidget.ui
            editors/personnageeditor.cpp
            editors/personnageeditor.ui
            editors/placeeditor.cpp
            editors/placeeditor.ui
            ressources/ressources_gui.qrc)

add_library(${LIB_NAME} ${LIB_SRC})

target_compile_definitions(${LIB_NAME}
  PRIVATE CATHIA_GUI_LIBRARY)

QT5_USE_MODULES(${LIB_NAME} Core)
QT5_USE_MODULES(${LIB_NAME} Widgets)
QT5_USE_MODULES(${LIB_NAME} Gui)

target_link_libraries(${LIB_NAME} Aline Model)
