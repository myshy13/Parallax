cmake_minimum_required(VERSION 3.20)
project(parallax LANGUAGES C CXX)

set(name "parallax")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
if(EMSCRIPTEN)
    set(PLATFORM Web CACHE STRING "" FORCE)
endif()

FetchContent_Declare(raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(raylib)
# Keeps our warning flags from firing on raylib's headers.
set_target_properties(raylib PROPERTIES
    INTERFACE_SYSTEM_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:raylib,INTERFACE_INCLUDE_DIRECTORIES>)

option(USE_GAMEPAD_INPUT "Use input_gamepad.cpp instead of the platform's input backend" OFF)
if(USE_GAMEPAD_INPUT)
    set(INPUT_BACKEND input_gamepad.cpp)
else()
    set(INPUT_BACKEND input_kbm.cpp)
endif()

file(GLOB_RECURSE sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

foreach(f input_kbm.cpp input_touch.cpp input_gamepad.cpp)
    if(NOT f STREQUAL INPUT_BACKEND)
        list(REMOVE_ITEM sources "${CMAKE_CURRENT_SOURCE_DIR}/src/Input/InputSources/${f}")
    endif()
endforeach()

add_executable(${name} ${sources})
target_include_directories(${name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(${name} PRIVATE raylib)

if(MSVC)
    target_compile_options(${name} PRIVATE /W4)
else()
    target_compile_options(${name} PRIVATE -Wall -Wextra)
endif()

if(EMSCRIPTEN)
    target_compile_definitions(${name} PRIVATE GLSL_VERSION=100)
    set_target_properties(${name} PROPERTIES
        SUFFIX ".html"
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/dist)
    target_link_options(${name} PRIVATE
        -sUSE_GLFW=3
        -sASYNCIFY
        "SHELL:--shell-file ${raylib_SOURCE_DIR}/src/minshell.html"
        "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/assets@/assets")
    # Files are packed at link time, so relink when one changes.
    file(GLOB_RECURSE resources CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/assets/*)
    set_target_properties(${name} PROPERTIES LINK_DEPENDS "${resources}")
else()
    target_compile_definitions(${name} PRIVATE GLSL_VERSION=330)
    # A custom target runs on every build, so asset edits get copied without a relink.
    add_custom_target(${name}_resources
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${name}>/assets)
    add_dependencies(${name} ${name}_resources)
endif()
