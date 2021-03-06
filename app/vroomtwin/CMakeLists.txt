# CMAKE BUILD FILE FOR vroomTwin
# (c) TERRANUM 2020
cmake_minimum_required(VERSION 3.11)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

PROJECT("vroomTwin")

#search all source and headers files
FILE(GLOB_RECURSE SRC_FILES src/*.cpp)
SOURCE_GROUP(source FILES ${SRC_FILES})
FILE(GLOB_RECURSE HEAD_FILES "src/*.h")
SOURCE_GROUP(header FILES ${HEAD_FILES})

# FIND WXWIDGETS
# FOR WINDOWS, DO NOT FORGET TO DEFINE WXWIN VARIABLE
# FOR UNIX, SET ENVIRONNEMENT VAR : export WX_CONFIG=<path to wxconfig> before calling cmake
find_package(wxWidgets COMPONENTS all REQUIRED)
include(${wxWidgets_USE_FILE})

#CREATE VROOMGIS LIBRARY AND WXWIDGETS - GDAL LIB
INCLUDE("../../vroomgis/build/cmake/Use_vroomGISlib.cmake")

# wxVERSION PLUGIN
include(FetchContent)
FetchContent_Declare(
        wxVersion
        GIT_REPOSITORY https://github.com/terranum-ch/wxVersion.git
        GIT_TAG        476a09746b5bb7ab134f8174602d8455d5693949
)
FetchContent_MakeAvailable(wxVersion)
message("${wxVersion_SOURCE_DIR} source dir")
INCLUDE_DIRECTORIES("${wxVersion_SOURCE_DIR}/src")

# Visual Leak Detector
set(CMAKE_MODULE_PATH ${VROOMGIS_PATH}/../build/cmake)
if (WIN32)
    set(USE_VLD 0 CACHE BOOL "Sould we use Visual Leak Detector (https://vld.codeplex.com) ?")
    if (USE_VLD)
        find_package(VLD)
        include_directories(${VLD_INCLUDE_DIRS})
        add_definitions(-DUSE_VLD)
    endif (USE_VLD)
endif (WIN32)

IF (APPLE)
    SET(APP_ICON "vroomgis.icns")
    SET(MACOSX_BUNDLE_INFO_STRING "VroomGIS test Program")
    SET(MACOSX_BUNDLE_ICON_FILE ${APP_ICON})
    SET(MACOSX_BUNDLE_BUNDLE_NAME "${CMAKE_PROJECT_NAME}")
    SET(MACOSX_BUNDLE_SHORT_VERSION_STRING "Alpha")
    SET(MACOSX_BUNDLE_BUNDLE_VERSION "1.0.0")
    SET(MACOSX_BUNDLE_COPYRIGHT "Lucien Schreiber (c) CREALP 2012")

    # copy icon
    SET(APP_ICON_LOCATION "../../vroomgis/art/vroomgis.icns")
    SET_SOURCE_FILES_PROPERTIES(${APP_ICON_LOCATION} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    SET(SRC_FILES ${SRC_FILES} ${APP_ICON_LOCATION})
ENDIF (APPLE)


# conditionnal compilation if windows needs ressource
IF (WIN32)
    SOURCE_GROUP(resource FILES src/resource.rc)
    ADD_EXECUTABLE(${CMAKE_PROJECT_NAME} WIN32 ${SRC_FILES} ${HEAD_FILES} src/resource.rc)
ELSE (WIN32)
    ADD_EXECUTABLE(${CMAKE_PROJECT_NAME} MACOSX_BUNDLE ${SRC_FILES} ${HEAD_FILES})
ENDIF (WIN32)
TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} ${wxWidgets_LIBRARIES})


# LINK VROOMGIS LIBRARY
TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} ${VROOMGIS_LIBRARY})

# LINK WXVERSION PLUGIN
TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} wxVersion)

# Visual Leak Detector
if (WIN32)
    if (USE_VLD)
        target_link_libraries(${CMAKE_PROJECT_NAME} ${VLD_LIBRARIES})
    endif (USE_VLD)
endif (WIN32)

# copy libs to bundle if we are under mac...
# check is done internally.
INCLUDE("../../vroomgis/build/cmake/installMacGISLIB.cmake")


