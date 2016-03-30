# Find SQLite Library
# Created by Lucien Schreiber 
#
#  SQLITE_INCLUDE_DIR - where to find sqlite3.h, etc.
#  SQLITE_LIBRARIES   - List of libraries when using sqlite.

SET(SQLITE_PATH CACHE PATH "Path to the SQLite library")
#SET(CMAKE_FIND_FRAMEWORK NEVER)
# MESSAGE("NEVER SEARCHING FRAMEWORK")

# SEARCHING HEADER
FIND_PATH(SQLITE_INCLUDE_DIR sqlite3.h
        HINTS ${SQLITE_PATH}
        ${SQLITE_PATH}/include)

IF (NOT SQLITE_INCLUDE_DIR)
    MESSAGE(FATAL_ERROR "sqlite3.h file not found")
ENDIF (NOT SQLITE_INCLUDE_DIR)
INCLUDE_DIRECTORIES(${SQLITE_INCLUDE_DIR})

# SEARCHING LIBRARY
FIND_LIBRARY(SQLITE_LIBRARIES sqlite3 NAMES sqlite3_i
        HINTS ${SQLITE_PATH}
        ${SQLITE_PATH}/lib)

MESSAGE("${SQLITE_PATH}/lib   ${SQLITE_INCLUDE_DIR} ")

IF (NOT SQLITE_LIBRARIES)
    MESSAGE(FATAL_ERROR "SQLite library not found")
ENDIF (NOT SQLITE_LIBRARIES)
MESSAGE("SQLite libraries : ${SQLITE_LIBRARIES}")
LINK_LIBRARIES(${SQLITE_LIBRARIES})
