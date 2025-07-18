# CMAKE BUILD FILE FOR DRAWSPEEDTEST 
message(STATUS "Building drawspeedtest test application")
set(src_files "main.cpp")
add_executable(drawspeedtest ${src_files})
target_link_libraries(drawspeedtest vroomgis)

