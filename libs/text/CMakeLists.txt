set(LIB_NAME Text)
set(LIB_SRC text_global.h
	        textnode.h
			textnode.cpp
			abstracttextstyle.h
			abstracttextstyle.cpp
			textstylemanager.h
			textstylemanager.cpp
			comicscript.h
			comicscript.cpp)

add_library(${LIB_NAME} ${LIB_SRC})

target_compile_definitions(${LIB_NAME}
  PRIVATE SABRINA_TEXT_LIBRARY)

target_link_libraries(${LIB_NAME} Qt5::Core Qt5::Gui)
