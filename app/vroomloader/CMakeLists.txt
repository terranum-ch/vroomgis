# CMAKE BUILD FILE FOR vroomLoader
# (c) TERRANUM 2020
cmake_minimum_required(VERSION 3.11)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project("vroomLoader")

#Export for sourcetrail
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#search all source and headers files
file(GLOB_RECURSE SRC_FILES src/*.cpp)
source_group(source FILES ${SRC_FILES})
file(GLOB_RECURSE HEAD_FILES "src/*.h")
source_group(header FILES ${HEAD_FILES})

include_directories(${CMAKE_SOURCE_DIR}/vroomgis/src)
include_directories(${CMAKE_SOURCE_DIR}/vroomgis/art)

# mac specific stuff
IF (APPLE)
    set(APP_ICON "vroomgis.icns")
    set(MACOSX_BUNDLE_INFO_STRING "VroomGIS test Program")
    set(MACOSX_BUNDLE_ICON_FILE ${APP_ICON})
    set(MACOSX_BUNDLE_BUNDLE_NAME "vroomLoader")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "Alpha")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "1.0.0")
    set(MACOSX_BUNDLE_COPYRIGHT "Lucien Schreiber (c) CREALP 2012")

    # copy icon
    set(APP_ICON_LOCATION "../../vroomgis/art/vroomgis.icns")
    SET_SOURCE_FILES_PROPERTIES(${APP_ICON_LOCATION} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    set(SRC_FILES ${SRC_FILES} ${APP_ICON_LOCATION})
ENDIF (APPLE)

# conditionnal compilation if windows needs ressource
IF (WIN32)
    source_group(resource FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/resource.rc)
    add_executable(vroomLoader WIN32 ${SRC_FILES} ${HEAD_FILES} "${CMAKE_CURRENT_SOURCE_DIR}/src/resource.rc")
ELSE (WIN32)
    add_executable(vroomLoader MACOSX_BUNDLE ${SRC_FILES} ${HEAD_FILES})
ENDIF (WIN32)

# LINK VROOMGIS LIBRARY
target_link_libraries(vroomLoader vroomgis-lib)

# Visual Leak Detector
if (WIN32)
    if (USE_VLD)
        target_link_libraries(vroomLoader ${VLD_LIBRARIES})
    endif (USE_VLD)
endif (WIN32)

