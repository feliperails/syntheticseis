cmake_minimum_required (VERSION 3.12 FATAL_ERROR)
project (Core)

find_package(Qt5 COMPONENTS REQUIRED Core)

add_library(InvertSeisCore SHARED
    src/Core.h
    src/Core.cpp
    src/CoreDefs.h
#    src/Entity.cpp
#    src/Entity.h
#    src/DomainObject.h
#    src/DomainObject.cpp
#    src/MetaClass.h
#    src/MetaClass.cpp
    )

add_library(InvertSeisCoreMock SHARED mock/CoreMock.h mock/CoreMock.cpp)

include(GenerateExportHeader)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
generate_export_header(InvertSeisCore
    BASE_NAME CORE
    PREFIX_NAME INVERTSEIS_
    EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/core/CoreExport.h)
add_definitions(-DInvertSeisCore_EXPORTS)
set(INCLUDE_DIRECTORIES)

install(FILES
 CoreExport.h
 ${PROJECT_BINARY_DIR}/core/CoreExport.h DESTINATION .
)

add_library(InvertSeisCoreTest SHARED test/CoreTest.cpp)

target_link_libraries(InvertSeisCore
    PUBLIC Qt5::Core)

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/CPackConfig.cmake)
    include(${CMAKE_CURRENT_SOURCE_DIR}/CPackConfig.cmake)
endif()
