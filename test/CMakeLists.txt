# CMAKE BUILD FILE FOR TESTING VROOMGIS
# USE WXWIDGETS (CONSOLE)
# PART OF QA FOR VROOMGIS 
# (c) LUCIEN SCHREIBER CREALP 2010
cmake_minimum_required(VERSION 2.6)

## POLICY FOR WINDOWS ONLY ???
if (COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

# REMOVE THIS LINE IF CALLED FROM ANOTHER CMAKE FILE
project("vroomgis_test")

#search all source files
file(GLOB SRC_FILES src/*.cpp)
source_group(source FILES ${SRC_FILES})
file(GLOB HEAD_FILES "src/*.h")
source_group(header FILES ${HEAD_FILES})

#ONLY USED FOR ADDING HEADERS TO PROJECT 
IF (APPLE)
    add_executable("${CMAKE_PROJECT_NAME}-not_build" EXCLUDE_FROM_ALL main.cpp ${HEAD_FILES})
ENDIF (APPLE)

# GTK LIB IF LINUX
message("System name is : " ${CMAKE_SYSTEM_NAME})
IF (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message("Systeme linux detecte")
    include("../vroomgis/build/cmake/FindGTK2.cmake")
    include_directories(${GTK2_INCLUDE_DIRS})
    message("Gtk include is " ${GTK2_INCLUDE_DIRS})
ENDIF (CMAKE_SYSTEM_NAME STREQUAL "Linux")

#CREATE VROOMGIS LIBRARY
include("../vroomgis/CMakeLists.txt")

#PARAMETERS FOR TEST
set(UNIT_TESTING_PATH CACHE PATH "Location of unit testing files and projects")
IF (NOT UNIT_TESTING_PATH)
    message(FATAL_ERROR "Unit testing path not specified, tests are going to fail")
ENDIF (NOT UNIT_TESTING_PATH)
configure_file("src/test_param.h.in" "${PROJECT_BINARY_DIR}/test_param.h")
include_directories(${PROJECT_BINARY_DIR})

# include_directories(${CXXTEST_INCLUDE_DIR})
find_package(CxxTest)
if(CXXTEST_FOUND)
    set(CXXTEST_USE_PYTHON 1)
    include_directories(${CXXTEST_INCLUDE_DIR})
    enable_testing()
    cxxtest_add_test(${CMAKE_PROJECT_NAME} main.cpp ${HEAD_FILES})
endif()

# LINK LIBRARIES
target_link_libraries(${CMAKE_PROJECT_NAME} ${VROOMGIS_LIBRARY})
target_link_libraries(${CMAKE_PROJECT_NAME} CONAN_PKG::wxwidgets)

