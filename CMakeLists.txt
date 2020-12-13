cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
file(STRINGS "version.txt" VERSION_STRING)

project (SyntheticSeismic LANGUAGES CXX VERSION ${VERSION_STRING})

add_definitions(-D_USE_MATH_DEFINES)

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
set(GTEST_LINKED_AS_SHARED_LIBRARY 1)


execute_process(
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/configure.bat
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

## Download automatically, you can also just copy the conan.cmake file
#if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake" )
#    message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
#    file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.15/conan.cmake"
#        "${CMAKE_BINARY_DIR}/conan.cmake"
#        TLS_VERIFY ON)
#endif()
#include(${CMAKE_BINARY_DIR}/conan.cmake)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/conan.cmake)

# Versão do conan
conan_check(VERSION 1.25.0 REQUIRED)

conan_cmake_run(ninja/1.10.1
    BASIC_SETUP
    BUILD missing)

conan_cmake_run(REQUIRES hdf5/1.8.21
    BASIC_SETUP
    BUILD missing)

conan_cmake_run(REQUIRES gtest/1.8.1
    BASIC_SETUP
    BUILD missing)

conan_cmake_run(REQUIRED GMock/1.8.0@iblis_ms/stable
    BASIC_SETUP
    BUILD missing)

conan_cmake_run(REQUIRES cgal/5.1
    BASIC_SETUP
    BUILD missing)

conan_cmake_run(REQUIRES pthreads4w/3.0.0
    BASIC_SETUP
    BUILD missing)

add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)

find_package(OpenMP)
find_package(Qt5 COMPONENTS REQUIRED Core Test Widgets)
find_package(Threads REQUIRED)

include_directories(${GTEST_INCLUDE_DIR})
include_directories(${HDF5_INCLUDE_DIRS})

# FIXME Retirar essa aberração.
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_library(SyntheticSeismic SHARED

    # geometry
    geometry/src/Coordinate.h
    geometry/src/Coordinate.cpp
    geometry/src/Point3D.h
    geometry/src/Point3D.cpp
    geometry/src/Volume.h
    geometry/src/Volume.cpp
    geometry/src/Point2D.h
    geometry/src/Point2D.cpp

    # domain
    domain/src/ConvolutionRegularGridCalculator.cpp
    domain/src/DomainConstant.cpp
    domain/src/DomainConstant.h
    domain/src/ExtractMinimumRectangle2D.cpp
    domain/src/ExtractMinimumRectangle2D.h
    domain/src/ExtractVolumes.h
    domain/src/ExtractVolumes.cpp
    domain/src/Facade.h
    domain/src/Facade.cpp
    domain/src/EclipseGrid.h
    domain/src/EclipseGrid.cpp
    domain/src/Lithology.h
    domain/src/Lithology.cpp
    domain/src/LithologyDictionary.h
    domain/src/LithologyDictionary.cpp
    domain/src/ImpedanceRegularGridCalculator.h
    domain/src/ImpedanceRegularGridCalculator.cpp
    domain/src/RegularGrid.h
    domain/src/RegularGrid.cpp
    domain/src/ReflectivityRegularGridCalculator.h
    domain/src/ReflectivityRegularGridCalculator.cpp
    domain/src/RickerWaveletCalculator.h
    domain/src/RickerWaveletCalculator.cpp
    domain/src/RotateVolumeCoordinate.h
    domain/src/RotateVolumeCoordinate.cpp
    domain/src/SeismicTraces.h
    domain/src/SeismicTraces.cpp
    domain/src/VolumeToRegularGrid.h
    domain/src/VolumeToRegularGrid.cpp
    domain/src/Wavelet.h
    domain/src/Wavelet.cpp
    # storage
    storage/src/Facade.h
    storage/src/Facade.cpp
    storage/src/reader/EclipseGridReader.h
    storage/src/reader/EclipseGridReader.cpp
    storage/src/writer/EclipseGridWriter.h
    storage/src/writer/EclipseGridWriter.cpp
    storage/src/writer/SEGYWriter.h
    storage/src/writer/SEGYWriter.cpp
    storage/src/writer/RegularGridHdf5Writer.h
    storage/src/writer/RegularGridHdf5Writer.cpp
    )

qt5_add_resources(RESOURCES console/images/SyntheticSeisApp.qrc)

add_library(SyntheticSeismicMock
    # geometry
    geometry/mock/GeometryMock.cpp

    # domain
    domain/mock/DomainMock.h

    # storage
    storage/mock/StorageMock.cpp
    )

add_executable(SyntheticSeismicTest
    cmake/TestMain.cpp
    geometry/test/GeometryTest.cpp
    domain/test/DomainTestValues.cpp
    domain/test/DomainTestValues.h
    domain/test/DomainTest.cpp
    storage/test/StorageTest.cpp
    )

add_executable(SyntheticSeismicConsole
    console/main.cpp
    console/Wizard.cpp
    console/Wizard.h
    ${RESOURCES}
    )

#add_library(SyntheticSeismicCoreTest SHARED test/CoreTest.cpp)
include(GenerateExportHeader)
generate_export_header(SyntheticSeismic BASE_NAME SyntheticSeismic)

target_link_libraries(SyntheticSeismic
    PUBLIC Qt5::Core
    PUBLIC OpenMP::OpenMP_CXX
    PUBLIC ${HDF5_LIBRARIES}
    PUBLIC ${GTEST_LIBRARIES}
    PUBLIC ${CONAN_LIBS}
)

target_link_libraries(SyntheticSeismicMock
    PUBLIC Qt5::Core
    PUBLIC ${CONAN_LIBS}
    PUBLIC ${HDF5_LIBRARIES}
    PUBLIC SyntheticSeismic
    )

target_link_libraries(SyntheticSeismicTest
    PUBLIC Qt5::Core
    PUBLIC Qt5::Test
    PUBLIC ${CONAN_LIBS}
    PUBLIC SyntheticSeismic
    PUBLIC SyntheticSeismicMock
    PUBLIC ${HDF5_LIBRARIES}
    PUBLIC ${CONAN_LIBS}
    PUBLIC ${GTEST_LIBRARIES}
    )

target_link_libraries(SyntheticSeismicConsole
    PUBLIC Qt5::Core
    PUBLIC Qt5::Test
    PUBLIC Qt5::Widgets
    PUBLIC ${CONAN_LIBS}
    PUBLIC SyntheticSeismic
    PUBLIC SyntheticSeismicMock
    PUBLIC ${HDF5_LIBRARIES}
    PUBLIC ${CONAN_LIBS}
    PUBLIC ${GTEST_LIBRARIES}
    )

if(WIN32)
    include(GetPrerequisites)

    #get_filename_component(@_package_upper_name@_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)
    set(SEARCH_PREFIX_PATH "${GTest_ROOT}/lib" ${Qt5_DIR})
    MESSAGE(STATUS "SEARCH_PREFIX_PATH: ${SEARCH_PREFIX_PATH}")
    set(_search_paths "${CMAKE_PREFIX_PATH};${CMAKE_BINARY_DIR}/bin;${SEARCH_PREFIX_PATH}")
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
