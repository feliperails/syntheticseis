cmake_minimum_required (VERSION 3.12 FATAL_ERROR)
project (Widgets)

add_library(InvertSeisWidgets SHARED src/Widgets.h src/Widgets.cpp)

add_library(InvertSeisWidgetsMock SHARED mock/WidgetsMock.h mock/WidgetsMock.cpp)

add_library(InvertSeisWidgetsTest SHARED test/WidgetsTest.cpp)

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/CPackConfig.cmake)
    include(${CMAKE_CURRENT_SOURCE_DIR}/CPackConfig.cmake)
endif()
