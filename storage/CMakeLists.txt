cmake_minimum_required (VERSION 3.12 FATAL_ERROR)
project (Storage)

add_library(InvertSeisStorage SHARED src/Storage.h src/Storage.cpp)

add_library(InvertSeisStorageMock SHARED mock/StorageMock.h mock/StorageMock.cpp)

add_library(InvertSeisStorageTest SHARED test/StorageTest.cpp)

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/CPackConfig.cmake)
    include(${CMAKE_CURRENT_SOURCE_DIR}/CPackConfig.cmake)
endif()

