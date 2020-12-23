set(LIB_NAME Gui)
set(LIB_SRC mainwindows.cpp
	        mainwindows.h
			mainwindowsfactory.cpp
			mainwindowsfactory.h
			gui_global.h
			utilsDialogs/aboutdialog.cpp
			utilsDialogs/aboutdialog.h
			utilsDialogs/aboutdialog.ui
			utilsDialogs/licensedialog.cpp
			utilsDialogs/licensedialog.h
			utilsDialogs/licensedialog.ui
			dockWidgets/projectnotesdockwidget.cpp
			dockWidgets/projectnotesdockwidget.h
            dockWidgets/projectnotesdockwidget.ui
			editors/personnageeditor.cpp
			editors/personnageeditor.h
			editors/personnageeditor.ui
			editors/placeeditor.cpp
			editors/placeeditor.h
			editors/placeeditor.ui
			editors/cartographyeditor.cpp
			editors/cartographyeditor.h
			editors/cartographyeditor.ui
			editors/comicscripteditor.cpp
			editors/comicscripteditor.h
			editors/comicscripteditor.ui
			widgets/noteseditwidget.cpp
			widgets/noteseditwidget.h
            widgets/noteseditwidget.ui
			widgets/comicscripteditwidget.cpp
			widgets/comicscripteditwidget.h
            ressources/ressources_gui.qrc)

add_library(${LIB_NAME} ${LIB_SRC})

target_compile_definitions(${LIB_NAME}
  PRIVATE CATHIA_GUI_LIBRARY)

target_link_libraries(${LIB_NAME} Qt5::Core)
target_link_libraries(${LIB_NAME} Qt5::Widgets)
target_link_libraries(${LIB_NAME} Qt5::Gui)
target_link_libraries(${LIB_NAME} Qt5::QuickWidgets)

target_link_libraries(${LIB_NAME} Aline Model)
