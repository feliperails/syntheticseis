cmake_minimum_required (VERSION 3.12 FATAL_ERROR)
project (dataIO)

add_library(InvertSeisDataIO SHARED src/DataIO.h src/DataIO.cpp)

add_library(InvertSeisDataIOMock SHARED mock/DataIOMock.h mock/DataIOMock.cpp)

add_library(InvertSeisDataIOTest SHARED test/DataIOTest.cpp)

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/CPackConfig.cmake)
    include(${CMAKE_CURRENT_SOURCE_DIR}/CPackConfig.cmake)
endif()
