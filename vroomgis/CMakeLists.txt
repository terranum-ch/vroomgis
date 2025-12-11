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
    find_package(wxWidgets CONFIG REQUIRED)
    find_package(PROJ CONFIG REQUIRED)
    find_package(GDAL CONFIG REQUIRED)
    find_package(PNG CONFIG REQUIRED)
    find_package(GEOS CONFIG REQUIRED)
    target_link_libraries(vroomgis wx::core wx::base wx::net GDAL::GDAL GEOS::geos_c PROJ::proj)
endif (BUILD_VROOMGIS_WITH_CONAN)

if (UNIX AND NOT APPLE)
    message(STATUS "PNG_LIBRARY for vroomgis: ${PNG_LIBRARY}")
    target_link_libraries(vroomgis ${PNG_LIBRARY} xkbcommon)
endif (UNIX AND NOT APPLE)

# USED FOR PERFORMANCE UNDER WINDOWS
IF (WIN32)
    target_link_libraries(vroomgis "psapi")
ENDIF (WIN32)
