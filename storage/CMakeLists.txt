cmake_minimum_required (VERSION 3.12 FATAL_ERROR)
project (Storage)

add_library(SyntheticSeismicStorage SHARED
    src/Facade.h
    src/Facade.cpp)

add_library(SyntheticSeismicStorageMock SHARED
    mock/StorageMock.h
    mock/StorageMock.cpp)

add_library(SyntheticSeismicStorageTest SHARED
    test/SyntheticSeismicStorageTest.cpp)

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/CPackConfig.cmake)
    include(${CMAKE_CURRENT_SOURCE_DIR}/CPackConfig.cmake)
endif()
