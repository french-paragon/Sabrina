#git
find_package (Git)

if (GIT_FOUND)
    message("git found: ${GIT_EXECUTABLE} in version ${GIT_VERSION_STRING}")

    exec_program(
        "git"
        ${CMAKE_CURRENT_SOURCE_DIR}
        ARGS "describe"
        OUTPUT_VARIABLE GIT_TAG )

    exec_program(
        "git"
        ${CMAKE_CURRENT_SOURCE_DIR}
        ARGS "describe --abbrev=0"
        OUTPUT_VARIABLE GIT_SHORT_TAG )

    if( ${GIT_TAG} MATCHES "fatal:.*" )
        set( GIT_TAG v0.0a )
        set( GIT_SHORT_TAG v0.0a )
    endif()

    exec_program(
      "git"
      ${CMAKE_CURRENT_SOURCE_DIR}
      ARGS "rev-parse HEAD"
    OUTPUT_VARIABLE GIT_HASH )

else()
    set( GIT_TAG unset )
endif (GIT_FOUND)


exec_program(
    "date"
    ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE COMPILE_TIME_FULL )

exec_program(
    "date"
    ${CMAKE_CURRENT_SOURCE_DIR}
    ARGS "+%d.%m.%Y"
    OUTPUT_VARIABLE COMPILE_DATE )


exec_program(
    "touch"
    ${CMAKE_CURRENT_SOURCE_DIR}
    ARGS "app_info.cpp.in")

configure_file(app_info.cpp.in app_info.cpp)

set(LIB_NAME Utils)
set(LIB_SRC utils_global.h
            app_info.h
            settings_global_keys.h
            ${CMAKE_CURRENT_BINARY_DIR}/app_info.cpp)

add_library(${LIB_NAME} ${LIB_SRC})

target_compile_definitions(${LIB_NAME}
  PRIVATE CATHIA_UTILS_LIBRARY)

QT5_USE_MODULES(${LIB_NAME} Core)
