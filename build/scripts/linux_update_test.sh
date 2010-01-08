#! /bin/sh

# Script for updating and running tests
# (c) Lucien Schreiber CREALP 2010
#PARAMETERS
# script for updating vroomGIS test under windows
# works only for linux
# 1) Update the svn repositories
# 1b) remove main.cpp
# 2) Run cmake to update the Visual studio solution
# 3) Run (manually) the vroomGIS compilation
# 4) Launch the tests


TRUNKDIR=/home/lucien/Documents/ColtopGIS/trunk-vroomgis
BINDIR=/home/lucien/Documents/ColtopGIS/bin/vroomgistest

VARLINE="----------------------------------"


echo "----------------------------------------"
echo "         UPDATE VROOMGIS TESTS"
echo  "    (c) Lucien Schreiber CREALP"
echo "----------------------------------------"
echo "1) Updating SVN ..."

cd $TRUNKDIR
svn update
VARSVNNUMBER=`svnversion -n`
echo '1) Updating SVN .... DONE (\c'
echo  $VARSVNNUMBER ')'	
echo $VARLINE

echo "2) REMOVING MAIN.CPP, will be generated again"
rm $BINDIR/main.cpp

echo "3) configuring ..."
cd $BINDIR

cmake $TRUNKDIR/test/build -DCXXTEST_DIRECTORY:PATH="/home/lucien/Documents/LIB/cxxtest" -DSEARCH_GDAL:BOOL=1 -DSEARCH_GEOS:BOOL=1 -DSEARCH_GIS_LIB_PATH:PATH="/home/lucien/Documents/LIB/_LIBGIS" -DUNIT_TESTING_PATH:PATH="/home/lucien/Documents/ColtopGIS/test_data" -DCMAKE_WXWINDOWS_WXCONFIG_EXECUTABLE:FILE="/home/lucien/Documents/LIB/_LIBWXSVN/bin/wx-config"

echo $VARLINE
echo "4) Making the tests"
make
echo "4) Making the tests ... DONE"

echo $VARLINE
echo "5) Running the tests"
ctest
scite $BINDIR/Testing/Temporary/LastTest.log
echo "5) Running the tests... DONE"



