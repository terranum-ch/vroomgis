# CMAKE BUILD FILE FOR VROOMDRAWER
# (c) TERRANUM 2020
cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project("vroomDrawer")

#search source and headers files
file(GLOB_RECURSE SRC_FILES "src/*.cpp")
source_group(source FILES ${SRC_FILES})
file(GLOB_RECURSE HEAD_FILES "src/*.h")
source_group(header FILES ${HEAD_FILES})

# add art source files
list(APPEND SRC_FILES "art/vroomdrawer_toolbmp.cpp")
list(APPEND HEAD_FILES "art/vroomdrawer_toolbmp.h")

include_directories(${CMAKE_SOURCE_DIR}/vroomgis/src)

IF (APPLE)
    set(APP_ICON "vroomgis.icns")
    set(MACOSX_BUNDLE_INFO_STRING "VroomGIS test Program")
    set(MACOSX_BUNDLE_ICON_FILE ${APP_ICON})
    set(MACOSX_BUNDLE_BUNDLE_NAME "vroomDrawer")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "Alpha")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "1.0.0")
    set(MACOSX_BUNDLE_COPYRIGHT "Lucien Schreiber (c) 2013")

    # copy icon
    set(APP_ICON_LOCATION "../../vroomgis/art/vroomgis.icns")
    SET_SOURCE_FILES_PROPERTIES(${APP_ICON_LOCATION} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    set(SRC_FILES ${SRC_FILES} ${APP_ICON_LOCATION})
ENDIF (APPLE)


# conditionnal compilation, Windows needs resource.rc
IF (WIN32)
    source_group(resource FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/resource.rc)
    add_executable(vroomDrawer WIN32 ${SRC_FILES} ${HEAD_FILES} "${CMAKE_CURRENT_SOURCE_DIR}/src/resource.rc")
ELSE (WIN32)
    add_executable(vroomDrawer MACOSX_BUNDLE ${SRC_FILES} ${HEAD_FILES})
ENDIF (WIN32)

# use system PNG for linux otherwise we have problem when displaying icons
target_link_libraries(vroomDrawer vroomgis)
if (UNIX AND NOT APPLE)
    message(STATUS "PNG lib for vroomDrawer is : ${PNG_LIBRARY}")
    target_link_libraries(vroomDrawer ${PNG_LIBRARY} xkbcommon)
endif (UNIX AND NOT APPLE)


# Install step for copying *dylib into application bundle on OSX and for changing their reference from
# absolute to relative
IF (APPLE)
    # change cmake_install_prefix from /usr/local to the binary directory
    set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})
    set(APPS "${CMAKE_INSTALL_PREFIX}/bin/vroomDrawer.app")  # paths to executables
    set(DIRS ${CMAKE_INSTALL_PREFIX}/bin/)  # paths to executables
    INSTALL(CODE "
   include(BundleUtilities)
   fixup_bundle(\"${APPS}\"   \"\"   \"${DIRS}\")
   " COMPONENT Runtime)
ENDIF (APPLE)

install(TARGETS vroomDrawer
        BUNDLE DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
        RUNTIME DESTINATION bin)

# Visual Leak Detector
if (WIN32)
    if (USE_VLD)
        target_link_libraries(vroomDrawer ${VLD_LIBRARIES})
    endif (USE_VLD)
endif (WIN32)
