# CMAKE BUILD FILE FOR CREATING A STATIC
# VROOMGIS Library
# (c) LUCIEN SCHREIBER CREALP 2010
# VROOMGIS NEEDS wxWidgets and GEOS / GDAL

# PARAMETERS : 
# VR_LIBRARY_NAME - Name of the vroomgislibrary 

# USAGE :
# Call INCLUDE(Use_vroomGISlib.cmake)
# TARGET_LINK_LIBRARIES(Your_program ${VROOMGIS_LIBRARY})


# VROOMGIS LIB
SET(VROOMGIS_LIBRARY "vroomgis")

#FINDING VROOMGIS PATH
FIND_PATH(VROOMGIS_PATH "vroomgis.h" 
"/plugins/vroomgis/src/"
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

IF(NOT VROOMGIS_PATH)
	MESSAGE(FATAL_ERROR "Path not found, not able to use the vroomgis library. You may fill the VROOMGIS_PATH manually")
ENDIF(NOT VROOMGIS_PATH)
MESSAGE ("VroomGIS found here : ${VROOMGIS_PATH}")

# NEEDED LIBRARY
# WXWIDGETS
# DEFINE ALL INCLUDES AND SO ON
SET(WXWINDOWS_USE_GL 0) 
INCLUDE ("${VROOMGIS_PATH}/../build/cmake/FindwxWidgets.cmake")

#GEOS - GDAL libs
INCLUDE ("${VROOMGIS_PATH}/../build/cmake/findGISLib.cmake")


#SQLITE libs
INCLUDE ("${VROOMGIS_PATH}/../build/cmake/findsqlite.cmake")


#CREATE LIB WITH ALL MAIN PROJECT CLASS
#search all source files
FILE (GLOB VROOMGIS_LIB_SRC_FILES "${VROOMGIS_PATH}/*.cpp")
SOURCE_GROUP(source FILES ${VROMGIS_LIB_SRC_FILES})

#TEMP CODE FOR COUNTING
LIST(LENGTH VROOMGIS_LIB_SRC_FILES MY_LENGTH)
IF(NOT MY_LENGTH)
	MESSAGE(FATAL_ERROR "${MY_LENGTH} source found for vroomGIS library")
ENDIF(NOT MY_LENGTH)

# search all headers files
FILE (GLOB VROOMGIS_LIB_HEAD_FILES "${VROOMGIS_PATH}/*.h")
SOURCE_GROUP(header FILES ${VROOMGIS_LIB_HEAD_FILES})
INCLUDE_DIRECTORIES("${VROOMGIS_PATH}")

ADD_LIBRARY(${VROOMGIS_LIBRARY} STATIC ${VROOMGIS_LIB_SRC_FILES} ${VROOMGIS_LIB_HEAD_FILES})
#TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} ${VROOMGIS_LIBRARY})
