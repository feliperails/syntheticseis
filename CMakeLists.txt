cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
file(STRINGS "version.txt" VERSION_STRING)

project (InvertSeis LANGUAGES CXX VERSION ${VERSION_STRING})

#include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/FindGMock.cmake)
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")
list(REMOVE_DUPLICATES CMAKE_PREFIX_PATH)

if(EXISTS ${CMAKE_SOURCE_DIR}/CMakeLists.config)
    include(${CMAKE_SOURCE_DIR}/CMakeLists.config)
endif()

set(INVERTSEIS_ROOT ${CMAKE_INSTALL_PREFIX})
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

#add_subdirectory(core)
#add_subdirectory(storage)
#add_subdirectory(domain)
#add_subdirectory(dataIO)
#add_subdirectory(gui)
#add_subdirectory(projectManager)
#add_subdirectory(widgets)
#add_subdirectory(app)

find_package(Qt5 COMPONENTS REQUIRED Core Widgets Gui Test DataVisualization)
find_package(Threads REQUIRED)
find_package(GTest REQUIRED)
#find_package(GMock REQUIRED)

include_directories(${GTEST_INCLUDE_DIR})

add_library(InvertSeisLibrary

    # core
    core/src/Core.h
    core/src/Core.cpp
    core/src/CoreDefs.h
    core/src/MetaClass.h
    core/src/MetaClass.cpp

    geometry/src/Geometry.h
    geometry/src/Geometry.cpp
    geometry/src/Coordinate.h
    geometry/src/Coordinate.cpp

    data/src/Data.h
    data/src/Data.cpp
    data/src/Dictionary.h
    data/src/Dictionary.cpp
    data/src/Entity.h
    data/src/Entity.cpp
    data/src/EntityManager.h
    data/src/EntityManager.cpp
    data/src/DomainObject.h
    data/src/DomainObject.cpp

    # storage
    storage/src/Storage.h
    storage/src/Storage.cpp

    # domain
    domain/src/Domain.h
    domain/src/Domain.cpp
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

    # dataIO
    dataIO/src/DataIO.h
    dataIO/src/DataIO.cpp
    dataIO/src/reader/EclipseGridReader.h
    dataIO/src/reader/EclipseGridReader.cpp
    dataIO/src/writer/EclipseGridWriter.h
    dataIO/src/writer/EclipseGridWriter.cpp
    dataIO/src/writer/SEGYWriter.h
    dataIO/src/writer/SEGYWriter.cpp

    # gui
    gui/src/Gui.h
    gui/src/Gui.cpp
    gui/src/GuiConstants.h
    gui/src/GuiConstants.cpp
    gui/src/MainWindow.cpp
    gui/src/MainWindow.cpp
    gui/src/MainWindow.ui
    gui/src/SplashScreen.ui
    gui/src/images/Images.qrc

    # projectManager
    projectManager/src/ProjectManager.h
    projectManager/src/ProjectManager.cpp

    # widgets
    widgets/src/Widgets.h
    widgets/src/Widgets.cpp
    widgets/src/EclipseGridView.cpp
    )

add_executable(InvertSeis
    # app
    app/src/main.cpp
    )

add_library(InvertSeisMock

    #c ore
    core/mock/CoreMock.cpp

    # geometry
    geometry/mock/GeometryMock.cpp

    #data
    data/mock/DataMock.cpp
    data/mock/DomainObjectMock.cpp

    # dataIO
    dataIO/mock/DataIOMock.cpp

    #domain
    domain/mock/DomainMock.cpp

    storage/mock/StorageMock.cpp
    )

add_executable(InvertSeisTest
    cmake/TestMain.cpp
    core/test/CoreTest.cpp
    geometry/test/GeometryTest.cpp
    data/test/DataTest.cpp
    dataIO/test/DataIOTest.cpp
    domain/test/DomainTest.cpp
    storage/test/StorageTest.cpp
    )

