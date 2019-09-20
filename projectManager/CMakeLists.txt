cmake_minimum_required (VERSION 3.12 FATAL_ERROR)
project (ProjectManager)

add_library(InvertSeisProjectManager SHARED src/ProjectManager.h src/ProjectManager.cpp)

add_library(InvertSeisProjectManagerMock SHARED mock/ProjectManagerMock.h mock/ProjectManagerMock.cpp)

add_library(InvertSeisProjectManagerTest SHARED test/ProjectManagerTest.cpp)

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/CPackConfig.cmake)
    include(${CMAKE_CURRENT_SOURCE_DIR}/CPackConfig.cmake)
endif()
