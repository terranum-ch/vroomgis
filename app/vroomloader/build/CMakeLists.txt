# CMAKE BUILD FILE FOR vroomLoader
# (c) LUCIEN SCHREIBER CREALP 2012
cmake_minimum_required(VERSION 2.6)

PROJECT ("vroomLoader")

#search all source and headers files
FILE (GLOB_RECURSE SRC_FILES ../src/*.cpp)
SOURCE_GROUP (source FILES ${SRC_FILES})
FILE (GLOB_RECURSE HEAD_FILES "../src/*.h")
SOURCE_GROUP (header FILES ${HEAD_FILES})

#CREATE VROOMGIS LIBRARY AND WXWIDGETS - GDAL LIB
INCLUDE("../../../vroomgis/build/cmake/Use_vroomGISlib.cmake")	

#USE LSVERSION PLUGIN
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

# mac specific stuff
IF( APPLE )
	SET (APP_ICON "vroomgis.icns")
	SET(MACOSX_BUNDLE_INFO_STRING "VroomGIS test Program")
	SET(MACOSX_BUNDLE_ICON_FILE ${APP_ICON})
	SET(MACOSX_BUNDLE_BUNDLE_NAME "${CMAKE_PROJECT_NAME}")
	SET(MACOSX_BUNDLE_SHORT_VERSION_STRING "Alpha")
	SET(MACOSX_BUNDLE_BUNDLE_VERSION "1.0.0")
	SET(MACOSX_BUNDLE_COPYRIGHT "Lucien Schreiber (c) CREALP 2012")
	
	# copy icon
	SET(APP_ICON_LOCATION "../../../vroomgis/art/vroomgis.icns")
	SET_SOURCE_FILES_PROPERTIES(${APP_ICON_LOCATION} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
	SET(SRC_FILES ${SRC_FILES} ${APP_ICON_LOCATION})
ENDIF (APPLE)

 
# conditionnal compilation if windows needs ressource
IF (WIN32)
	SOURCE_GROUP (resource FILES ../src/resource.rc)
	ADD_EXECUTABLE (${CMAKE_PROJECT_NAME} WIN32 ${SRC_FILES} ${HEAD_FILES} "../src/resource.rc")
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

# SHOULD WE USE THE MT VERSION FOR DEFAULT 
# COMPATIBILITY WITH < WINXP
IF (WIN32)
	INCLUDE ("../../../vroomgis/build/cmake/Use_visualstudioMT.cmake")
ENDIF (WIN32)


