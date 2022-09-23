# CMAKE BUILD FILE FOR CREATING A STATIC VROOMGIS Library
# (c) LUCIEN SCHREIBER 2013 
# VROOMGIS NEEDS wxWidgets, GEOS, GDAL

# ADDITION OF CUSTOM MODULES
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/build/cmake)

#CREATE LIB WITH ALL MAIN PROJECT CLASS
#search all source files
file(GLOB VROOMGIS_LIB_SRC_FILES "${CMAKE_SOURCE_DIR}/vroomgis/src/*.cpp")
file(GLOB VROOMGIS_LIB_ART_FILES "${CMAKE_SOURCE_DIR}/vroomgis/art/*.cpp")
source_group(source FILES ${VROMGIS_LIB_SRC_FILES} ${VROOMGIS_LIB_ART_FILES})

#COUNT SRC FILES IN VROOMGIS
list(LENGTH VROOMGIS_LIB_SRC_FILES MY_LENGTH)
IF (NOT MY_LENGTH)
    message(FATAL_ERROR "${MY_LENGTH} source found for vroomGIS library")
ENDIF (NOT MY_LENGTH)

# search all headers files
file(GLOB VROOMGIS_LIB_HEAD_FILES "${CMAKE_SOURCE_DIR}/vroomgis/src/*.h")
file(GLOB VROOMGIS_LIB_HEAD_ART_FILES "${CMAKE_SOURCE_DIR}/vroomgis/art/*.h")
source_group(header FILES ${VROOMGIS_LIB_HEAD_FILES} ${VROOMGIS_LIB_HEAD_ART_FILES})
include_directories("${CMAKE_SOURCE_DIR}/vroomgis/src")
include_directories("${CMAKE_SOURCE_DIR}/vroomgis/art")

add_library(vroomgis STATIC ${VROOMGIS_LIB_SRC_FILES}
        ${VROOMGIS_LIB_ART_FILES}
        ${VROOMGIS_LIB_HEAD_FILES}
        ${VROOMGIS_LIB_HEAD_ART_FILES})

target_link_libraries(vroomgis CONAN_PKG::wxwidgets)
target_link_libraries(vroomgis CONAN_PKG::gdal)
target_link_libraries(vroomgis CONAN_PKG::geos)

# USED FOR PERFORMANCE UNDER WINDOWS
IF (WIN32)
    target_link_libraries(vroomgis "psapi")
ENDIF (WIN32)
