# SCRIPT FOR BUILDING lsCrashReport
#OPTIONS - PARAMETERS 
set(LSCRASHREPORT_NAME "lsCrashReport")
set(USE_CRASHREPORT 1 CACHE BOOL "Use ${LSCRASHREPORT_NAME} component?")
IF (USE_CRASHREPORT)
    #FINDING OUR PATH
    FIND_PATH(LSCRASHREPORT_PATH "lscrashreport.h"
            "${CMAKE_CURRENT_LIST_DIR}/../src"
            "/plugins/lscrashreport/src/"
            "../plugins/lscrashreport/src/"
            "../../plugins/lscrashreport/src/"
            "../../../plugins/lscrashreport/src/"
            "../../../../plugins/lscrashreport/src/"
            "/lib/lscrashreport/src/"
            "../lib/lscrashreport/src/"
            "../../lib/lscrashreport/src/"
            "../../../lib/lscrashreport/src/"
            "../../../../lib/lscrashreport/src/"
            "/lscrashreport/src/"
            "../lscrashreport/src/"
            "../../lscrashreport/src/"
            "../../../lscrashreport/src/"
            "../../../../lscrashreport/src/"
            NO_DEFAULT_PATH)
    IF (NOT LSCRASHREPORT_PATH)
        message(FATAL_ERROR "Unable to find lscrashreport.h")
    ENDIF (NOT LSCRASHREPORT_PATH)

    # CURL for sending crashreport
    mark_as_advanced(CLEAR CURL_INCLUDE_DIR)
    mark_as_advanced(CLEAR CURL_LIBRARY)
    find_package(CURL REQUIRED)
    include_directories(${CURL_INCLUDE_DIRS})

    #
    # CREATE LIBRARY
    #
    #search all source files
    #message(${LSCRASHREPORT_PATH})
    file(GLOB LSCRASH_SRC_FILES "${LSCRASHREPORT_PATH}/*.cpp")
    source_group(source FILES ${LSCRASH_SRC_FILES})

    # search all headers files
    file(GLOB LSCRASH_HEAD_FILES "${LSCRASHREPORT_PATH}/*.h")
    source_group(header FILES ${LSCRASH_HEAD_FILES})
    include_directories("${LSCRASHREPORT_PATH}")
    add_library("${LSCRASHREPORT_NAME}" STATIC ${LSCRASH_SRC_FILES} ${LSCRASH_HEAD_FILES})

    #message(" src:::::: ${LSCRASH_SRC_FILES}")
ENDIF (USE_CRASHREPORT)

configure_file("${LSCRASHREPORT_PATH}/lscrashreport_param.h.in" "${PROJECT_BINARY_DIR}/lscrashreport_param.h")
include_directories(${PROJECT_BINARY_DIR})





