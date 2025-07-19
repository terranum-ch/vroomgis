message(STATUS "Generating version information for ${PROJECT_NAME} in ${PROJECT_SOURCE_DIR}")
execute_process(COMMAND git rev-parse --short HEAD
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        OUTPUT_VARIABLE VR_GIT_REV
        ERROR_QUIET)

# Check whether we got any revision (which isn't
# always the case, e.g. when someone downloaded a zip
# file from Github instead of a checkout)
if ("${VR_GIT_REV}" STREQUAL "")
    set(VR_GIT_REV "N/A")
    set(VR_GIT_TAG "N/A")
    set(VR_GIT_BRANCH "N/A")
    set(VR_GIT_NUMBER "N/A")
else ()
    execute_process(
            COMMAND git describe --exact-match --tags
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            OUTPUT_VARIABLE VR_GIT_TAG ERROR_QUIET)
    execute_process(
            COMMAND git rev-parse --abbrev-ref HEAD
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            OUTPUT_VARIABLE VR_GIT_BRANCH)
    execute_process(
            COMMAND git rev-list HEAD --count
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            OUTPUT_VARIABLE VR_GIT_NUMBER)
    string(STRIP "${VR_GIT_REV}" VR_GIT_REV)
    string(STRIP "${VR_GIT_TAG}" VR_GIT_TAG)
    string(STRIP "${VR_GIT_BRANCH}" VR_GIT_BRANCH)
    string(STRIP "${VR_GIT_NUMBER}" VR_GIT_NUMBER)
endif ()

set(VERSION
        "#ifndef _VR_VERSION_H_
        #define _VR_VERSION_H_
        const char* const ${PROJECT_NAME}_MAJOR_VERSION=\"${${PROJECT_NAME}_MAJOR_VERSION}\";
        const char* const ${PROJECT_NAME}_MINOR_VERSION=\"${${PROJECT_NAME}_MINOR_VERSION}\";
        const char* const VR_GIT_REV=\"${VR_GIT_REV}\";
        const char* const VR_GIT_TAG=\"${VR_GIT_TAG}\";
        const char* const VR_GIT_BRANCH=\"${VR_GIT_BRANCH}\";
        const char* const VR_GIT_NUMBER=\"${VR_GIT_NUMBER}\";
        #endif")

set(VERSION_FILEPATH ${PROJECT_SOURCE_DIR}/vroomgis/src/vrversion.h)

if (EXISTS ${VERSION_FILEPATH})
    file(READ ${VERSION_FILEPATH} VERSION_)
else ()
    set(VERSION_ "")
endif ()

if (NOT "${VERSION}" STREQUAL "${VERSION_}")
    file(WRITE ${VERSION_FILEPATH} "${VERSION}")
endif ()

message(STATUS "${PROJECT_NAME} Version: ${${PROJECT_NAME}_MAJOR_VERSION}.${${PROJECT_NAME}_MINOR_VERSION}.${VR_GIT_NUMBER}")
message(STATUS "Revision: ${VR_GIT_REV}")