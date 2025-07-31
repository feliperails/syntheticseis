cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
file(STRINGS "version.txt" VERSION_STRING)

project (SyntheticSeismic LANGUAGES CXX VERSION ${VERSION_STRING})

add_definitions(-D_USE_MATH_DEFINES)

set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")
list(REMOVE_DUPLICATES CMAKE_PREFIX_PATH)

if(EXISTS ${CMAKE_SOURCE_DIR}/CMakeLists.config)
    include(${CMAKE_SOURCE_DIR}/CMakeLists.config)
endif()

message("*********************************************************")
message("${CMAKE_PREFIX_PATH}")

set(SyntheticSeismic_ROOT ${CMAKE_INSTALL_PREFIX})
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(GTEST_LINKED_AS_SHARED_LIBRARY 1)



add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)

include_directories(${GTEST_INCLUDE_DIR})
include_directories(${HDF5_INCLUDE_DIRS})

find_package(OpenMP)
find_package(Qt5 COMPONENTS REQUIRED Core Test Widgets Concurrent)
find_package(CGAL REQUIRED COMPONENTS Core)
find_package(Threads REQUIRED)
find_package(GMP REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(VTK REQUIRED)





# FIXME Retirar essa aberração.
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

FILE(GLOB SyntheticSeis_UI "console/*.ui")

qt5_wrap_ui(
    Plugin_UI_HEADERS
    ${SyntheticSeis_UI}
)

add_library(SyntheticSeismic SHARED
    # geometry
    geometry/src/Coordinate.cpp
    geometry/src/EnumUnit.cpp
    geometry/src/Point3D.cpp
    geometry/src/Volume.cpp
    geometry/src/Point2D.cpp

    # domain
    domain/src/CellSizeCalculator.h
    domain/src/CellSizeCalculator.cpp
    domain/src/ConvertRegularGridCalculator.cpp
    domain/src/ConvolutionRegularGridCalculator.cpp
    domain/src/DomainConstant.cpp
    domain/src/EclipseGridSurface.cpp
    domain/src/EquationPlane.cpp
    domain/src/ExtractMinimumRectangle2D.cpp
    domain/src/ExtractVolumes.cpp
    domain/src/Facade.cpp
    domain/src/EclipseGrid.cpp
    domain/src/GrdSurface.cpp
    domain/src/Lithology.cpp
    domain/src/LithologyDictionary.cpp
    domain/src/ImpedanceRegularGridCalculator.cpp
    domain/src/RegularGrid.cpp
    domain/src/ReflectivityRegularGridCalculator.cpp
    domain/src/RickerWaveletCalculator.cpp
    domain/src/RotateVolumeCoordinate.cpp
    domain/src/SeismicTraces.cpp
    domain/src/VolumeToRegularGrid.cpp
    domain/src/Wavelet.cpp
    # storage
    storage/src/Facade.cpp
    storage/src/RegularGridHdf5Storage.cpp
    storage/src/reader/EclipseGridReader.cpp
    storage/src/reader/GrdSurfaceReader.cpp
    storage/src/writer/EclipseGridWriter.cpp
    storage/src/writer/SegyWriter.cpp
    storage/src/writer/GrdSurfaceWriter.cpp
    # infrastructure
    infrastructure/src/OperatingSystemCommand.cpp
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
    domain/test/DomainTest.cpp
    domain/test/ConvertRegularGridCalculatorTest.cpp
    domain/test/ConvolutionRegularGridCalculatorTest.cpp
    storage/test/StorageTestValues.cpp
    storage/test/StorageTest.cpp
    domain/test/ConvertRegularGridToMetersCalculatorTest.cpp
    domain/test/ConvertRegularGridTestValues.cpp
    domain/test/LithologyDictionaryTest.cpp
    domain/test/ImpedanceCalculatorTest.cpp
    domain/test/ReflectivityCalculatorTest.cpp
    domain/test/RickerWaveletCalculatorTest.cpp
    domain/test/EclipseGridSurfaceTest.cpp
    domain/test/ExtractMinimumRectangle2DTest.cpp
)

add_executable(SyntheticSeismicConsole
    console/main.cpp
    console/Wizard.cpp
    ${Plugin_UI_HEADERS}
    ${RESOURCES}
        console/FileSelectionPage.cpp
        console/FileSelectionPage.h
        console/SegyCreationPage.cpp
        console/SegyCreationPage.h
        console/VtkViewerDialog.cpp
        console/VtkViewerDialog.h
        console/EclipseGridWorker.cpp
        console/EclipseGridWorker.h
        console/RegularGridWorker.cpp
        console/RegularGridWorker.h
)

#add_library(SyntheticSeismicCoreTest SHARED test/CoreTest.cpp)
include(GenerateExportHeader)
generate_export_header(SyntheticSeismic BASE_NAME SyntheticSeismic)

target_link_libraries(SyntheticSeismic
    PUBLIC Qt5::Core
    PUBLIC OpenMP::OpenMP_CXX
    PUBLIC ${GMP_LIBRARIES}/gmp.lib
    PUBLIC CGAL::CGAL
    PUBLIC CGAL::CGAL_Core
    PUBLIC ${HDF5_LIBRARIES}
    PUBLIC ${GTEST_LIBRARIES}
)

target_link_libraries(SyntheticSeismicMock
    PUBLIC Qt5::Core
    PUBLIC ${GMP_LIBRARIES}/gmp.lib
    PUBLIC CGAL::CGAL
    PUBLIC CGAL::CGAL_Core
    PUBLIC ${HDF5_LIBRARIES}
    PUBLIC SyntheticSeismic
)

target_link_libraries(SyntheticSeismicTest
    PUBLIC Qt5::Core
    PUBLIC Qt5::Test
    PUBLIC ${GMP_LIBRARIES}/gmp.lib
    PUBLIC CGAL::CGAL
    PUBLIC CGAL::CGAL_Core
    PUBLIC SyntheticSeismic
    PUBLIC SyntheticSeismicMock
    PUBLIC ${HDF5_LIBRARIES}
    PUBLIC GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main
)

target_link_libraries(SyntheticSeismicConsole
    PUBLIC Qt5::Core
    PUBLIC Qt5::Test
    PUBLIC Qt5::Widgets
    PUBLIC Qt5::Concurrent
    PUBLIC SyntheticSeismic
    PUBLIC SyntheticSeismicMock
    PUBLIC ${HDF5_LIBRARIES}
    PUBLIC ${VTK_LIBRARIES}
    PUBLIC GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main
)

if(WIN32)
    include(GetPrerequisites)

    #get_filename_component(@_package_upper_name@_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)
    set(SEARCH_PREFIX_PATH "${GTest_ROOT}/lib" ${Qt5_DIR})
    set(SEARCH_PREFIX_PATH "${GLEW_ROOT}/lib" ${Qt5_DIR})
    set(SEARCH_PREFIX_PATH "${VTK_ROOT}/lib" ${Qt5_DIR})
    message("Qt dir: ${Qt5_DIR} \n\n")
    get_cmake_property(_variableNames VARIABLES)
    foreach(_variableName ${_variableNames})
        message("${_variableName}=${${_variableName}}")
    endforeach()
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
        -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/ConfigureBuildPrerequisites.cmake
    )

    if(CMAKE_BUILD_TYPE MATCHES Debug)
        file(COPY "${Qt5_DIR}/../../../bin/platforms"
             DESTINATION "${CMAKE_BINARY_DIR}/bin/")


        file(GLOB DLLS_TO_COPY "${VTK_DIR}/bin/Debug/*.dll")

    else()
        file(COPY "${Qt5_DIR}/../../../bin/platforms"
             DESTINATION "${CMAKE_BINARY_DIR}/bin/")

        file(GLOB DLLS_TO_COPY "${VTK_DIR}/bin/Release/*.dll")

    endif()

    foreach(dll ${DLLS_TO_COPY})
        file(COPY "${dll}" DESTINATION "${CMAKE_BINARY_DIR}/bin/")
    endforeach()

    #IF(CMAKE_BUILD_TYPE MATCHES Debug)
    #    file(COPY "${Qt5_DIR}/../../../bin/Qt5Cored.dll" DESTINATION "${CMAKE_BINARY_DIR}/bin/")
    #else()
    #    file(COPY "${Qt5_DIR}/../../../bin/Qt5Core.dll" DESTINATION "${CMAKE_BINARY_DIR}/bin/")
    #endif()
