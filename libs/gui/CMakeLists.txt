set(LIB_NAME Gui)
set(LIB_SRC mainwindows.cpp
            mainwindowsfactory.cpp
            gui_global.h
            actions/editableitemtypespecializedaction.cpp
            utilsDialogs/aboutdialog.cpp
            utilsDialogs/aboutdialog.ui
            utilsDialogs/licensedialog.cpp
            utilsDialogs/licensedialog.ui
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
            editors/cartographyeditor.cpp
            editors/cartographyeditor.ui
            widgets/noteseditwidget.cpp
            widgets/noteseditwidget.ui
            ressources/ressources_gui.qrc)

add_library(${LIB_NAME} ${LIB_SRC})

target_compile_definitions(${LIB_NAME}
  PRIVATE CATHIA_GUI_LIBRARY)

target_link_libraries(${LIB_NAME} Qt5::Core)
target_link_libraries(${LIB_NAME} Qt5::Widgets)
target_link_libraries(${LIB_NAME} Qt5::Gui)
target_link_libraries(${LIB_NAME} Qt5::QuickWidgets)

target_link_libraries(${LIB_NAME} Aline Model)
