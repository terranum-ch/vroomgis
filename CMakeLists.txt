# CMAKE BUILD FILE FOR CREATING A STATIC VROOMGIS Library
# (c) LUCIEN SCHREIBER 2013 
# VROOMGIS NEEDS wxWidgets, GEOS, GDAL
cmake_minimum_required(VERSION 3.11)

project("vroomgis")

# searching PNG library before conan magic stuff
if(UNIX AND NOT APPLE)
    find_package(PNG REQUIRED)
    message(STATUS "PNG lib is : ${PNG_LIBRARY}")
endif(UNIX AND NOT APPLE)

# Conan setup
# using conan_basic_setup(TARGETS) leeds to linking error on Linux
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# Global definitions
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Hide some wxWidgets-related warnings.
IF (WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
ENDIF (WIN32)

add_subdirectory(vroomgis)

option(BUILD_TESTS "Do you want to build the tests (recommended) ?" ON)
option(BUILD_APPS "Do you want to build the apps ?" ON)

# Visual Leak Detector
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/build/cmake)
if (WIN32)
    option(USE_VLD "Sould we use Visual Leak Detector (https://vld.codeplex.com) ?" OFF)
    if (USE_VLD)
        find_package(VLD)
        include_directories(${VLD_INCLUDE_DIRS})
        add_definitions(-DUSE_VLD)
    endif (USE_VLD)
endif (WIN32)

if (BUILD_TESTS)
    add_subdirectory(test)
endif ()

if (BUILD_APPS)
    add_subdirectory(app/vroomdrawer)
    add_subdirectory(app/vroomloader)
    add_subdirectory(app/vroomtwin)
endif ()

