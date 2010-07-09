#! /bin/sh

# Script for updating and running tests
# (c) Lucien Schreiber CREALP 2010
#PARAMETERS
# script for updating vroomGIS test under windows
# works only for Mac
# 1) Update the svn repositories
# 1b) remove main.cpp
# 2) Run cmake to update the Visual studio solution
# 3) Run (manually) the vroomGIS compilation
# 4) Launch the tests


TRUNKDIR=/Users/lucien/Documents/PRJ/COLTOPGIS/trunk-vroomgis
BINDIR=/Users/lucien/Documents/PRJ/COLTOPGIS/bin/vroomgistest

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

echo "Should we totally clean the build directory (bin) (y/n) if not sure say yes"
read value
echo "$value"
if [ "$value" = "y" ] ; then 
   rm -rvf $BINDIR
   mkdir $BINDIR
else
   echo "build directory not deleted"
fi

echo "3) configuring ..."
cd $BINDIR

cmake -GXcode $TRUNKDIR/test/build -DCMAKE_OSX_ARCHITECTURES:TEXT="x86_64" -DCXXTEST_DIRECTORY:PATH="/Users/lucien/Documents/PROGRAMMATION/cxxtest" -DSEARCH_GDAL:BOOL=1 -DSEARCH_GEOS:BOOL=1 -DSEARCH_GIS_LIB_PATH:PATH="/Users/lucien/Documents/PROGRAMMATION/64/_LIBGIS" -DUNIT_TESTING_PATH:PATH="/Users/lucien/Documents/PRJ/COLTOPGIS/test_data" -DCMAKE_WXWINDOWS_WXCONFIG_EXECUTABLE:FILE="/Users/lucien/Documents/PROGRAMMATION/64/_LIBWXSVN/bin/wx-config"

echo $VARLINE
echo "4) Making the tests"
xcodebuild
echo "4) Making the tests ... DONE"

echo $VARLINE
echo "5) Running the tests"
ctest
open -a "MacVim" $BINDIR/Testing/Temporary/LastTest.log &
echo "5) Running the tests... DONE"



