# CMAKE BUILD FILE FOR CREATING A STATIC VROOMGIS Library
# (c) LUCIEN SCHREIBER 2013 
# VROOMGIS NEEDS wxWidgets, GEOS, GDAL


# VROOMGIS LIB
SET(VROOMGIS_LIBRARY "vroomgis")

# FINDING VROOMGIS PATH
FIND_PATH(VROOMGIS_PATH "vroomgis.h"
        "src"
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
        "/vroomgis/src"
        "../vroomgis/src/"
        "../../vroomgis/src/"
        "src"
        "../../../../vroomgis/src/"
        "../../../../../vroomgis/src/"
        NO_DEFAULT_PATH)

IF (NOT VROOMGIS_PATH)
    MESSAGE("src")
    MESSAGE(FATAL_ERROR "Path not found, not able to use the vroomgis library. You may fill the VROOMGIS_PATH manually")

ENDIF (NOT VROOMGIS_PATH)
#MESSAGE ("VroomGIS found here : ${VROOMGIS_PATH}")

# ADDITION OF CUSTOM MODULES
set(CMAKE_MODULE_PATH ${VROOMGIS_PATH}/../build/cmake)

# NEEDED LIBRARY
OPTION(SEARCH_VROOMGIS_LIBS "Sould we search for libraries for vroomGIS ?" ON)

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
