message(STATUS "Building vroomDrawer Application")

#search source and headers files
set(SRC_FILES "src/frame.cpp"
        "src/vrlayervectorstar.cpp"
        "src/vrmemorylayerdialog.cpp"
        "art/vroomdrawer_toolbmp.cpp"
        "src/resource.rc"  # resource file for Windows
)

include_directories(${CMAKE_SOURCE_DIR}/vroomgis/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/art)

IF (APPLE)
    set(APP_ICON "vroomgis.icns")
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "my.domain.style.identifier.vroomdrawer")
    set(MACOSX_BUNDLE_INFO_STRING "VroomGIS test Program")
    set(MACOSX_BUNDLE_ICON_FILE ${APP_ICON})
    set(MACOSX_BUNDLE_BUNDLE_NAME "vroomDrawer")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "Alpha")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "1.0.0")
    set(MACOSX_BUNDLE_COPYRIGHT "Lucien Schreiber (c) 2025")

    # copy icon
    set(APP_ICON_LOCATION "../../vroomgis/art/vroomgis.icns")
    SET_SOURCE_FILES_PROPERTIES(${APP_ICON_LOCATION} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    set(SRC_FILES ${SRC_FILES} ${APP_ICON_LOCATION})
ENDIF (APPLE)

add_executable(vroomDrawer MACOSX_BUNDLE WIN32 ${SRC_FILES})
target_link_libraries(vroomDrawer vroomgis)

# use system PNG for linux otherwise we have problem when displaying icons
if (UNIX AND NOT APPLE)
    message(STATUS "PNG lib for vroomDrawer is : ${PNG_LIBRARY}")
    target_link_libraries(vroomDrawer ${PNG_LIBRARY} xkbcommon)
endif (UNIX AND NOT APPLE)

# Install step for copying *dylib into application bundle on OSX and for changing their reference from
# absolute to relative
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})
IF (APPLE)
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
