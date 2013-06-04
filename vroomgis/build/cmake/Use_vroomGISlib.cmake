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

IF(NOT VROOMGIS_PATH)
	MESSAGE(FATAL_ERROR "Path not found, not able to use the vroomgis library. You may fill the VROOMGIS_PATH manually")
ENDIF(NOT VROOMGIS_PATH)
MESSAGE ("VroomGIS found here : ${VROOMGIS_PATH}")

# NEEDED LIBRARY
# WXWIDGETS
# DEFINE ALL INCLUDES AND SO ON
SET (SEARCH_VROOMGIS_LIBS CACHE BOOL "Sould we search for libraries for vroomGIS ?" )
IF(SEARCH_VROOMGIS_LIBS)
	SET(WXWINDOWS_USE_GL 0) 
	INCLUDE ("${VROOMGIS_PATH}/../build/cmake/FindwxWidgets.cmake")

	#GEOS - GDAL libs
	INCLUDE ("${VROOMGIS_PATH}/../build/cmake/findGISLib.cmake")

	#SQLITE libs
	IF(NOT "${SQLITE_PATH}" STREQUAL "NO")
		INCLUDE ("${VROOMGIS_PATH}/../build/cmake/findsqlite.cmake")
	ELSE(NOT "${SQLITE_PATH}" STREQUAL "NO")
		MESSAGE("SQLite ignored!")
	ENDIF(NOT "${SQLITE_PATH}" STREQUAL "NO")
ENDIF(SEARCH_VROOMGIS_LIBS)

#CREATE LIB WITH ALL MAIN PROJECT CLASS
#search all source files
FILE (GLOB VROOMGIS_LIB_SRC_FILES "${VROOMGIS_PATH}/*.cpp")
FILE (GLOB VROOMGIS_LIB_ART_FILES "${VROOMGIS_PATH}/../art/*.cpp")
SOURCE_GROUP(source FILES ${VROMGIS_LIB_SRC_FILES} ${VROOMGIS_LIB_ART_FILES})


#TEMP CODE FOR COUNTING
LIST(LENGTH VROOMGIS_LIB_SRC_FILES MY_LENGTH)
IF(NOT MY_LENGTH)
	MESSAGE(FATAL_ERROR "${MY_LENGTH} source found for vroomGIS library")
ENDIF(NOT MY_LENGTH)

# search all headers files
FILE (GLOB VROOMGIS_LIB_HEAD_FILES "${VROOMGIS_PATH}/*.h")
FILE (GLOB VROOMGIS_LIB_HEAD_ART_FILES "${VROOMGIS_PATH}/../art/*.h")
SOURCE_GROUP(header FILES ${VROOMGIS_LIB_HEAD_FILES} ${VROOMGIS_LIB_HEAD_ART_FILES})
INCLUDE_DIRECTORIES("${VROOMGIS_PATH}")
INCLUDE_DIRECTORIES("${VROOMGIS_PATH}/../art")

ADD_LIBRARY(${VROOMGIS_LIBRARY} STATIC ${VROOMGIS_LIB_SRC_FILES}
	${VROOMGIS_LIB_ART_FILES}
	${VROOMGIS_LIB_HEAD_FILES}
	${VROOMGIS_LIB_HEAD_ART_FILES})

# USED FOR PERFORMANCE UNDER WINDOWS
IF(WIN32)
	TARGET_LINK_LIBRARIES (${VROOMGIS_LIBRARY} "psapi")
ENDIF(WIN32)
