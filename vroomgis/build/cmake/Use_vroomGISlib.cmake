# CMAKE BUILD FILE FOR CREATING A STATIC VROOMGIS Library
# (c) LUCIEN SCHREIBER 2013 
# VROOMGIS NEEDS wxWidgets, GEOS, GDAL

# USAGE :
# Call INCLUDE(Use_vroomGISlib.cmake)
# TARGET_LINK_LIBRARIES(Your_program ${VROOMGIS_LIBRARY})


# VROOMGIS LIB
SET(VROOMGIS_LIBRARY "vroomgis")

# FINDING VROOMGIS PATH
FIND_PATH(VROOMGIS_PATH "vroomgis.h"
        "${CMAKE_CURRENT_LIST_DIR}/../../src"
        "../plugins/vroomgis/src/"
        "../../plugins/vroomgis/src/"
        "../../../plugins/vroomgis/src/"
        "../../../../plugins/vroomgis/src/"
        "../../../../../plugins/vroomgis/src/"
        "/lib/vroomgis/src/"
        "../lib/vroomgis/src/"
        "../../lib/vroomgis/src/"
        "../../../lib/vroomgis/src/"
        "../../../../lib/vroomgis/src/"
        "../../../../../lib/vroomgis/src/"
        "/vroomgis/src/"
        "../vroomgis/src/"
        "../../vroomgis/src/"
        "../../../vroomgis/src/"
        "../../../../vroomgis/src/"
        "../../../../../vroomgis/src/"
        NO_DEFAULT_PATH)

IF (NOT VROOMGIS_PATH)
    MESSAGE("${CMAKE_CURRENT_LIST_DIR}/../../src")
    MESSAGE(FATAL_ERROR "Path not found, not able to use the vroomgis library. You may fill the VROOMGIS_PATH manually")

ENDIF (NOT VROOMGIS_PATH)
#MESSAGE ("VroomGIS found here : ${VROOMGIS_PATH}")

# ADDITION OF CUSTOM MODULES
set(CMAKE_MODULE_PATH ${VROOMGIS_PATH}/../build/cmake)

# NEEDED LIBRARY
SET(SEARCH_VROOMGIS_LIBS CACHE BOOL "Sould we search for libraries for vroomGIS ?")
IF (SEARCH_VROOMGIS_LIBS)
    mark_as_advanced(wxWidgets_wxrc_EXECUTABLE)
    find_package(wxWidgets COMPONENTS core base adv xml qa REQUIRED)
    include("${wxWidgets_USE_FILE}")
    include_directories(${wxWidgets_INCLUDE_DIRS})
    link_libraries(${wxWidgets_LIBRARIES})

    FIND_PACKAGE(GEOS PATHS "${CMAKE_CURRENT_LIST_DIR}/")

    #GDAL
    #find_package(GDAL REQUIRED)
    #include_directories(${GDAL_INCLUDE_DIRS})
    #link_libraries(${GDAL_LIBRARIES})

    INCLUDE ("${VROOMGIS_PATH}/../build/cmake/findGISLib.cmake")

    # TODO: SQLITE SHOULD BE OUTSIDE OF VROOMGIS
    #SQLITE libs
    #IF(NOT "${SQLITE_PATH}" STREQUAL "NO")
    #	INCLUDE ("${VROOMGIS_PATH}/../build/cmake/findsqlite.cmake")
    #ELSE(NOT "${SQLITE_PATH}" STREQUAL "NO")
    #	MESSAGE("SQLite ignored!")
    #ENDIF(NOT "${SQLITE_PATH}" STREQUAL "NO")
ENDIF (SEARCH_VROOMGIS_LIBS)

#CREATE LIB WITH ALL MAIN PROJECT CLASS
#search all source files
FILE(GLOB VROOMGIS_LIB_SRC_FILES "${VROOMGIS_PATH}/*.cpp")
FILE(GLOB VROOMGIS_LIB_ART_FILES "${VROOMGIS_PATH}/../art/*.cpp")
SOURCE_GROUP(source FILES ${VROMGIS_LIB_SRC_FILES} ${VROOMGIS_LIB_ART_FILES})

#COUNT SRC FILES IN VROOMGIS
LIST(LENGTH VROOMGIS_LIB_SRC_FILES MY_LENGTH)
IF (NOT MY_LENGTH)
    MESSAGE(FATAL_ERROR "${MY_LENGTH} source found for vroomGIS library")
ENDIF (NOT MY_LENGTH)

# search all headers files
FILE(GLOB VROOMGIS_LIB_HEAD_FILES "${VROOMGIS_PATH}/*.h")
FILE(GLOB VROOMGIS_LIB_HEAD_ART_FILES "${VROOMGIS_PATH}/../art/*.h")
SOURCE_GROUP(header FILES ${VROOMGIS_LIB_HEAD_FILES} ${VROOMGIS_LIB_HEAD_ART_FILES})
INCLUDE_DIRECTORIES("${VROOMGIS_PATH}")
INCLUDE_DIRECTORIES("${VROOMGIS_PATH}/../art")

ADD_LIBRARY(${VROOMGIS_LIBRARY} STATIC ${VROOMGIS_LIB_SRC_FILES}
        ${VROOMGIS_LIB_ART_FILES}
        ${VROOMGIS_LIB_HEAD_FILES}
        ${VROOMGIS_LIB_HEAD_ART_FILES})

# USED FOR PERFORMANCE UNDER WINDOWS
IF (WIN32)
    TARGET_LINK_LIBRARIES(${VROOMGIS_LIBRARY} "psapi")
ENDIF (WIN32)
