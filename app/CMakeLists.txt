cmake_minimum_required (VERSION 3.12 FATAL_ERROR)
project (App)

find_package(Qt5 COMPONENTS REQUIRED Core Gui)

add_executable(InvertSeis src/main.cpp)

target_link_libraries(InvertSeis
    PUBLIC Qt5::Core
    PUBLIC Qt5::Gui
    PUBLIC InvertSeisCore)
