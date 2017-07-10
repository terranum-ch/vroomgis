# CMAKE BUILD FILE FOR CREATING A STATIC VROOMGIS Library
# (c) LUCIEN SCHREIBER 2013 
# VROOMGIS NEEDS wxWidgets, GEOS, GDAL
cmake_minimum_required(VERSION 2.6)

PROJECT("vroomgis")

include(${CMAKE_CURRENT_LIST_DIR}/build/cmake/Use_vroomGISlib.cmake)
