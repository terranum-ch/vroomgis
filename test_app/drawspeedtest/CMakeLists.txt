# CMAKE BUILD FILE FOR DRAWSPEEDTEST 
# (c) LUCIEN SCHREIBER CREALP 2013
cmake_minimum_required(VERSION 3.11)

project("drawSpeedTest")

#search all source files
file(GLOB_RECURSE SRC_FILES "src/*.cpp")
file(GLOB_RECURSE HEAD_FILES "src/*.h")
source_group(source FILES ${SRC_FILES})
source_group(header FILES ${HEAD_FILES})

# WXWIDGETS
set(WXWINDOWS_USE_GL 0)

add_executable(${CMAKE_PROJECT_NAME} ${HEAD_FILES} ${SRC_FILES})

