# CMAKE BUILD FILE FOR ADDING C2D FORMAT TO GDAL
# (c) LUCIEN SCHREIBER CREALP 2010
cmake_minimum_required(VERSION 3.11)

#
#
# COPY FILES TO THE GDAL TREE STRUCTURE
#
#

# BE SURE THAT GDAL PATH IS DEFINED 
set(GDAL_SOURCE_PATH CACHE PATH "Path to the GDAL source directory")
IF (NOT GDAL_SOURCE_PATH)
    message(FATAL_ERROR "GDAL_SOURCE_PATH isn't defined")
ENDIF (NOT GDAL_SOURCE_PATH)
message("GDAL Source path is : ${GDAL_SOURCE_PATH}")

# FULL SOURCE PATH IS NEEDED FOR COPYING
message("Actual path is : ${CMAKE_CURRENT_SOURCE_DIR}")

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
    set(GDAL_CONFIGURED CACHE BOOL "Is GDAL correctly configured (nmake.opt) ?")
    IF (NOT GDAL_CONFIGURED)
        message(FATAL_ERROR "Configure GDAL first using nmake.opt")
    ENDIF (NOT GDAL_CONFIGURED)

    set(VCVARS_PATH CACHE PATH "Path to the vcvars.bat file for setting up VisualStudio")
    IF (NOT VCVARS_PATH)
        message(FATAL_ERROR "Visual Studio Vars not found")
    ENDIF (NOT VCVARS_PATH)

    set(GDAL_SOURCE_PATH_WINDOWS CACHE PATH "Path to the GDAL source with backslash")
    IF (NOT GDAL_SOURCE_PATH_WINDOWS)
        message(FATAL_ERROR "Path to GDAL not defined")
    ENDIF (NOT GDAL_SOURCE_PATH_WINDOWS)

    #message (${PROJECT_SOURCE_DIR})
    EXECUTE_PROCESS(COMMAND build-gdal-win.bat ${VCVARS_PATH} ${GDAL_SOURCE_PATH_WINDOWS}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            OUTPUT_VARIABLE MYCONFIGOUTPUT
            ERROR_VARIABLE MYCONFIGERROR)
    message("Configure : ${MYCONFIGOUTPUT} ${MYCONFIGERROR}")

    #message(FATAL_ERROR "Code for windows support didn't exist. Please add code to this file")


ELSE (WIN32)
    #CONFIGURE GDAL ? ONLY NEEDED IF GDALMake.opt.in WAS CHANGED
    set(GDAL_CONFIGURE CACHE BOOL "Configure GDAL ? only needed if GDALMake.opt.in was changed")
    IF (GDAL_CONFIGURE)
        set(USE_SYSTEM_LIBS CACHE BOOL "Use GEOS and PROJ library from the system. If not checked, will take them from : ${GDAL_PREFIX}")
        #SET (GEOS_PATH "")
        IF (USE_SYSTEM_LIBS)
            message(DEBUG "Using GEOS and PROJ from the system...")
            set(GEOS_PATH "/usr/bin/")
            set(PROJ_PATH "/usr")

        ELSE (USE_SYSTEM_LIBS)
            message(DEBUG "Using GEOS and PROJ from ${GDAL_PREFIX}")
            set(GEOS_PATH "${GDAL_PREFIX}/bin/")
            set(PROJ_PATH "${GDAL_PREFIX}")

        ENDIF (USE_SYSTEM_LIBS)
        message("Geos PATH = ${GEOS_PATH}")

        set(GDAL_PREFIX CACHE PATH "Path the the LIBGIS directory")
        IF (NOT GDAL_PREFIX)
            message(FATAL_ERROR "No LIBGIS directory defined")
        ENDIF (NOT GDAL_PREFIX)

        message(DEBUG "./configure --prefix=${GDAL_PREFIX} --with-geos=${GEOS_PATH}geos-config --with-python=no --with-sqlite3=yes --with-static-proj4=${PROJ_PATH}")
        EXECUTE_PROCESS(COMMAND ./configure --prefix=${GDAL_PREFIX} --with-geos=${GEOS_PATH}geos-config --with-python=no --with-sqlite3=yes --with-static-proj4=${PROJ_PATH}
                WORKING_DIRECTORY ${GDAL_SOURCE_PATH}
                RESULT_VARIABLE MYCONFIGRESULT
                OUTPUT_VARIABLE MYCONFIGOUTPUT
                ERROR_VARIABLE MYCONFIGERROR)

        set(COMPILER_JOBS CACHE STRING "Specifiy the number of parallel threads to start ?")
        IF (NOT COMPILER_JOBS)
            message(WARNING "No processor specified, will use 1 (slow)")
            set(COMPILER_JOBS 1)
        ENDIF (NOT COMPILER_JOBS)

        message("Configure : ${MYCONFIGOUTPUT} | ${MYCONFIGERROR} | ${MYCONFIGRESULT}")
    ENDIF (GDAL_CONFIGURE)

    #MAKE AND INSTALL GDAL
    set(GDAL_MAKE_AND_INSTALL CACHE BOOL "make and install GDAL ? if empty, only configure will be run")
    IF (GDAL_MAKE_AND_INSTALL)
        EXECUTE_PROCESS(COMMAND "make --jobs=${COMPILER_JOBS}" WORKING_DIRECTORY ${GDAL_SOURCE_PATH} OUTPUT_VARIABLE MYOUTPUT ERROR_VARIABLE MYERROR)
        message("make message are : ${MYOUTPUT}")
        IF (MYERROR)
            message(FATAL_ERROR "Error during GDAL build : ${MYERROR}")
        ENDIF (MYERROR)
        EXECUTE_PROCESS(COMMAND make install WORKING_DIRECTORY ${GDAL_SOURCE_PATH} OUTPUT_VARIABLE MYOUTPUTINSTALL)
        message("make install message are : ${MYOUTPUTINSTALL}")
    ENDIF (GDAL_MAKE_AND_INSTALL)
ENDIF (WIN32)

#
#
# CREATE PROJECT FOR GATHERING ALL FILES TOGETHER
#
#

set(PROJECTNAME gdal_c2d_support)
project(${PROJECTNAME})

set(SOURCE_C2D_VAR ${CMAKE_CURRENT_SOURCE_DIR}/../frmts/c2d/c2ddataset.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../gcore/gdal_frmts.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../frmts/gdalallregister.cpp)
source_group(SOURCE_C2D FILES ${SOURCE_C2D_VAR})

file(GLOB_RECURSE SOURCE_PORT_C ${GDAL_SOURCE_PATH}/port/*.cpp)
file(GLOB_RECURSE SOURCE_PORT_H ${GDAL_SOURCE_PATH}/port/*.h)
source_group(SOURCE_PORT FILES ${SOURCE_PORT_C} ${SOURCE_PORT_H})


set(SOURCE_SAMPLE_VAR ${GDAL_SOURCE_PATH}/frmts/jdem/jdemdataset.cpp
        ${GDAL_SOURCE_PATH}/frmts/saga/sagadataset.cpp
        ${GDAL_SOURCE_PATH}/frmts/jpeg/jpgdataset.cpp
        ${GDAL_SOURCE_PATH}/frmts/raw/pnmdataset.cpp)
source_group(SOURCE_SAMPLE FILES ${SOURCE_SAMPLE_VAR})


add_executable("c2d-wontbuild" ${SOURCE_C2D_VAR})
add_executable("sample-wontbuild" ${SOURCE_SAMPLE_VAR})
add_executable("port-wontbuild" ${SOURCE_PORT_C} ${SOURCE_PORT_H})






 




