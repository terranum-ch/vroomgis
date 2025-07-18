cmake_minimum_required(VERSION 3.20)
project("vroomgis")

# searching PNG library before conan magic stuff
if(UNIX AND NOT APPLE)
    find_package(PNG REQUIRED)
    message(STATUS "PNG lib is : ${PNG_LIBRARY}")
endif(UNIX AND NOT APPLE)

# Conan setup
# using conan_basic_setup(TARGETS) leeds to linking error on Linux
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(KEEP_RPATHS)
message(STATUS "Conan libs: ${CONAN_LIBS}")
if (APPLE)
    set(CMAKE_INSTALL_RPATH "@executable_path")
else()
    set(CMAKE_INSTALL_RPATH "$ORIGIN")
endif()

# Global definitions
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Hide some wxWidgets-related warnings.
IF (WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
ENDIF (WIN32)

## Code coverage for IDE, see https://www.jetbrains.com/help/clion/code-coverage-clion.html#compiler-flags
SET(USE_CODECOVERAGE_IDE OFF CACHE BOOL "Should we do code coverage for IDE ?")
if (USE_CODECOVERAGE_IDE)
    message(STATUS "Code coverage for IDE")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
    message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
endif (USE_CODECOVERAGE_IDE)



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
    enable_testing()
    add_subdirectory(test)
endif ()

if (BUILD_APPS)
    add_subdirectory(app/vroomdrawer)
    add_subdirectory(app/vroomloader)
    add_subdirectory(app/vroomtwin)
endif ()

