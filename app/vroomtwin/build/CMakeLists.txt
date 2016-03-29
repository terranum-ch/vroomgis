# CMAKE BUILD FILE FOR vroomLoader
# (c) LUCIEN SCHREIBER 2013
cmake_minimum_required(VERSION 2.6)

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

#USE WXHGVERSION PLUGIN
INCLUDE("../../../plugins/wxhgversion/build/use_wxhgversion.cmake")

# Visual Leak Detector
set(CMAKE_MODULE_PATH ${VROOMGIS_PATH}/../build/cmake)
if (WIN32)
    set(USE_VLD 0 CACHE BOOL "Sould we use Visual Leak Detector (https://vld.codeplex.com) ?" )
    if (USE_VLD)
        find_package(VLD)
        include_directories(${VLD_INCLUDE_DIRS})
        add_definitions(-DUSE_VLD)
    endif (USE_VLD)
endif (WIN32)

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
	SET(MACOSX_BUNDLE_COPYRIGHT "Lucien Schreiber (c) 2013")
	
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
 	TARGET_LINK_LIBRARIES (${CMAKE_PROJECT_NAME} ${wxWidgets_LIBRARIES})

ELSE (WIN32)
 	ADD_EXECUTABLE (${CMAKE_PROJECT_NAME} MACOSX_BUNDLE ${SRC_FILES} ${HEAD_FILES})
ENDIF (WIN32)


# LINK VROOMGIS LIBRARY
TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} ${VROOMGIS_LIBRARY})

# LINK WXHGVERSION PLUGIN
IF(USE_WXHGVERSION)
  TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} ${WXHGVERSION_NAME})
ENDIF(USE_WXHGVERSION)

# Visual Leak Detector
if (WIN32)
	if (USE_VLD)
		target_link_libraries(${CMAKE_PROJECT_NAME} ${VLD_LIBRARIES})
	endif (USE_VLD)
endif (WIN32)

# copy libs to bundle if we are under mac...
# check is done internally.
INCLUDE ("../../../vroomgis/build/cmake/installMacGISLIB.cmake")


