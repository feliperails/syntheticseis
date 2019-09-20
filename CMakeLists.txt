cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
file(STRINGS "version.txt" VERSION_STRING)

project (InvertSeis LANGUAGES CXX VERSION ${VERSION_STRING})

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


find_package(Qt5 COMPONENTS REQUIRED Core Widgets Gui)

add_executable(InvertSeis
    core/src/Core.h
    core/src/Core.cpp
    core/src/CoreDefs.h
    #    src/Entity.cpp
    #    src/Entity.h
    #    src/DomainObject.h
    #    src/DomainObject.cpp
    #    src/MetaClass.h
    #    src/MetaClass.cpp

    # storage
    storage/src/Storage.h
    storage/src/Storage.cpp

    # domain
    domain/src/Domain.h
    domain/src/Domain.cpp

    # dataIO
    dataIO/src/DataIO.h
    dataIO/src/DataIO.cpp

    # gui
    gui/src/Gui.h
    gui/src/Gui.cpp
    gui/src/MainWindow.cpp
    gui/src/MainWindow.cpp
    gui/src/MainWindow.ui
    gui/src/SplashScreen.ui
    gui/src/images/Images.qrc

    # gui
    projectManager/src/ProjectManager.h
    projectManager/src/ProjectManager.cpp

    # gui
    widgets/src/Widgets.h
    widgets/src/Widgets.cpp

    # app
    app/src/main.cpp
    )

#add_library(InvertSeisMock SHARED mock/CoreMock.h mock/CoreMock.cpp)

#install(FILES
#    CoreExport.h
#    ${CURRENT_BUILD_DIR}/CoreExport.h DESTINATION .
#    )

#add_library(InvertSeisCoreTest SHARED test/CoreTest.cpp)

target_link_libraries(InvertSeis
    PUBLIC Qt5::Core
    PUBLIC Qt5::Gui
    PUBLIC Qt5::Widgets)

install(FILES README.md
    version.txt
    DESTINATION .
    PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
    )

enable_testing()

if (EXISTS ${CMAKE_SOURCE_DIR}/CPackConfig.cmake)
    include(${CMAKE_SOURCE_DIR}/CPackConfig.cmake)
endif()
