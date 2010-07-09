#! /bin/sh

# Script for updating vroomloader
# (c) Lucien Schreiber CREALP 2010
#PARAMETERS

# works only for Mac
# Do not modify for your paths but copy outside the 
# trunk and edit for your paths


# 1) Update the svn repositories
# 1b) remove main.cpp
# 2) Run cmake to update the Visual studio solution
# 3) Run (manually) the vroomGIS compilation
# 4) Launch the tests


TRUNKDIR=/Users/lucien/Documents/PRJ/COLTOPGIS/trunk-vroomgis
BINDIR=/Users/lucien/Documents/PRJ/COLTOPGIS/bin/vroomloader
BINDIR2=/Users/lucien/Documents/PRJ/COLTOPGIS/bin/vroomtwin


VARLINE="----------------------------------"


echo "----------------------------------------"
echo "        UPDATING VROOMGIS SAMPLES "
echo  "    (c) Lucien Schreiber CREALP"
echo "----------------------------------------"
echo "Should we totally clean the build directory before building (bin) (y/n) if not sure say yes"
read value
echo -n "Build release version (y/n) ?"
read releasebuild





echo "----------------------------------------"
echo "         UPDATE VROOMLOADER "
echo  "    (c) Lucien Schreiber CREALP"
echo "----------------------------------------"
echo "1) Updating SVN ..."

cd $TRUNKDIR
svn update
VARSVNNUMBER=`svnversion -n`
echo '1) Updating SVN .... DONE (\c'
echo  $VARSVNNUMBER ')'	
echo $VARLINE

echo "$value"
if [ "$value" = "y" ] ; then 
   rm -rvf $BINDIR
   mkdir $BINDIR
else
   echo "build directory not deleted"
fi

echo "3) configuring ..."
cd $BINDIR

cmake -GXcode $TRUNKDIR/app/vroomloader/build -DCMAKE_OSX_ARCHITECTURES:TEXT="x86_64" -DSEARCH_GDAL:BOOL=1 -DSEARCH_GEOS:BOOL=1 -DSEARCH_GIS_LIB_PATH:PATH="/Users/lucien/Documents/PROGRAMMATION/64/_LIBGIS" -DCMAKE_WXWINDOWS_WXCONFIG_EXECUTABLE:FILE="/Users/lucien/Documents/PROGRAMMATION/64/_LIBWXSVN/bin/wx-config" -DUSE_VERSION:BOOL=1 -DwxWIDGETS_USING_SVN:BOOL=1 -DwxWIDGETS_PATH_SVN="/Users/lucien/Documents/PROGRAMMATION/64/wxWidgets-svn"

echo $VARLINE
echo "4) Making DEBUG"
xcodebuild -configuration Debug
echo "4) Making DEBUG ... DONE"


if [ "$releasebuild" == "y" ]  
then 
	echo $VARLINE
	echo "4) Making RELEASE"
	xcodebuild -configuration Release
	echo "4) Making RELEASE ... DONE"
fi


echo $VARLINE
echo "Starting Debug vroomloader"
open $BINDIR/Debug/vroomLoader.app


echo "----------------------------------------"
echo "         UPDATE VROOMTWIN "
echo  "    (c) Lucien Schreiber CREALP"
echo "----------------------------------------"
echo "1) Updating SVN ..."

echo "$value"
if [ "$value" = "y" ] ; then 
   rm -rvf $BINDIR2
   mkdir $BINDIR2
else
   echo "build directory not deleted"
fi

echo "3) configuring ..."
cd $BINDIR2

cmake -GXcode $TRUNKDIR/app/vroomtwin/build -DCMAKE_OSX_ARCHITECTURES:TEXT="x86_64" -DSEARCH_GDAL:BOOL=1 -DSEARCH_GEOS:BOOL=1 -DSEARCH_GIS_LIB_PATH:PATH="/Users/lucien/Documents/PROGRAMMATION/64/_LIBGIS" -DCMAKE_WXWINDOWS_WXCONFIG_EXECUTABLE:FILE="/Users/lucien/Documents/PROGRAMMATION/64/_LIBWXSVN/bin/wx-config" -DUSE_VERSION:BOOL=1 -DwxWIDGETS_USING_SVN:BOOL=1 -DwxWIDGETS_PATH_SVN="/Users/lucien/Documents/PROGRAMMATION/64/wxWidgets-svn"

echo $VARLINE
echo "4) Making DEBUG"
xcodebuild -configuration Debug
echo "4) Making DEBUG ... DONE"



if [ "$releasebuild" == "y" ]  
then 
	echo $VARLINE
	echo "4) Making RELEASE"
	xcodebuild -configuration Release
	echo "4) Making RELEASE ... DONE"
fi

echo $VARLINE
echo "Starting Debug vroomTwin"
open $BINDIR2/Debug/vroomTwin.app
