# CMAKE BUILD FILE FOR vroomLoader
# (c) LUCIEN SCHREIBER CREALP 2009
cmake_minimum_required(VERSION 2.6)


## POLICY FOR WINDOWS ONLY ???
## ENCORE NECESSAIRE ????????????????? A TESTER SOUS WIN
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)


PROJECT ("vroomTwin")

#search all source files
FILE (GLOB_RECURSE SRC_FILES ../src/*.cpp)
SOURCE_GROUP (source FILES ${SRC_FILES})

# search all headers files
FILE (GLOB_RECURSE HEAD_FILES "../src/*.h")
SOURCE_GROUP (header FILES ${HEAD_FILES})

MESSAGE ("Build type is : " ${CMAKE_BUILD_TYPE})

#CREATE VROOMGIS LIBRARY AND WXWIDGETS - GDAL LIB
INCLUDE("../../../vroomgis/build/cmake/Use_vroomGISlib.cmake")	

#USE LSVERSION PLUGIN
INCLUDE("../../../plugins/lsversion/build/use_lsversion.cmake")

IF( APPLE )
	SET (APP_ICON "vroomgis.icns")
	#bundle variables for compilation under Mac OS
	SET(MACOSX_BUNDLE_INFO_STRING "VroomGIS test Program")
	SET(MACOSX_BUNDLE_ICON_FILE ${APP_ICON})
	#MACOSX_BUNDLE_GUI_IDENTIFIER
	#MACOSX_BUNDLE_LONG_VERSION_STRING
	SET(MACOSX_BUNDLE_BUNDLE_NAME "${CMAKE_PROJECT_NAME}")
	SET(MACOSX_BUNDLE_SHORT_VERSION_STRING "Alpha")
	SET(MACOSX_BUNDLE_BUNDLE_VERSION "1.0.0")
	SET(MACOSX_BUNDLE_COPYRIGHT "Lucien Schreiber (c) CREALP 2010")
	
	# Need to copy the icon file
	EXEC_PROGRAM( "mkdir -p       ${PROJECT_BINARY_DIR}/Debug/${CMAKE_PROJECT_NAME}.app/Contents/Resources")
	EXEC_PROGRAM( "cp ${PROJECT_SOURCE_DIR}/../../../vroomgis/art/${APP_ICON} ${PROJECT_BINARY_DIR}/Debug/${CMAKE_PROJECT_NAME}.app/Contents/Resources")
	EXEC_PROGRAM( "mkdir -p       ${PROJECT_BINARY_DIR}/Release/${CMAKE_PROJECT_NAME}.app/Contents/Resources")
	EXEC_PROGRAM( "cp ${PROJECT_SOURCE_DIR}/../../../vroomgis/art/${APP_ICON} ${PROJECT_BINARY_DIR}/Release/${CMAKE_PROJECT_NAME}.app/Contents/Resources")
ENDIF (APPLE)


 
# conditionnal compilation if windows needs ressource
IF (WIN32)
	SOURCE_GROUP (resource FILES ../src/resource.rc)
 	ADD_EXECUTABLE (${CMAKE_PROJECT_NAME} WIN32 ${SRC_FILES} ${HEAD_FILES} ../src/resource.rc)
 	TARGET_LINK_LIBRARIES (${CMAKE_PROJECT_NAME} ${WXWINDOWS_LIBS})

ELSE (WIN32)
 	ADD_EXECUTABLE (${CMAKE_PROJECT_NAME} MACOSX_BUNDLE ${SRC_FILES} ${HEAD_FILES})
ENDIF (WIN32)


# LINK VROOMGIS LIBRARY
TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} ${VROOMGIS_LIBRARY})

#LINK LS VERSION LIBRARY
IF(USE_VERSION)
  TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} ${LSVERSION_NAME})
ENDIF(USE_VERSION)

# copy libs to bundle if we are under mac...
# check is done internally.
INCLUDE ("../../../vroomgis/build/cmake/installMacGISLIB.cmake")


# SHOULD WE USE THE MT VERSION FOR DEFAULT 
# COMPATIBILITY WITH < WINXP
IF (WIN32)
	INCLUDE ("../../../vroomgis/build/cmake/Use_visualstudioMT.cmake")
ENDIF (WIN32)


