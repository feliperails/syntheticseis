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

find_package(Qt5 COMPONENTS REQUIRED Core Widgets Gui Test)
find_package(Threads REQUIRED)
find_package(GTest REQUIRED)
#find_package(GMock REQUIRED)

include_directories(${GTEST_INCLUDE_DIR})

add_library(InvertSeisLibrary

    # core
    core/src/Core.h
    core/src/Core.cpp
    core/src/CoreDefs.h
    core/src/Entity.cpp
    core/src/Entity.h
    core/src/DomainObject.h
    core/src/DomainObject.cpp
    core/src/MetaClass.h
    core/src/MetaClass.cpp

    # storage
    storage/src/Storage.h
    storage/src/Storage.cpp

    # domain
    domain/src/Domain.h
    domain/src/Domain.cpp
    domain/src/EclipseGrid.h
    domain/src/EclipseGrid.cpp

    # dataIO
    dataIO/src/DataIO.h
    dataIO/src/DataIO.cpp
    dataIO/src/reader/EclipseGridReader.h
    dataIO/src/reader/EclipseGridReader.cpp
    dataIO/src/writer/EclipseGridWriter.h
    dataIO/src/writer/EclipseGridWriter.cpp

    # gui
    gui/src/Gui.h
    gui/src/Gui.cpp
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
    )

add_executable(InvertSeis
    # app
    app/src/main.cpp
    )

add_executable(InvertSeisTest
    cmake/TestMain.cpp
    core/test/CoreTest.cpp
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
    PUBLIC Qt5::Widgets)

target_link_libraries(InvertSeis
    PUBLIC Qt5::Core
    PUBLIC Qt5::Gui
    PUBLIC Qt5::Widgets
    PUBLIC InvertSeisLibrary)

target_link_libraries(InvertSeisTest
    PUBLIC Qt5::Core
    PUBLIC Qt5::Gui
    PUBLIC Qt5::Widgets
    PUBLIC Qt5::Test
    PUBLIC InvertSeisLibrary
    PUBLIC GTest::GTest
#    PUBLIC GMock::GMock
)

if(WIN32)
    target_compile_definitions(InvertSeisTest PRIVATE -DGTEST_LINKED_AS_SHARED_LIBRARY )
endif()

install(FILES README.md
    version.txt
    DESTINATION .
    PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
    )



enable_testing()

add_test(
    NAME InvertSeisTest
    COMMAND InvertSeisTest
)

if (EXISTS ${CMAKE_SOURCE_DIR}/CPackConfig.cmake)
    include(${CMAKE_SOURCE_DIR}/CPackConfig.cmake)
endif()
