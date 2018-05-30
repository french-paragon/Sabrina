set(LIB_NAME Gui)
set(LIB_SRC mainwindows.cpp
            mainwindowsfactory.cpp
            gui_global.h
            actions/editableitemtypespecializedaction.cpp
            utilsDialogs/aboutdialog.cpp
            utilsDialogs/aboutdialog.ui
            dockWidgets/projecttreedockwidget.cpp
            dockWidgets/projecttreedockwidget.ui
            dockWidgets/projectlabelsdockwidget.cpp
            dockWidgets/projectlabelsdockwidget.ui
            dockWidgets/labelitemsdockwidget.cpp
            dockWidgets/labelitemsdockwidget.ui
            dockWidgets/projectnotesdockwidget.cpp
            dockWidgets/projectnotesdockwidget.ui
            editors/personnageeditor.cpp
            editors/personnageeditor.ui
            editors/placeeditor.cpp
            editors/placeeditor.ui
            delegates/labelsmodelitemdelegate.cpp
            widgets/noteseditwidget.cpp
            widgets/noteseditwidget.ui
            ressources/ressources_gui.qrc)

add_library(${LIB_NAME} ${LIB_SRC})

target_compile_definitions(${LIB_NAME}
  PRIVATE CATHIA_GUI_LIBRARY)

QT5_USE_MODULES(${LIB_NAME} Core)
QT5_USE_MODULES(${LIB_NAME} Widgets)
QT5_USE_MODULES(${LIB_NAME} Gui)
QT5_USE_MODULES(${LIB_NAME} QuickWidgets)

target_link_libraries(${LIB_NAME} Aline Model)
