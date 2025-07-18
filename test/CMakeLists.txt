MESSAGE(STATUS "Using unit testing for ${CMAKE_PROJECT_NAME}")

# Unit testing path configuration
set(UNIT_TESTING_PATH ${CMAKE_CURRENT_SOURCE_DIR}/data)
set(UNIT_TESTING_PATH_OUTPUT ${CMAKE_BINARY_DIR}/output_test_data)

# create output directory if it does not exist
if (NOT EXISTS ${UNIT_TESTING_PATH_OUTPUT})
    file(MAKE_DIRECTORY ${UNIT_TESTING_PATH_OUTPUT})
endif ()
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/test_param.h.in" "${PROJECT_BINARY_DIR}/test_param.h")
include_directories(${PROJECT_BINARY_DIR})

set(src_tests
        "src/fake_vrviewerdisplay.cpp"
        "src/main.cpp"
        "src/test_setup.cpp"
        "src/test_vrcoordinate.cpp"
        "src/test_vrdriver.cpp"
        "src/test_vrlabel.cpp"
        "src/test_vrlayermanager.cpp"
        "src/test_vrlayerrasterC2D.cpp"
        "src/test_vrlayerrasterGDAL.cpp"
        "src/test_vrlayervectorOGR.cpp"
        "src/test_vroperationvector.cpp"
        "src/test_vrrealrect.cpp"
        "src/test_vrrender.cpp"
        "src/test_vrrenderc2pdips.cpp"
        "src/test_vrrenderer.cpp"
        "src/test_vrrubberband.cpp"
        "src/test_vrviewerlayermanager.cpp"
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/vroomgis/src)

add_executable(Unittest ${src_tests})
target_link_libraries(Unittest vroomgis)

include(GoogleTest)
gtest_discover_tests(Unittest
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src
        DISCOVERY_TIMEOUT 60
)

# use system PNG for linux otherwise we have problem when displaying icons
if (UNIX AND NOT APPLE)
    message(STATUS "PNG lib for test is : ${PNG_LIBRARY}")
    target_link_libraries(Unittest ${PNG_LIBRARY} xkbcommon)
endif (UNIX AND NOT APPLE)
