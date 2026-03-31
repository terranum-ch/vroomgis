# CMAKE BUILD FILE FOR CREATING A STATIC VROOMGIS Library
# VROOMGIS NEEDS wxWidgets, GEOS, GDAL

set(VROOMGIS_SRC_FILES
        "src/tmpercent.cpp"
        "src/vrbitmaps.cpp"
        "src/vrcoordinate.cpp"
        "src/vrdisplaytool.cpp"
        "src/vrdriver.cpp"
        "src/vrlabel.cpp"
        "src/vrlabelc2p.cpp"
        "src/vrlayer.cpp"
        "src/vrlayermanager.cpp"
        "src/vrlayerraster.cpp"
        "src/vrlayerrasterc2d.cpp"
        "src/vrlayervector.cpp"
        "src/vrlayervectorc2p.cpp"
        "src/vroperationvectorprofiler.cpp"
        "src/vrperformance.cpp"
        "src/vrprogress.cpp"
        "src/vrrealrect.cpp"
        "src/vrrender.cpp"
        "src/vrrendercoltop.cpp"
        "src/vrrenderer.cpp"
        "src/vrrendervectorc2p.cpp"
        "src/vrrubberband.cpp"
        "src/vrserialize.cpp"
        "src/vrshapeeditor.cpp"
        "src/vrviewerdisplay.cpp"
        "src/vrviewerlayermanager.cpp"
        "src/vrvieweroverlay.cpp"
        "src/vrviewertoc.cpp"
)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/art")

add_library(vroomgis STATIC ${VROOMGIS_SRC_FILES})

if (BUILD_VROOMGIS_WITH_CONAN)
    target_link_libraries(vroomgis ${CONAN_LIBS})
else (BUILD_VROOMGIS_WITH_CONAN)
    find_package(PROJ CONFIG REQUIRED)
    find_package(GDAL CONFIG REQUIRED)
    find_package(PNG CONFIG REQUIRED)
    # Try to find wxWidgets, with fallback to FetchContent targets
    if (NOT wxWidgets_FOUND)
        find_package(wxWidgets CONFIG QUIET)
    endif()

    if (wxWidgets_FOUND AND TARGET wx::core)
        # wxWidgets was found via find_package or FetchContent
        message(STATUS "wxWidgets found: ${wxWidgets_DIR}")
    elseif (TARGET wxcore)
        # Fallback: wxWidgets built via FetchContent but without config files
        set(wxWidgets_FOUND TRUE)
        message(STATUS "Using wxWidgets targets from FetchContent")
    else()
        message(FATAL_ERROR "wxWidgets not found and no FetchContent targets available")
    endif()

    # Link libraries - try the standard names first, fallback to FetchContent targets
    if (TARGET wx::core)
        target_link_libraries(vroomgis wx::core wx::base wx::net GDAL::GDAL GEOS::geos_c PROJ::proj)
    elseif (TARGET wxcore)
        target_link_libraries(vroomgis wxcore wxbase wxnet GDAL::GDAL GEOS::geos_c PROJ::proj)
    else()
        message(FATAL_ERROR "No wxWidgets targets found to link against")
    endif()
endif (BUILD_VROOMGIS_WITH_CONAN)

if (UNIX AND NOT APPLE)
    message(STATUS "PNG_LIBRARY for vroomgis: ${PNG_LIBRARY}")
    target_link_libraries(vroomgis ${PNG_LIBRARY} xkbcommon)
endif (UNIX AND NOT APPLE)

# USED FOR PERFORMANCE UNDER WINDOWS
IF (WIN32)
    target_link_libraries(vroomgis "psapi")
ENDIF (WIN32)
