/***************************************************************************
								vrdriver.h
				List of supported drivers and misc function for them
                             -------------------
    copyright            : (C) 2009 CREALP Lucien Schreiber 
    email                : lucien.schreiber at crealp dot vs dot ch
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _VRDRIVER_H
#define _VRDRIVER_H


#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif


enum vrDRIVERS_TYPE {
	vrDRIVER_UNKNOWN = -1,
	vrDRIVER_VECTOR_SHP = 0,
	vrDRIVER_VECTOR_C2P,
	vrDRIVER_VECTOR_MEMORY,
	vrDRIVER_RASTER_TIFF,
	vrDRIVER_RASTER_JPEG,
	vrDRIVER_RASTER_ESRIGRID,
	vrDRIVER_RASTER_C2D,
    vrDRIVER_RASTER_EASC,
    vrDRIVER_RASTER_SGRD7
};


static const wxString vrDRIVERS_EXTENSION []= {
	"*.shp",
	"*.c2p",
	wxEmptyString,
	"*.tif;*.tiff",
	"*.jpg;*.jpeg",
	"*.adf",
	"*.c2d",
    "*.asc",
    "*.grd"
};


const wxString vrDRIVERS_GDAL_NAMES [] = {
    "ESRI Shapefile",
    "SQLite",
    "Memory",
    "GTiff",
    "JPEG",
    "AIG",
    "C2D",
    "AAIGrid",
    "GS7BG"
};

const wxString vrDRIVERS_NAMES [] = {
	"Shapefiles",
	"ColtopGIS project",
	wxEmptyString,
	"Tiff",
	"JPEG",
	"ESRI's binary GRID",
	"ColtopGIS",
    "ESRI ASCII grid",
    "Surfer 7 Binary Grid"
};


class vrDrivers {
  public:
    wxString GetWildcards();
	wxString GetSpecificWildcards(const wxArrayInt & types);
	vrDRIVERS_TYPE GetType(const wxString & extension);
    bool IsSupported(const wxString & extension);

};


#endif
