# CMAKE BUILD FILE FOR ADDING C2D FORMAT TO GDAL
# (c) LUCIEN SCHREIBER CREALP 2010
cmake_minimum_required(VERSION 2.6)

#
#
# COPY FILES TO THE GDAL TREE STRUCTURE
#
#

# BE SURE THAT GDAL PATH IS DEFINED 
SET(GDAL_SOURCE_PATH CACHE PATH "Path to the GDAL source directory")
IF (NOT GDAL_SOURCE_PATH)
    MESSAGE(FATAL_ERROR "GDAL_SOURCE_PATH isn't defined")
ENDIF (NOT GDAL_SOURCE_PATH)
MESSAGE("GDAL Source path is : ${GDAL_SOURCE_PATH}")

# FULL SOURCE PATH IS NEEDED FOR COPYING
MESSAGE("Actual path is : ${CMAKE_CURRENT_SOURCE_DIR}")

#MAKING C2D DIRECTORY INTO GDAL/FRMTS
file(MAKE_DIRECTORY "${GDAL_SOURCE_PATH}/frmts/c2d")

#COPYING FILES INTO FRMTS
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../frmts/gdalallregister.cpp ${GDAL_SOURCE_PATH}/frmts/gdalallregister.cpp COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../frmts/makefile.vc ${GDAL_SOURCE_PATH}/frmts/makefile.vc COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../frmts/c2d/c2ddataset.cpp ${GDAL_SOURCE_PATH}/frmts/c2d/c2ddataset.cpp COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../frmts/c2d/GNUmakefile ${GDAL_SOURCE_PATH}/frmts/c2d/GNUmakefile COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../frmts/c2d/makefile.vc ${GDAL_SOURCE_PATH}/frmts/c2d/makefile.vc COPYONLY)

#COPYING FILES INTO GCORE
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../gcore/gdal_frmts.h ${GDAL_SOURCE_PATH}/gcore/gdal_frmts.h COPYONLY)

#COPYING FILES TO GDAL ROOT
#configure_file (${CMAKE_CURRENT_SOURCE_DIR}/../GDALmake.opt   ${GDAL_SOURCE_PATH}/GDALmake.opt COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../GDALmake.opt.in ${GDAL_SOURCE_PATH}/GDALmake.opt.in COPYONLY)


#
#
# BUILD THE GDAL LIBRARY WITH C2D SUPPORT
#
#

IF (WIN32)
    SET(GDAL_CONFIGURED CACHE BOOL "Is GDAL correctly configured (nmake.opt) ?")
    IF (NOT GDAL_CONFIGURED)
        MESSAGE(FATAL_ERROR "Configure GDAL first using nmake.opt")
    ENDIF (NOT GDAL_CONFIGURED)

    SET(VCVARS_PATH CACHE PATH "Path to the vcvars.bat file for setting up VisualStudio")
    IF (NOT VCVARS_PATH)
        MESSAGE(FATAL_ERROR "Visual Studio Vars not found")
    ENDIF (NOT VCVARS_PATH)

    SET(GDAL_SOURCE_PATH_WINDOWS CACHE PATH "Path to the GDAL source with backslash")
    IF (NOT GDAL_SOURCE_PATH_WINDOWS)
        MESSAGE(FATAL_ERROR "Path to GDAL not defined")
    ENDIF (NOT GDAL_SOURCE_PATH_WINDOWS)

    #MESSAGE (${PROJECT_SOURCE_DIR})
    EXECUTE_PROCESS(COMMAND build-gdal-win.bat ${VCVARS_PATH} ${GDAL_SOURCE_PATH_WINDOWS}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            OUTPUT_VARIABLE MYCONFIGOUTPUT
            ERROR_VARIABLE MYCONFIGERROR)
    MESSAGE("Configure : ${MYCONFIGOUTPUT} ${MYCONFIGERROR}")

    #MESSAGE(FATAL_ERROR "Code for windows support didn't exist. Please add code to this file")


