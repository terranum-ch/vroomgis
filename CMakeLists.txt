# CMAKE BUILD FILE FOR CREATING A STATIC VROOMGIS Library
# (c) LUCIEN SCHREIBER 2013 
# VROOMGIS NEEDS wxWidgets, GEOS, GDAL
cmake_minimum_required(VERSION 2.6)

project("vroomgis")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

include_directories(${CONAN_INCLUDE_DIRS})

add_subdirectory(vroomgis)

option(BUILD_TESTS "Do you want to build the tests (recommended) ?" ON)

if (BUILD_TESTS)
    add_subdirectory(test)
endif()
