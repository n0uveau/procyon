cmake_minimum_required(VERSION 3.28...4.4)
project(Procyon LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

option(PROCYON_WERROR "Treat warnings as errors" OFF)

add_library(procyon_warnings INTERFACE)
target_compile_options(procyon_warnings INTERFACE
    $<$<CXX_COMPILER_ID:MSVC>:/W4;/permissive->
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall;-Wextra;-Wpedantic;-Wconversion;-Wshadow>)
target_compile_options(procyon_warnings INTERFACE
    $<$<AND:$<BOOL:${PROCYON_WERROR}>,$<CXX_COMPILER_ID:MSVC>>:/WX>
    $<$<AND:$<BOOL:${PROCYON_WERROR}>,$<NOT:$<CXX_COMPILER_ID:MSVC>>>:-Werror>)

include(FetchContent)
set(SFML_BUILD_AUDIO OFF)
set(SFML_BUILD_NETWORK OFF)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        3.1.0
    GIT_SHALLOW    ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

add_subdirectory(src)