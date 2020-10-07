cmake_minimum_required (VERSION 3.13 FATAL_ERROR)
project (Domain)

add_library(InvertSeisDomain SHARED src/Domain.h src/Domain.cpp)

add_library(InvertSeisDomainMock SHARED mock/DomainMock.h mock/DomainMock.cpp)

add_library(InvertSeisDomainTest SHARED test/DomainTest.cpp)

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/CPackConfig.cmake)
    include(${CMAKE_CURRENT_SOURCE_DIR}/CPackConfig.cmake)
endif()
