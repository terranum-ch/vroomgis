# SCRIPT FOR BUILDING LSVersion
# INCLUDE THIS FILE FOR INCLUDING A FILE AND OPTIONNALY A DIALOG CONTAINING
# ALL VERSION INFORMATIONS 



#OPTIONS - PARAMETERS 
SET(USE_VERSION 1 CACHE BOOL "Use component lsVersion")
IF(USE_VERSION)
	SET(lsVERSION_ENABLED 1)

	SET(wxWIDGETS_USING_SVN 0 CACHE BOOL "Using latest version of wxWidgets from SVN")
	IF(wxWIDGETS_USING_SVN)
		SET(wxWIDGETS_PATH_SVN 0 CACHE PATH "Path to the SVN version of wxWidgets from")
		SET(lsVERSION_SOFT_NAME ${CMAKE_PROJECT_NAME})		
	
		# GET WXWIDGETS VERSION
		EXECUTE_PROCESS(COMMAND "svnversion" -n
		WORKING_DIRECTORY ${wxWIDGETS_PATH_SVN}
		TIMEOUT 20
		OUTPUT_VARIABLE lsVERSION_WXWIDGETS_SVN
		ERROR_VARIABLE MY_ERROR2)
		IF(MY_ERROR2)
			MESSAGE(FATAL_ERROR "myError2 : ${MY_ERROR2}")
		ENDIF(MY_ERROR2)
		MESSAGE("my soft version ${lsVERSION_WXWIDGETS_SVN}")

	ENDIF(wxWIDGETS_USING_SVN)
	
	# IF GEOS WAS SEARCHED
	IF(GEOS_INCLUDE_DIR)
		SET(lsVERSION_HAS_GEOS 1)
	ENDIF(GEOS_INCLUDE_DIR)	

	# IF GDAL WAS SEARCHED
	IF(GDAL_INCLUDE_DIR)
		SET(lsVERSION_HAS_GDAL 1)
	ENDIF(GDAL_INCLUDE_DIR)	

	#FINDING OUR PATH
	FIND_PATH(PLUGIN_PATH "lsversion_param.h.in" 
	"../../../plugins/lsversion/src/"
	"../../plugins/lsversion/src/" 
	"../../../../../src/"
	 "../../../../src/"
	 "../../../src/"
	 "../../src/"
	 "../src/"
	 "../"
	 NO_DEFAULT_PATH)
	
	# GET PROGRAM SVN VERSION
	EXECUTE_PROCESS(COMMAND "svnversion" -n
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	TIMEOUT 20
	OUTPUT_VARIABLE lsVERSION_SOFT_VERSION
	ERROR_VARIABLE MY_ERROR1)
	IF(MY_ERROR1)
		MESSAGE(FATAL_ERROR "myError1 : ${MY_ERROR1}")
	ENDIF(MY_ERROR1)
	MESSAGE("my soft version ${lsVERSION_SOFT_VERSION}")



	#
	# CREATE LIBRARY 
	#
	#search all source files
	FILE (GLOB VERSION_SRC_FILES "${PLUGIN_PATH}/*.cpp")
	SOURCE_GROUP(source FILES ${VERSION_SRC_FILES})

	#COUNTING NUMBER OF SOURCES FOUND
	LIST(LENGTH VERSION_SRC_FILES MY_VERSION_LENGTH)
	IF(NOT MY_VERSION_LENGTH)
		MESSAGE(FATAL_ERROR "${MY_VERSION_LENGTH} source found for lsVersion Plugin")
	ENDIF(NOT MY_VERSION_LENGTH)

	# search all headers files
	FILE (GLOB VERSION_HEAD_FILES "${PLUGIN_PATH}/*.h")
	SOURCE_GROUP(header FILES ${VERSION_HEAD_FILES})
	INCLUDE_DIRECTORIES("${PLUGIN_PATH}")
	ADD_LIBRARY("lsVersion" STATIC ${VERSION_SRC_FILES} ${VERSION_HEAD_FILES})
	

ELSE(USE_VERSION)
	MESSAGE( "Not using vrVersion Component")
ENDIF(USE_VERSION)


CONFIGURE_FILE("${PLUGIN_PATH}/lsversion_param.h.in" "${PROJECT_BINARY_DIR}/lsversion_param.h")
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR})




