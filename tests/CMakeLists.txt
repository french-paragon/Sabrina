add_executable(testCharacterEditableItem testcharactereditableitem.cpp)

target_link_libraries(testCharacterEditableItem Qt5::Core)
target_link_libraries(testCharacterEditableItem Qt5::Test)

target_link_libraries(testCharacterEditableItem Model Core)

target_link_libraries(testCharacterEditableItem Aline::Aline_tests)

add_test(TestCharacterEditableItem testCharacterEditableItem)



add_executable(testTextNode testtextnode.cpp)

target_link_libraries(testTextNode Qt5::Core)
target_link_libraries(testTextNode Qt5::Test)

target_link_libraries(testTextNode Text Core)

add_test(TestTextNode testTextNode)

add_executable(mockupComicTextEdit textEditorComicScriptMockup.cpp)

target_link_libraries(mockupComicTextEdit Qt5::Core)
target_link_libraries(mockupComicTextEdit Qt5::Gui)

target_link_libraries(mockupComicTextEdit Text Gui Core)