endif()

install(FILES README.md
    version.txt
    DESTINATION .
    PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
)

# Arquivo de execução Python
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/bin/python")
file(COPY "${CMAKE_SOURCE_DIR}/python/generate_segy.py" DESTINATION "${CMAKE_BINARY_DIR}/bin/python/")

# Arquivo para teste de leitura do EclipseGrid
file(COPY "${CMAKE_SOURCE_DIR}/storage/test/EclipseGridTest.grdecl" DESTINATION "${CMAKE_BINARY_DIR}/bin/")
file(COPY "${CMAKE_SOURCE_DIR}/storage/test/EclipseGridPerformanceTest.grdecl" DESTINATION "${CMAKE_BINARY_DIR}/bin/")
file(COPY "${CMAKE_SOURCE_DIR}/storage/test/gridMollesImpedance_5_5_5.h5" DESTINATION "${CMAKE_BINARY_DIR}/bin/")
file(COPY "${CMAKE_SOURCE_DIR}/storage/test/grdDsaaTest.grd" DESTINATION "${CMAKE_BINARY_DIR}/bin/")

enable_testing()

add_test(
    NAME SyntheticSeismicTest
    COMMAND SyntheticSeismicTest
)

if (EXISTS ${CMAKE_SOURCE_DIR}/CPackConfig.cmake)
    include(${CMAKE_SOURCE_DIR}/CPackConfig.cmake)
endif()
