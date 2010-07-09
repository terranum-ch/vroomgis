#! /bin/sh
# Script for updating wxWidgets from SVN
# (c) Lucien Schreiber CREALP 2010

# 1) Checkout the latest svn trunk
# 2) run configure 
# 3) run makefile
# 4) run make install 

#PARAMETERS
VARBINDIR="/Users/lucien/Documents/PROGRAMMATION/64/_LIBWXSVN_DBG"
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

rm -rvf $VARTRUNKDIR/build64-dbg
mkdir $VARTRUNKDIR/build64-dbg
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
cd $VARTRUNKDIR/build64-dbg/
../configure --prefix=$VARBINDIR --enable-unicode --disable-monolithic --disable-shared --enable-debug  --with-osx_cocoa --with-macosx-version-min=10.5
echo "2) Configuring library.. DONE"
echo $VARLINE

#compiling project
echo "3)Building library..."
make
echo "3)Building library... DONE"
echo $VARLINE

#copying file for installer
echo "4) Installing library"
make install
echo "4) Installing library... DONE"

