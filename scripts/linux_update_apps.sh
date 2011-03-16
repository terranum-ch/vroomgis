#! /bin/sh

# Script for updating vroomloader
# (c) Lucien Schreiber CREALP 2010
#PARAMETERS
# script for updating vroomloader app under linux
# works only for linux
# 1) Update the svn repositories
# 2) Run cmake to configure
# 3) Run (make) the vroomGIS compilation
# 4) Launch the resulting program


TRUNKDIR=/home/lucien/programmation/ColtopGIS/vroomgis-trunk
BINDIR=/home/lucien/programmation/ColtopGIS/bin/vroomloader
BINDIR2=/home/lucien/programmation/ColtopGIS/bin/vroomtwin

VARLINE="----------------------------------"

echo "----------------------------------------"
echo "      UPDATE VROOMGIS�APPLICATIONS"
echo  "    (c) Lucien Schreiber CREALP"
echo "----------------------------------------"
echo "Should we totally clean the build directory (bin) (y/n) if not sure say yes"
read value



echo "----------------------------------------"
echo "         UPDATE VROOMLOADER"
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

cmake $TRUNKDIR/app/vroomloader/build -G "CodeBlocks - Unix Makefiles" -DSEARCH_GDAL:BOOL=1 -DSEARCH_GEOS:BOOL=1 -DSEARCH_GIS_LIB_PATH:PATH="/home/lucien/programmation/LIB/_LIBGIS" -DCMAKE_WXWINDOWS_WXCONFIG_EXECUTABLE:FILE="/home/lucien/programmation/LIB/_LIBWX/bin/wx-config" -DUSE_VERSION:BOOL=1 -DwxWIDGETS_USING_SVN:BOOL=1 -DwxWIDGETS_PATH_SVN="/home/lucien/programmation/LIB/wxWidgets-SVN"

echo $VARLINE
echo "4) Making the binary"
make --jobs=8
echo "4) Making the binary ... DONE"

echo $VARLINE
echo "5) Running..-"
./vroomLoader &
echo "5) vroomloader LAUNCHED"




echo "----------------------------------------"
echo "         UPDATE VROOMTWIN"
echo  "    (c) Lucien Schreiber CREALP"
echo "----------------------------------------"


echo "$value"
if [ "$value" = "y" ] ; then 
   rm -rvf $BINDIR2
   mkdir $BINDIR2
else
   echo "build directory not deleted"
fi


echo "3) configuring ..."
cd $BINDIR2

cmake $TRUNKDIR/app/vroomtwin/build -G "CodeBlocks - Unix Makefiles" -DSEARCH_GDAL:BOOL=1 -DSEARCH_GEOS:BOOL=1 -DSEARCH_GIS_LIB_PATH:PATH="/home/lucien/programmation/LIB/_LIBGIS" -DCMAKE_WXWINDOWS_WXCONFIG_EXECUTABLE:FILE="/home/lucien/programmation/LIB/_LIBWX/bin/wx-config" -DUSE_VERSION:BOOL=1 -DwxWIDGETS_USING_SVN:BOOL=1 -DwxWIDGETS_PATH_SVN="/home/lucien/programmation/LIB/wxWidgets-SVN"

echo $VARLINE
echo "4) Making the binary"
make
echo "4) Making the binary ... DONE"

echo $VARLINE
echo "5) Running..-"
./vroomTwin &
echo "5) vroomloader LAUNCHED"



