add_executable(testCharacterEditableItem testcharactereditableitem.cpp)

target_link_libraries(testCharacterEditableItem Qt5::Core)
target_link_libraries(testCharacterEditableItem Qt5::Test)
target_link_libraries(testCharacterEditableItem Qt5::Test)

target_link_libraries(testCharacterEditableItem Model Core)

target_link_libraries(testCharacterEditableItem Aline_tests)


add_executable(mockupComicTextEdit textEditorComicScriptMockup.cpp)

target_link_libraries(mockupComicTextEdit Qt5::Core)
target_link_libraries(mockupComicTextEdit Qt5::Gui)

target_link_libraries(mockupComicTextEdit Text Gui Core)
