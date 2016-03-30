# CMAKE BUILD FILE FOR INSTALLING GIS LIB (GDAL / GEOS)
# UNDER MAC : AKA COPYING LIBS INTO BUNDLE AND CHANGING
# PATH TO TAKE LOCAL LIBS
# (c) LUCIEN SCHREIBER CREALP 2008
###################################################
# DON'T FORGET TO 
# CHANGE GEOS_VERSION AND GEOS_C_VERSION
# FOR NEW LIB VERSION...
###################################################

#definitions
SET(LIBNAME "Library")
SET(BUNDLEPATH "${PROJECT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_PROJECT_NAME}.app")
SET(BUNDLELIBPATH "${BUNDLEPATH}/Contents/${LIBNAME}")
SET(GEOS_VERSION "3.4.2")
SET(GEOS_C_VERSION "1")
SET(CURL_VERSION "4")
#SET (PROJ_VERSION "0.6.6")

# this code will only be called if apple
IF (NOT APPLE)
    return()
ENDIF (NOT APPLE)


### COPY GEOS ##
IF (GEOS_LIBRARIES AND SEARCH_GIS_LIB_PATH)
    ADD_CUSTOM_COMMAND(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
            COMMAND mkdir -p ${BUNDLELIBPATH}
            COMMAND cp ${SEARCH_GIS_LIB_PATH}/lib/libgeos.dylib ${BUNDLELIBPATH}/libgeos.dylib
            COMMAND cp ${SEARCH_GIS_LIB_PATH}/lib/libgeos_c.dylib ${BUNDLELIBPATH}/libgeos_c.dylib
            COMMAND install_name_tool -id @executable_path/../${LIBNAME}/libgeos.dylib ${BUNDLELIBPATH}/libgeos.dylib
            COMMAND install_name_tool -id @executable_path/../${LIBNAME}/libgeos_c.dylib ${BUNDLELIBPATH}/libgeos_c.dylib
            COMMAND install_name_tool -change ${SEARCH_GIS_LIB_PATH}/lib/libgeos-${GEOS_VERSION}.dylib @executable_path/../${LIBNAME}/libgeos.dylib ${BUNDLELIBPATH}/libgeos_c.dylib

            COMMAND install_name_tool -change ${SEARCH_GIS_LIB_PATH}/lib/libgeos_c.1.dylib @executable_path/../${LIBNAME}/libgeos_c.dylib ${BUNDLEPATH}/Contents/MacOS/${CMAKE_PROJECT_NAME}
            COMMENT "Copying and updating program GEOS libs"
            )
ENDIF ()

### COPY GDAL & PROJ ###
IF (GDAL_LIBRARIES AND SEARCH_GIS_LIB_PATH)
    ADD_CUSTOM_COMMAND(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
            COMMAND mkdir -p ${BUNDLELIBPATH}
            COMMAND cp ${SEARCH_GIS_LIB_PATH}/lib/libgdal.dylib ${BUNDLELIBPATH}
            COMMAND cp ${SEARCH_GIS_LIB_PATH}/lib/libproj.dylib ${BUNDLELIBPATH}
            COMMAND install_name_tool -id @executable_path/../${LIBNAME}/libgdal.dylib ${BUNDLELIBPATH}/libgdal.dylib
            COMMAND install_name_tool -change ${SEARCH_GIS_LIB_PATH}/lib/libgeos-${GEOS_VERSION}.dylib @executable_path/../${LIBNAME}/libgeos.dylib ${BUNDLELIBPATH}/libgdal.dylib
            COMMAND install_name_tool -change ${SEARCH_GIS_LIB_PATH}/lib/libgeos_c.1.dylib @executable_path/../${LIBNAME}/libgeos_c.dylib ${BUNDLELIBPATH}/libgdal.dylib
            COMMAND install_name_tool -id @executable_path/../${LIBNAME}/libproj.dylib ${BUNDLELIBPATH}/libproj.dylib
            COMMAND install_name_tool -change ${SEARCH_GIS_LIB_PATH}/lib/libproj.0.dylib @executable_path/../${LIBNAME}/libproj.dylib ${BUNDLELIBPATH}/libgdal.dylib

            COMMAND install_name_tool -change ${SEARCH_GIS_LIB_PATH}/lib/libgdal.1.dylib @executable_path/../${LIBNAME}/libgdal.dylib ${BUNDLEPATH}/Contents/MacOS/${CMAKE_PROJECT_NAME}

            COMMENT "Copying and updating GDAL libs"
            )
ENDIF ()


# CURL
IF (CURL_LIBRARIES)
    get_filename_component(CURL_LIBRARIES_PATH ${CURL_LIBRARIES} PATH)
    IF ("/usr/lib" STREQUAL CURL_LIBRARIES_PATH)
        return()
    ENDIF ()

    ADD_CUSTOM_COMMAND(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
            COMMAND mkdir -p ${BUNDLELIBPATH}
            COMMAND cp ${CURL_LIBRARIES_PATH}/libcurl.dylib ${BUNDLELIBPATH}
            COMMAND install_name_tool -id @executable_path/../${LIBNAME}/libcurl.dylib ${BUNDLELIBPATH}/libcurl.dylib
            COMMAND install_name_tool -change ${CURL_LIBRARIES_PATH}/libcurl.${CURL_VERSION}.dylib @executable_path/../${LIBNAME}/libcurl.dylib ${BUNDLELIBPATH}/libgdal.dylib
            COMMAND install_name_tool -change ${CURL_LIBRARIES_PATH}/libcurl.${CURL_VERSION}.dylib @executable_path/../${LIBNAME}/libcurl.dylib ${BUNDLEPATH}/Contents/MacOS/${CMAKE_PROJECT_NAME}
            COMMENT "Copying and updating CURL libs"
            )
ENDIF (CURL_LIBRARIES)
