cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "CMAKE_SKIP_RPATH: ${CMAKE_SKIP_RPATH}")

if (APPLE)
    set(CMAKE_INSTALL_RPATH "@executable_path")
else()
    set(CMAKE_INSTALL_RPATH "$ORIGIN")
endif()

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
    add_executable(vroomgis-tests ${src_tests} ${head_tests})
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
if (NOT UNIT_TESTING_PATH)
    message(FATAL_ERROR "UNIT_TESTING_PATH is not set. Please set it to the location of your unit testing files and projects.")
endif ()
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/test_param.h.in" "${PROJECT_BINARY_DIR}/test_param.h")
include_directories(${PROJECT_BINARY_DIR})

# LINK LIBRARIES
target_link_libraries(vroomgis-tests vroomgis)
# use system PNG for linux otherwise we have problem when displaying icons
if (UNIX AND NOT APPLE)
    message(STATUS "PNG lib for test is : ${PNG_LIBRARY}")
    target_link_libraries(vroomgis-tests ${PNG_LIBRARY} xkbcommon)
endif (UNIX AND NOT APPLE)

set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}")
message(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")
install(TARGETS vroomgis-tests
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

# Visual Leak Detector
if (WIN32)
    if (USE_VLD)
        target_link_libraries(vroomgis-tests ${VLD_LIBRARIES})
    endif (USE_VLD)
endif (WIN32)