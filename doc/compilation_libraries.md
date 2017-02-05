These are the instructions to compile the libraries needed by vroomGiS.

# wxWidgets

## OSX

`../configure --prefix=<path_to>/_LIBWX --enable-unicode --disable-shared --enable-mediactrl=no --with-macosx-version-min=10.8`

> Note: Last sentence is very important on OSX 10.11 (El Capitain) Otherwise we will have lots of linking errors


# GEOS

## OSX

    mkdir build
    cd build
    ../configure --prefix=<path_to>/LIBGIS
    make -j8; make install

# PROJ4

## OSX

Use the same simple option as GEOS.

# GDAL

## OSX

    ./configure
    --prefix=~/LIB/_LIBGIS
    --with-geos=~/LIB/_LIBGIS/bin/geos-config
    --with-static-proj4=~/LIB/_LIBGIS
    --with-sqlite3=yes
    --with-python=no
    --with-pg=no
    --with-grass=no
    --with-jasper=no
    --with-jpeg=internal
    --with-png=interna

# Building sample applications

Use the following command line to create the makefile

    cmake /Users/lucien/DATA/PRJ/COLTOP-GIS/trunk_vroomgis/app/vroomloader/build 
    -DSEARCH_VROOMGIS_LIBS:BOOL=1 
    -DSEARCH_GIS_LIB_PATH:PATH=/Users/lucien/DATA/PROGRAMATION/_LIB/_LIBGIS

Then run `make -j8` to create the binary

