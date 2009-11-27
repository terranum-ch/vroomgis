# CMAKE BUILD FILE FOR vroomLoader
# (c) LUCIEN SCHREIBER CREALP 2009
cmake_minimum_required(VERSION 2.6)


## POLICY FOR WINDOWS ONLY ???
## ENCORE NECESSAIRE ????????????????? A TESTER SOUS WIN
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)


SET (PROGNAME vroomLoader)
PROJECT (${PROGNAME})

#search all source files
FILE (GLOB_RECURSE SRC_FILES ../src/*.cpp)
SOURCE_GROUP (source FILES ${SRC_FILES})

# search all headers files
FILE (GLOB_RECURSE HEAD_FILES "../src/*.h")
SOURCE_GROUP (header FILES ${HEAD_FILES})

## search files for vroomgis engine
FILE (GLOB_RECURSE VRSRC_FILES ../../../src/*.cpp)
SOURCE_GROUP (vroomgis_source FILES ${VRSRC_FILES})
FILE (GLOB_RECURSE VRHEAD_FILES "../../../src/*.h")
SOURCE_GROUP (vroomgis_header FILES ${VRHEAD_FILES})


MESSAGE ("Build type is : " ${CMAKE_BUILD_TYPE})

# WXWIDGETS
# DEFINE ALL INCLUDES AND SO ON
 SET(WXWINDOWS_USE_GL 0)
 
 
 INCLUDE ("cmake/Use_wxWindows.cmake")


#GEOS - GDAL libs
# INCLUDE ("cmake/findGISLib.cmake")
 
 
#wxTreeMultiCtrl
# INCLUDE ("${PROJECT_SOURCE_DIR}/../src/components/wxtreemultictrl/wxtreectrloptions.cmake")
 

#CURL
#INCLUDE ("cmake/FindCURL.cmake")
	


IF( APPLE )
	SET (APP_ICON "vroomgis.icns")
	#bundle variables for compilation under Mac OS
	SET(MACOSX_BUNDLE_INFO_STRING "VroomGIS test Program")
	SET(MACOSX_BUNDLE_ICON_FILE ${APP_ICON})
	#MACOSX_BUNDLE_GUI_IDENTIFIER
	#MACOSX_BUNDLE_LONG_VERSION_STRING
	SET(MACOSX_BUNDLE_BUNDLE_NAME "${PROGNAME}")
	SET(MACOSX_BUNDLE_SHORT_VERSION_STRING "Alpha")
	SET(MACOSX_BUNDLE_BUNDLE_VERSION "1.0.0")
	SET(MACOSX_BUNDLE_COPYRIGHT "Lucien Schreiber (c) CREALP 2009")
	
	# Need to copy the icon file
	EXEC_PROGRAM( "mkdir -p       ${PROJECT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${PROGNAME}.app/Contents/Resources")
	EXEC_PROGRAM( "cp ${PROJECT_SOURCE_DIR}/../../../art/${APP_ICON} ${PROJECT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${PROGNAME}.app/Contents/Resources")
ENDIF (APPLE)


 
# conditionnal compilation if windows needs ressource
IF (WIN32)

	SOURCE_GROUP (resource FILES ../src/resource.rc)
 	ADD_EXECUTABLE (${PROGNAME} WIN32 ${SRC_FILES} ${HEAD_FILES} ${VRSRC_FILES} ${VRHEAD_FILES} ../src/gui/resource.rc)
 	TARGET_LINK_LIBRARIES (${PROGNAME} ${WXWINDOWS_LIBS})

ELSE (WIN32)
 	ADD_EXECUTABLE (${PROGNAME} MACOSX_BUNDLE ${SRC_FILES} ${HEAD_FILES} ${VRSRC_FILES} ${VRHEAD_FILES} )
ENDIF (WIN32)

# copy libs to bundle if we are under mac...
# check is done internally.
#INCLUDE ("cmake/installMacGISLIB.cmake")


#SVN NUMBERING
#INCLUDE ("cmake/use_svnnumber.cmake")



#SOULD WE ADD THE LIBTIFF INTO BUNDLE
# ONLY FOR CERTAIN MAC WITH PPC 
# CHECK ARE DONE INTERNALLY YOU 
# CAN CALL THIS WITHOUT FEAR...
#INCLUDE ("cmake/installMacLibTiff.cmake")


#SOULD WE ADD THE GDIPLUS.dll INTO project
# ONLY FOR Windows, do nothing under other 
# OS. 
# CHECK ARE DONE INTERNALLY YOU 
# CAN CALL THIS WITHOUT FEAR...
INCLUDE ("cmake/Use_wingdiplus.cmake")

# SHOULD WE USE THE MT VERSION FOR DEFAULT 
# COMPATIBILITY WITH < WINXP
IF (WIN32)
	INCLUDE ("cmake/Use_visualstudioMT.cmake")
ENDIF (WIN32)


