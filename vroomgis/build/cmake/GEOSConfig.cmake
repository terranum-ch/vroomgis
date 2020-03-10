# if windows and search geos
# using C API of GEOS:
IF (WIN32)
    SET(SEARCH_GEOS_LIB_PATH CACHE PATH "Path to the GEOS libs if not with GDAL")
    SET(CMAKE_LIBRARY_PATH ${CMAKE_PREFIX_PATH}/src)
    SET(CMAKE_INCLUDE_PATH ${CMAKE_PREFIX_PATH}/capi ${CMAKE_PREFIX_PATH}/include)

    FIND_PATH(GEOS_C_INCLUDE_DIR geos_c.h
            HINTS
            ${SEARCH_GEOS_LIB_PATH}/capi
            ${SEARCH_GEOS_LIB_PATH}/include
            )


    FIND_PATH(GEOS_INCLUDE_DIR geos.h
            HINTS
            ${SEARCH_GEOS_LIB_PATH}/source/headers
            ${SEARCH_GEOS_LIB_PATH}/include
            )

    FIND_LIBRARY(GEOS_LIBRARIES
            geos_c_i geos_c
            HINTS ${SEARCH_GEOS_LIB_PATH}/source
            ${SEARCH_GEOS_LIB_PATH}/src
            ${SEARCH_GEOS_LIB_PATH}/lib
            ${SEARCH_GEOS_LIB_PATH}
            /src
            /source
            src
            source
            )

    # if unix / mac and search geos
ELSE (WIN32)
    IF (SEARCH_GIS_LIB_PATH)
        FIND_PATH(GEOS_INCLUDE_DIR geos_c.h
                HINTS ${SEARCH_GIS_LIB_PATH}/include
                ${SEARCH_GIS_LIB_PATH}/GEOS.framework/unix/include
                ${SEARCH_GIS_LIB_PATH}
                NO_DEFAULT_PATH)

        FIND_LIBRARY(GEOS_LIBRARIES
                geos_c
                HINTS ${SEARCH_GIS_LIB_PATH}/lib ${SEARCH_GIS_LIB_PATH}
                ${SEARCH_GIS_LIB_PATH}/GEOS.framework/unix/lib
                NO_DEFAULT_PATH)
    ELSE (SEARCH_GIS_LIB_PATH)
        MESSAGE("Searching GEOS into standard paths")
        FIND_PATH(GEOS_INCLUDE_DIR geos_c.h
                HINTS ${SEARCH_GIS_LIB_PATH}/include
                ${SEARCH_GIS_LIB_PATH})

        FIND_LIBRARY(GEOS_LIBRARIES
                geos_c
                HINTS ${SEARCH_GIS_LIB_PATH}/lib ${SEARCH_GIS_LIB_PATH})
    ENDIF (SEARCH_GIS_LIB_PATH)


ENDIF (WIN32)


MARK_AS_ADVANCED(
        GEOS_INCLUDE_DIR
        GEOS_LIBRARIES
)


## IF GEOS PARAMETERS ARE DEFINED, USE THEM
IF (GEOS_INCLUDE_DIR)
    INCLUDE_DIRECTORIES(${GEOS_INCLUDE_DIR})
    #INCLUDE_DIRECTORIES(${SEARCH_GEOS_LIB_PATH}/source/headers)
    #MESSAGE ("DBG : GEOS include is ${GEOS_INCLUDE_DIR}")
ENDIF (GEOS_INCLUDE_DIR)

IF (GEOS_C_INCLUDE_DIR)
    #MESSAGE("GEOS C INCLUDE dir: ${GEOS_C_INCLUDE_DIR}")
    INCLUDE_DIRECTORIES(${GEOS_C_INCLUDE_DIR})
ENDIF (GEOS_C_INCLUDE_DIR)


IF (GEOS_LIBRARIES)
    LINK_LIBRARIES(${GEOS_LIBRARIES})
    #MESSAGE ("DBG : GEOS lis is ${GEOS_LIBRARIES}")
ENDIF (GEOS_LIBRARIES)

#debug message
MESSAGE(STATUS "GEOS lib is ${GEOS_LIBRARIES}")
MESSAGE(STATUS "GEOS include is ${GEOS_INCLUDE_DIR}")


