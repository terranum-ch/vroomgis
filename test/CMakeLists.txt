MESSAGE(STATUS "USING UNIT TESTING")
SET(PROGNAMETEST ${CMAKE_PROJECT_NAME}_test)

# Unit testing path configuration
set(UNIT_TESTING_PATH ${CMAKE_CURRENT_SOURCE_DIR}/data)
set(UNIT_TESTING_PATH_OUTPUT ${CMAKE_BINARY_DIR}/output_test_data)
# create output directory if it does not exist
if (NOT EXISTS ${UNIT_TESTING_PATH_OUTPUT})
    file(MAKE_DIRECTORY ${UNIT_TESTING_PATH_OUTPUT})
endif ()
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/test_param.h.in" "${PROJECT_BINARY_DIR}/test_param.h")
include_directories(${PROJECT_BINARY_DIR})

#search all source files
file(GLOB src_tests ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/vroomgis/src)
add_executable(${PROGNAMETEST} ${src_tests})

#target_link_libraries(${PROGNAMETEST ${CONAN_LIBS} "${PROJECT_NAME}_lib")
target_link_libraries(${PROGNAMETEST} "${PROJECT_NAME}")
include(GoogleTest)
#add_test(NAME test COMMAND ${PROGNAMETEST})
gtest_discover_tests(${PROGNAMETEST}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src
        TEST_PREFIX "vroomgis."
        DISCOVERY_TIMEOUT 60
)

# LINK LIBRARIES
#target_link_libraries(vroomgis-tests vroomgis)
# use system PNG for linux otherwise we have problem when displaying icons
if (UNIX AND NOT APPLE)
    message(STATUS "PNG lib for test is : ${PNG_LIBRARY}")
    target_link_libraries(${PROGNAMETEST} ${PNG_LIBRARY} xkbcommon)
endif (UNIX AND NOT APPLE)
