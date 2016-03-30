/***************************************************************************
								vrdriver.cpp
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

#include "vrdriver.h"


wxString vrDrivers::GetWildcards(const vrDRIVERS_FLAG &flags)
{
    wxArrayInt myTypes;
    if (flags & vrDRIVERS_FLAG_GENERIC) {
        myTypes.Add(vrDRIVER_VECTOR_SHP);
        myTypes.Add(vrDRIVER_RASTER_TIFF);
        myTypes.Add(vrDRIVER_RASTER_JPEG);
        myTypes.Add(vrDRIVER_RASTER_ESRIGRID);
        myTypes.Add(vrDRIVER_RASTER_EASC);
        myTypes.Add(vrDRIVER_RASTER_SGRD7);
        myTypes.Add(vrDRIVER_RASTER_WMS);
    }

    if (flags & vrDRIVERS_FLAG_COLTOP) {
        //myTypes.Add(vrDRIVER_VECTOR_C2P);
        myTypes.Add(vrDRIVER_RASTER_C2D);
    }

    return GetSpecificWildcards(myTypes);
}


wxString vrDrivers::GetSpecificWildcards(const wxArrayInt &types)
{
    wxString myWildcards = wxEmptyString;

    if (types.GetCount() > 1) {
        myWildcards.Append(_("All supported formats|"));
        for (unsigned int j = 0; j < types.GetCount(); j++) {
            myWildcards.Append(vrDRIVERS_EXTENSION[types.Item(j)] + ";");
        }
        myWildcards.RemoveLast(1);
        myWildcards.Append("|");
    }

    for (unsigned int i = 0; i < types.GetCount(); i++) {
        myWildcards.Append(vrDRIVERS_NAMES[types.Item(i)] + " files (" + vrDRIVERS_EXTENSION[types.Item(i)] + ")|" +
                           vrDRIVERS_EXTENSION[types.Item(i)] + "|");
    }
    myWildcards.RemoveLast(1);
    return myWildcards;
}


vrDRIVERS_TYPE vrDrivers::GetType(const wxString &extension)
{
    wxString myExtension = extension;
    if (myExtension.IsEmpty()) {
        wxLogError("Empty extension not supported");
        return vrDRIVER_UNKNOWN;
    }

    if (myExtension.Len() < 3) {
        wxLogError("Only %d character in the extension (need 3 character)", (int) myExtension.Len());
        return vrDRIVER_UNKNOWN;
    }

    // truncate extension with space inside (used to pass layer number to open
    int mySpaceFound = myExtension.Find(_T(" "));
    if (mySpaceFound != wxNOT_FOUND) {
        myExtension = myExtension.Left(mySpaceFound);
    }

    for (unsigned int i = 0; i < sizeof(vrDRIVERS_EXTENSION) / sizeof(wxString); i++) {

        if (!vrDRIVERS_EXTENSION[i].IsEmpty()) {
            if (vrDRIVERS_EXTENSION[i].Find(myExtension.Lower()) != wxNOT_FOUND) {
                return (vrDRIVERS_TYPE) i;
            }
        }
    }
    return vrDRIVER_UNKNOWN;
}


wxString vrDrivers::GetWildcardsRaster()
{
    wxArrayInt myType;
    myType.Add(vrDRIVER_RASTER_TIFF);
    myType.Add(vrDRIVER_RASTER_JPEG);
    myType.Add(vrDRIVER_RASTER_ESRIGRID);
    myType.Add(vrDRIVER_RASTER_EASC);
    myType.Add(vrDRIVER_RASTER_SGRD7);
    myType.Add(vrDRIVER_RASTER_WMS);
    return GetSpecificWildcards(myType);
}


wxString vrDrivers::GetWildcardsVector()
{
    wxArrayInt myType;
    myType.Add(vrDRIVER_VECTOR_SHP);
    return GetSpecificWildcards(myType);
}


bool vrDrivers::IsSupported(const wxString &extension)
{
    return GetType(extension) != vrDRIVER_UNKNOWN;

}


