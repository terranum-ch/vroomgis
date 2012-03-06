#! /bin/sh
# Script for updating wxWidgets from SV
# (c) Lucien Schreiber CREALP 2010

# 1) Checkout the latest svn trunk
# 2) run configure 
# 3) run makefile
# 4) run make install 

#PARAMETERS
VARBINDIR="/Users/lucien/Documents/PROGRAMMATION/64/_LIBWXSVN"
VARTRUNKDIR="/Users/lucien/Documents/PROGRAMMATION/64/wxWidgets-SVN"
VARLINE="----------------------------------"

#title
echo $VARLINE
echo "   Creating wxWidgets SVN library"
echo "  (c) Lucien Schreiber CREALP"
echo $VARLINE

echo $VARLINE
echo "removing old installed / configurations files"
rm -rvf $VARBINDIR
mkdir $VARBINDIR

rm -rvf $VARTRUNKDIR/build64
mkdir $VARTRUNKDIR/build64
echo "removing old installed / configurations files ... DONE"
echo $VARLINE


#updating svn
echo "1) Getting last SVN version..."
cd $VARTRUNKDIR
svn update
VARSVNNUMBER=`svnversion -n`
echo '1) Getting last SVN version... DONE (\c'
echo  $VARSVNNUMBER ')'	
echo $VARLINE

#running configure
echo "2) Configuring library..."
cd $VARTRUNKDIR/build64/
../configure --prefix=$VARBINDIR --enable-unicode --disable-monolithic --disable-shared  --with-osx_cocoa --with-macosx-sdk=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.6.sdk --with-macosx-version-min=10.6 --without-libtiff --enable-mediactrl=no --enable-sound=no
echo "2) Configuring library.. DONE"
echo $VARLINE

#compiling project
echo "3)Building library..."
make --jobs=4
echo "3)Building library... DONE"
echo $VARLINE

#copying file for installer
echo "4) Installing library"
make install
echo "4) Installing library... DONE"

