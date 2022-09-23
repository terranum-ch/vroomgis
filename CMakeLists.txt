# CMAKE BUILD FILE FOR CREATING A STATIC VROOMGIS Library
# (c) LUCIEN SCHREIBER 2013 
# VROOMGIS NEEDS wxWidgets, GEOS, GDAL
cmake_minimum_required(VERSION 3.11)

project("vroomgis")

# Conan setup
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)
include_directories(${CONAN_INCLUDE_DIRS})

# Global definitions
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Hide some wxWidgets-related warnings.
IF (WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
ENDIF (WIN32)

add_subdirectory(vroomgis)

option(BUILD_TESTS "Do you want to build the tests (recommended) ?" ON)

if (BUILD_TESTS)
    add_subdirectory(test)
endif()
