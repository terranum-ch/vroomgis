MESSAGE(STATUS "USING UNIT TESTING")
SET(PROGNAMETEST ${CMAKE_PROJECT_NAME}_test)

# Unit testing path configuration
set(UNIT_TESTING_PATH CACHE PATH "Location of unit testing files and projects")
if (NOT UNIT_TESTING_PATH)
    message(FATAL_ERROR "UNIT_TESTING_PATH is not set. Please set it to the location of your unit testing files and projects.")
endif ()
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/test_param.h.in" "${PROJECT_BINARY_DIR}/test_param.h")
include_directories(${PROJECT_BINARY_DIR})

#search all source files
file(GLOB src_tests ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/vroomgis/src)
add_executable(${PROGNAMETEST} ${src_tests})

#target_link_libraries(${PROGNAMETEST ${CONAN_LIBS} "${PROJECT_NAME}_lib")
target_link_libraries(${PROGNAMETEST} "${PROJECT_NAME}")
add_test(NAME test COMMAND ${PROGNAMETEST})

# LINK LIBRARIES
#target_link_libraries(vroomgis-tests vroomgis)
# use system PNG for linux otherwise we have problem when displaying icons
if (UNIX AND NOT APPLE)
    message(STATUS "PNG lib for test is : ${PNG_LIBRARY}")
    target_link_libraries(vroomgis-tests ${PNG_LIBRARY} xkbcommon)
endif (UNIX AND NOT APPLE)
