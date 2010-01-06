# CMAKE BUILD FILE FOR CREATING A STATIC
# VROOMGIS Library
# (c) LUCIEN SCHREIBER CREALP 2009
# VROOMGIS NEEDS wxWidgets and GEOS / GDAL

# PARAMETERS : 
# VR_LIBRARY_NAME - Name of the vroomgislibrary 

# USAGE :
# Call INCLUDE(Use_vroomGISlib.cmake)
# TARGET_LINK_LIBRARIES(Your_program ${VR_LIBRARY_NAME})


#FINDING OUR PATH
FIND_PATH(THIS_DIR_PATH "Use_vroomGISlib.cmake" 
 "../../../../../build/cmake/"
 "../../../../build/cmake/"
 "../../../build/cmake/"
 "../../build/cmake/"
 "../build/cmake/"
 NO_DEFAULT_PATH)

IF(NOT THIS_DIR_PATH)
	MESSAGE(FATAL_ERROR "Path not found, not able to use the vroomgis library")
ENDIF(NOT THIS_DIR_PATH)



# NEEDED LIBRARY

# WXWIDGETS
# DEFINE ALL INCLUDES AND SO ON
SET(WXWINDOWS_USE_GL 0) 
INCLUDE ("${THIS_DIR_PATH}/FindwxWidgets.cmake")

#GEOS - GDAL libs
INCLUDE ("${THIS_DIR_PATH}/findGISLib.cmake")



# VROOMGIS LIB

SET(VR_LIBRARY_NAME "vroomgis")

#CREATE LIB WITH ALL MAIN PROJECT CLASS
#search all source files
FILE (GLOB LIB_SRC_FILES "${THIS_DIR_PATH}/../../src/*.cpp")
SOURCE_GROUP(source FILES ${LIB_SRC_FILES})

#TEMP CODE FOR COUNTING
LIST(LENGTH LIB_SRC_FILES MY_LENGTH)
IF(NOT MY_LENGTH)
	MESSAGE(FATAL_ERROR "${MY_LENGTH} source found for vroomGIS library")
ENDIF(NOT MY_LENGTH)

# search all headers files
FILE (GLOB LIB_HEAD_FILES "${THIS_DIR_PATH}/../../src/*.h")
SOURCE_GROUP(header FILES ${LIB_HEAD_FILES})
INCLUDE_DIRECTORIES("${THIS_DIR_PATH}/../../src")

ADD_LIBRARY(${VR_LIBRARY_NAME} STATIC ${LIB_SRC_FILES} ${LIB_HEAD_FILES})

