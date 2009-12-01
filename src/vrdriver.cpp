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


wxString vrDrivers::GetWildcards() 
{
	wxString myWildcards = wxEmptyString;
	for (unsigned i = 0; i< sizeof(vrDRIVERS_EXTENSION) / sizeof(wxString); i++)
	{
		if (vrDRIVERS_NAMES[i] != wxEmptyString)
			myWildcards.Append(vrDRIVERS_NAMES[i] + " files ("  + vrDRIVERS_EXTENSION[i] + ")|"
							   + vrDRIVERS_EXTENSION[i] + "|");
	}
	myWildcards.RemoveLast(1);
	return myWildcards;
}



wxArrayString vrDrivers::GetExtension() 
{
}