ELSE (WIN32)
    #CONFIGURE GDAL ? ONLY NEEDED IF GDALMake.opt.in WAS CHANGED
    SET(GDAL_CONFIGURE CACHE BOOL "Configure GDAL ? only needed if GDALMake.opt.in was changed")
    IF (GDAL_CONFIGURE)
        SET(USE_SYSTEM_LIBS CACHE BOOL "Use GEOS and PROJ library from the system. If not checked, will take them from : ${GDAL_PREFIX}")
        #SET (GEOS_PATH "")
        IF (USE_SYSTEM_LIBS)
            MESSAGE(DEBUG "Using GEOS and PROJ from the system...")
            SET(GEOS_PATH "/usr/bin/")
            SET(PROJ_PATH "/usr")

        ELSE (USE_SYSTEM_LIBS)
            MESSAGE(DEBUG "Using GEOS and PROJ from ${GDAL_PREFIX}")
            SET(GEOS_PATH "${GDAL_PREFIX}/bin/")
            SET(PROJ_PATH "${GDAL_PREFIX}")

        ENDIF (USE_SYSTEM_LIBS)
        MESSAGE("Geos PATH = ${GEOS_PATH}")

        SET(GDAL_PREFIX CACHE PATH "Path the the LIBGIS directory")
        IF (NOT GDAL_PREFIX)
            MESSAGE(FATAL_ERROR "No LIBGIS directory defined")
        ENDIF (NOT GDAL_PREFIX)

        MESSAGE(DEBUG "./configure --prefix=${GDAL_PREFIX} --with-geos=${GEOS_PATH}geos-config --with-python=no --with-sqlite3=yes --with-static-proj4=${PROJ_PATH}")
        EXECUTE_PROCESS(COMMAND ./configure --prefix=${GDAL_PREFIX} --with-geos=${GEOS_PATH}geos-config --with-python=no --with-sqlite3=yes --with-static-proj4=${PROJ_PATH}
                WORKING_DIRECTORY ${GDAL_SOURCE_PATH}
                RESULT_VARIABLE MYCONFIGRESULT
                OUTPUT_VARIABLE MYCONFIGOUTPUT
                ERROR_VARIABLE MYCONFIGERROR)

        SET(COMPILER_JOBS CACHE STRING "Specifiy the number of parallel threads to start ?")
        IF (NOT COMPILER_JOBS)
            MESSAGE(WARNING "No processor specified, will use 1 (slow)")
            SET(COMPILER_JOBS 1)
        ENDIF (NOT COMPILER_JOBS)

        MESSAGE("Configure : ${MYCONFIGOUTPUT} | ${MYCONFIGERROR} | ${MYCONFIGRESULT}")
    ENDIF (GDAL_CONFIGURE)

    #MAKE AND INSTALL GDAL
    SET(GDAL_MAKE_AND_INSTALL CACHE BOOL "make and install GDAL ? if empty, only configure will be run")
    IF (GDAL_MAKE_AND_INSTALL)
        EXECUTE_PROCESS(COMMAND "make --jobs=${COMPILER_JOBS}" WORKING_DIRECTORY ${GDAL_SOURCE_PATH} OUTPUT_VARIABLE MYOUTPUT ERROR_VARIABLE MYERROR)
        MESSAGE("make message are : ${MYOUTPUT}")
        IF (MYERROR)
            MESSAGE(FATAL_ERROR "Error during GDAL build : ${MYERROR}")
        ENDIF (MYERROR)
        EXECUTE_PROCESS(COMMAND make install WORKING_DIRECTORY ${GDAL_SOURCE_PATH} OUTPUT_VARIABLE MYOUTPUTINSTALL)
        MESSAGE("make install message are : ${MYOUTPUTINSTALL}")
    ENDIF (GDAL_MAKE_AND_INSTALL)
ENDIF (WIN32)

#
#
# CREATE PROJECT FOR GATHERING ALL FILES TOGETHER
#
#

SET(PROJECTNAME gdal_c2d_support)
PROJECT(${PROJECTNAME})

SET(SOURCE_C2D_VAR ${CMAKE_CURRENT_SOURCE_DIR}/../frmts/c2d/c2ddataset.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../gcore/gdal_frmts.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../frmts/gdalallregister.cpp)
SOURCE_GROUP(SOURCE_C2D FILES ${SOURCE_C2D_VAR})

FILE(GLOB_RECURSE SOURCE_PORT_C ${GDAL_SOURCE_PATH}/port/*.cpp)
FILE(GLOB_RECURSE SOURCE_PORT_H ${GDAL_SOURCE_PATH}/port/*.h)
SOURCE_GROUP(SOURCE_PORT FILES ${SOURCE_PORT_C} ${SOURCE_PORT_H})


SET(SOURCE_SAMPLE_VAR ${GDAL_SOURCE_PATH}/frmts/jdem/jdemdataset.cpp
        ${GDAL_SOURCE_PATH}/frmts/saga/sagadataset.cpp
        ${GDAL_SOURCE_PATH}/frmts/jpeg/jpgdataset.cpp
        ${GDAL_SOURCE_PATH}/frmts/raw/pnmdataset.cpp)
SOURCE_GROUP(SOURCE_SAMPLE FILES ${SOURCE_SAMPLE_VAR})


ADD_EXECUTABLE("c2d-wontbuild" ${SOURCE_C2D_VAR})
ADD_EXECUTABLE("sample-wontbuild" ${SOURCE_SAMPLE_VAR})
ADD_EXECUTABLE("port-wontbuild" ${SOURCE_PORT_C} ${SOURCE_PORT_H})






 




