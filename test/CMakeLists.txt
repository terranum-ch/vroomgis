cmake_minimum_required(VERSION 3.11)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Project name
project(vroomgis-tests)

#search all source files
file(GLOB src_tests "src/*.cpp")
file(GLOB head_tests "src/*.cpp")

include_directories(${CMAKE_SOURCE_DIR}/vroomgis/src)

# Conditional compilation
IF (WIN32)
    add_executable(vroomgis-tests WIN32 ${src_tests} ${head_tests})
ELSEIF (APPLE)
    add_executable(vroomgis-tests MACOSX_BUNDLE ${src_tests} ${head_tests})
ELSE ()
    add_executable(vroomgis-tests ${src_tests} ${head_tests})
ENDIF ()

# GTK LIB IF LINUX
message(STATUS "System name is : " ${CMAKE_SYSTEM_NAME})
IF (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "System linux detected")
    find_package(GTK2 2.6 REQUIRED gtk)
    include_directories(${GTK2_INCLUDE_DIRS})
    message("Gtk include is " ${GTK2_INCLUDE_DIRS})
ENDIF (CMAKE_SYSTEM_NAME STREQUAL "Linux")

# PARAMETERS FOR TEST
set(UNIT_TESTING_PATH CACHE PATH "Location of unit testing files and projects")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/test_param.h.in" "${PROJECT_BINARY_DIR}/test_param.h")
include_directories(${PROJECT_BINARY_DIR})

# LINK LIBRARIES
target_link_libraries(vroomgis-tests vroomgis)
# use system PNG for linux otherwise we have problem when displaying icons
if (UNIX AND NOT APPLE)
    message(STATUS "PNG lib for test is : ${PNG_LIBRARY}")
    target_link_libraries(vroomgis-tests ${PNG_LIBRARY} xkbcommon)
endif (UNIX AND NOT APPLE)

# Visual Leak Detector
if (WIN32)
    if (USE_VLD)
        target_link_libraries(vroomgis-tests ${VLD_LIBRARIES})
    endif (USE_VLD)
endif (WIN32)