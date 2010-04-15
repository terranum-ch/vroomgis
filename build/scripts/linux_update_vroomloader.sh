#! /bin/sh

# Script for updating vroomloader
# (c) Lucien Schreiber CREALP 2010
#PARAMETERS
# script for updating vroomloader app under linuxe
# works only for linux
# 1) Update the svn repositories
# 2) Run cmake to configure
# 3) Run (make) the vroomGIS compilation
# 4) Launch the resulting program


TRUNKDIR=/home/lucien/Documents/ColtopGIS/trunk-vroomgis
BINDIR=/home/lucien/Documents/ColtopGIS/bin/vroomloader

VARLINE="----------------------------------"


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

cmake $TRUNKDIR/app/vroomloader/build -G "CodeBlocks - Unix Makefiles" -DSEARCH_GDAL:BOOL=1 -DSEARCH_GEOS:BOOL=1 -DSEARCH_GIS_LIB_PATH:PATH="/home/lucien/Documents/LIB/_LIBGIS" -DCMAKE_WXWINDOWS_WXCONFIG_EXECUTABLE:FILE="/home/lucien/Documents/LIB/_LIBWXSVN/bin/wx-config"

echo $VARLINE
echo "4) Making the binary"
make
echo "4) Making the binary ... DONE"

echo $VARLINE
echo "5) Running..-"
./vroomLoader 
echo "5) vroomloader LAUNCHED"



