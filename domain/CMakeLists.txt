cmake_minimum_required (VERSION 3.13 FATAL_ERROR)
project (Domain)

add_library(SyntheticSeisDomain SHARED src/Domain.h src/Domain.cpp)

add_library(SyntheticSeisDomainMock SHARED mock/DomainMock.h mock/DomainMock.cpp)

add_library(SyntheticSeisDomainTest SHARED test/DomainTest.cpp test/ConvertRegularGridCalculatorTest.cpp)

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/CPackConfig.cmake)
    include(${CMAKE_CURRENT_SOURCE_DIR}/CPackConfig.cmake)
endif()
