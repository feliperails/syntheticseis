cmake_minimum_required (VERSION 3.12 FATAL_ERROR)
project (Gui)

add_library(InvertSeisGui SHARED src/Gui.h src/Gui.cpp)

add_library(InvertSeisGuiMock SHARED mock/GuiMock.h mock/GuiMock.cpp)

add_library(InvertSeisGuiTest SHARED test/GuiTest.cpp)

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/CPackConfig.cmake)
    include(${CMAKE_CURRENT_SOURCE_DIR}/CPackConfig.cmake)
endif()