#add_library(InvertSeisMock SHARED mock/CoreMock.h mock/CoreMock.cpp)

#install(FILES
#    CoreExport.h
#    ${CURRENT_BUILD_DIR}/CoreExport.h DESTINATION .
#    )

#add_library(InvertSeisCoreTest SHARED test/CoreTest.cpp)
include(GenerateExportHeader)
generate_export_header(InvertSeisLibrary BASE_NAME InvertSeis)

target_link_libraries(InvertSeisLibrary
    PUBLIC Qt5::Core
    PUBLIC Qt5::Gui
    PUBLIC Qt5::DataVisualization
    PUBLIC Qt5::Widgets)

target_link_libraries(InvertSeis
    PUBLIC Qt5::Core
    PUBLIC Qt5::Gui
    PUBLIC Qt5::Widgets
    PUBLIC InvertSeisLibrary)

target_link_libraries(InvertSeisMock
    PUBLIC Qt5::Core
    PUBLIC Qt5::Gui
    PUBLIC Qt5::Widgets
    PUBLIC Qt5::Test
    PUBLIC Qt5::DataVisualization
    PUBLIC InvertSeisLibrary
    PUBLIC GTest::GTest
    #    PUBLIC GMock::GMock
    )

target_link_libraries(InvertSeisTest
    PUBLIC Qt5::Core
    PUBLIC Qt5::Gui
    PUBLIC Qt5::Widgets
    PUBLIC Qt5::Test
    PUBLIC InvertSeisLibrary
    PUBLIC InvertSeisMock
    PUBLIC GTest::GTest
    #    PUBLIC GMock::GMock
    )



if(WIN32)
    target_compile_definitions(InvertSeisTest PRIVATE -DGTEST_LINKED_AS_SHARED_LIBRARY)

    include(GetPrerequisites)

    #get_filename_component(@_package_upper_name@_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)
    set(SEARCH_PREFIX_PATH "${GTest_ROOT}/lib" ${Qt5_DIR})
    MESSAGE(STATUS "SEARCH_PREFIX_PATH: ${SEARCH_PREFIX_PATH}")
    set(_search_paths ${CMAKE_PREFIX_PATH};${CMAKE_BINARY_DIR}/bin;${SEARCH_PREFIX_PATH})
    string(REPLACE ";" "%3b" _search_paths "${_search_paths}")

    message(STATUS "SEARCH_PATHS: " ${_search_paths})

    add_custom_command(TARGET InvertSeisTest
        POST_BUILD
        COMMAND ${CMAKE_COMMAND}
        #COMMAND copy_if_different
        -DTARGET_EXEC_NAME=InvertSeisTest.exe
        -DTARGET_EXEC_DIR=${CMAKE_BINARY_DIR}/bin
        -DPREREQUISITES_DEST_DIR=${CMAKE_BINARY_DIR}/bin
        -DSEARCH_PATHS=${_search_paths}
        -DTARGET_LOCAL_DEPS=$<TARGET_FILE:InvertSeisTest>
        -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/ConfigureBuildPrerequisites.cmake)

endif()

install(FILES README.md
    version.txt
    DESTINATION .
    PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
    )

# Arquivo para teste de leitura do EclipseGrid
file(COPY "${CMAKE_SOURCE_DIR}/dataIO/test/EclipseGridTest.grdecl" DESTINATION  "${CMAKE_BINARY_DIR}/bin/")
file(COPY "${CMAKE_SOURCE_DIR}/dataIO/test/EclipseGridPerformanceTest.grdecl" DESTINATION  "${CMAKE_BINARY_DIR}/bin/")

enable_testing()

add_test(
    NAME InvertSeisTest
    COMMAND InvertSeisTest
    )

if (EXISTS ${CMAKE_SOURCE_DIR}/CPackConfig.cmake)
    include(${CMAKE_SOURCE_DIR}/CPackConfig.cmake)
endif()
