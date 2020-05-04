cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
file(STRINGS "version.txt" VERSION_STRING)

project (SyntheticSeismic LANGUAGES CXX VERSION ${VERSION_STRING})

#include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/FindGMock.cmake)
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")
list(REMOVE_DUPLICATES CMAKE_PREFIX_PATH)

if(EXISTS ${CMAKE_SOURCE_DIR}/CMakeLists.config)
    include(${CMAKE_SOURCE_DIR}/CMakeLists.config)
endif()

set(SyntheticSeismic_ROOT ${CMAKE_INSTALL_PREFIX})
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS REQUIRED Core Test)
find_package(Threads REQUIRED)
find_package(GTest REQUIRED)

include_directories(${GTEST_INCLUDE_DIR})

# FIXME Retirar essa aberração.
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_library(SyntheticSeismic SHARED

    # geometry
    geometry/src/Coordinate.h
    geometry/src/Coordinate.cpp

    # domain
    domain/src/Facade.h
    domain/src/Facade.cpp
    domain/src/EclipseGrid.h
    domain/src/EclipseGrid.cpp
    domain/src/Lithology.h
    domain/src/Lithology.cpp
    domain/src/LithologyDictionary.h
    domain/src/LithologyDictionary.cpp
    domain/src/RegularGrid.h
    domain/src/RegularGrid.cpp
    domain/src/SeismicTraces.h
    domain/src/SeismicTraces.cpp
    domain/src/SeismicWaveVelocityDictionary.h
    domain/src/SeismicWaveVelocityDictionary.cpp
    domain/src/ExtractVolumes.h
    domain/src/ExtractVolumes.cpp

    # storage
    storage/src/Facade.h
    storage/src/Facade.cpp
    storage/src/reader/EclipseGridReader.h
    storage/src/reader/EclipseGridReader.cpp
    storage/src/writer/EclipseGridWriter.h
    storage/src/writer/EclipseGridWriter.cpp
    storage/src/writer/SEGYWriter.h
    storage/src/writer/SEGYWriter.cpp
    )

add_library(SyntheticSeismicMock
    # geometry
    geometry/mock/GeometryMock.cpp

    # domain
    domain/mock/DomainMock.cpp

    # storage
    storage/mock/StorageMock.cpp
    )

add_executable(SyntheticSeismicTest
    cmake/TestMain.cpp
    geometry/test/GeometryTest.cpp
    domain/test/DomainTest.cpp
    storage/test/StorageTest.cpp
    )

#install(FILES
#    CoreExport.h
#    ${CURRENT_BUILD_DIR}/CoreExport.h DESTINATION .
#    )

#add_library(SyntheticSeismicCoreTest SHARED test/CoreTest.cpp)
include(GenerateExportHeader)
generate_export_header(SyntheticSeismic BASE_NAME SyntheticSeismic)

target_link_libraries(SyntheticSeismic
    PUBLIC Qt5::Core)

#target_link_libraries(SyntheticSeismic
#    PUBLIC Qt5::Core
#    PUBLIC SyntheticSeismic)

target_link_libraries(SyntheticSeismicMock
    PUBLIC Qt5::Core
    PUBLIC SyntheticSeismic
    PUBLIC GTest::GTest
    #    PUBLIC GMock::GMock
    )

target_link_libraries(SyntheticSeismicTest
    PUBLIC Qt5::Core
    PUBLIC Qt5::Test
    PUBLIC SyntheticSeismic
    PUBLIC SyntheticSeismicMock
    PUBLIC GTest::GTest
    #    PUBLIC GMock::GMock
    )



if(WIN32)
    target_compile_definitions(SyntheticSeismicTest PRIVATE -DGTEST_LINKED_AS_SHARED_LIBRARY)

    include(GetPrerequisites)

    #get_filename_component(@_package_upper_name@_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)
    set(SEARCH_PREFIX_PATH "${GTest_ROOT}/lib" ${Qt5_DIR})
    MESSAGE(STATUS "SEARCH_PREFIX_PATH: ${SEARCH_PREFIX_PATH}")
    set(_search_paths ${CMAKE_PREFIX_PATH};${CMAKE_BINARY_DIR}/bin;${SEARCH_PREFIX_PATH})
    string(REPLACE ";" "%3b" _search_paths "${_search_paths}")

    message(STATUS "SEARCH_PATHS: " ${_search_paths})

    add_custom_command(TARGET SyntheticSeismicTest
        POST_BUILD
        COMMAND ${CMAKE_COMMAND}
        #COMMAND copy_if_different
        -DTARGET_EXEC_NAME=SyntheticSeismicTest.exe
        -DTARGET_EXEC_DIR=${CMAKE_BINARY_DIR}/bin
        -DPREREQUISITES_DEST_DIR=${CMAKE_BINARY_DIR}/bin
        -DSEARCH_PATHS=${_search_paths}
        -DTARGET_LOCAL_DEPS=$<TARGET_FILE:SyntheticSeismicTest>
        -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/ConfigureBuildPrerequisites.cmake)

endif()

install(FILES README.md
    version.txt
    DESTINATION .
    PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
    )

# Arquivo para teste de leitura do EclipseGrid
file(COPY "${CMAKE_SOURCE_DIR}/storage/test/EclipseGridTest.grdecl" DESTINATION  "${CMAKE_BINARY_DIR}/bin/")
file(COPY "${CMAKE_SOURCE_DIR}/storage/test/EclipseGridPerformanceTest.grdecl" DESTINATION  "${CMAKE_BINARY_DIR}/bin/")

enable_testing()

add_test(
    NAME SyntheticSeismicTest
    COMMAND SyntheticSeismicTest
    )

if (EXISTS ${CMAKE_SOURCE_DIR}/CPackConfig.cmake)
    include(${CMAKE_SOURCE_DIR}/CPackConfig.cmake)
endif()
