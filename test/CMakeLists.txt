# Project name
project(vroomgis-tests)

#search all source files
file(GLOB src_tests "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(vroomgis-tests ${src_tests})

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

include_directories(${CMAKE_SOURCE_DIR}/vroomgis/src)

# LINK LIBRARIES
add_dependencies(vroomgis-tests vroomgis)
target_link_libraries(vroomgis-tests vroomgis)
target_link_libraries(vroomgis-tests CONAN_PKG::gtest)
target_link_libraries(vroomgis-tests CONAN_PKG::wxwidgets)
