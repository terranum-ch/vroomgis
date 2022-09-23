
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
option(UNIT_TESTING_PATH CACHE PATH "Location of unit testing files and projects")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/test_param.h.in" "${PROJECT_BINARY_DIR}/test_param.h")
include_directories(${PROJECT_BINARY_DIR})

include_directories(${VROOMGIS_PATH})

# LINK LIBRARIES
target_link_libraries(vroomgis-tests ${VROOMGIS_LIBRARY})
target_link_libraries(vroomgis-tests CONAN_PKG::gtest)

