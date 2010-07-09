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
	
	// return all supported extensions
	myWildcards.Append("All supported formats|");
	for (unsigned int j = 0 ; j< sizeof(vrDRIVERS_EXTENSION) / sizeof(wxString); j++) {
		if (vrDRIVERS_NAMES[j] != wxEmptyString) {
			myWildcards.Append(vrDRIVERS_EXTENSION[j] + ";");
		}
	}
	myWildcards.RemoveLast(1);
	myWildcards.Append("|");
	
	
	for (unsigned int i = 0; i< sizeof(vrDRIVERS_EXTENSION) / sizeof(wxString); i++)
	{
		if (vrDRIVERS_NAMES[i] != wxEmptyString)
			myWildcards.Append(vrDRIVERS_NAMES[i] + " files ("  + vrDRIVERS_EXTENSION[i] + ")|"
							   + vrDRIVERS_EXTENSION[i] + "|");
	}
	myWildcards.RemoveLast(1);
	return myWildcards;
}



vrDRIVERS_TYPE vrDrivers::GetType(const wxString & extension) {
	
	if (extension.IsEmpty()){
		wxLogError("Empty extension not supported");
		return vrDRIVER_UNKNOWN;
	}
	
	if (extension.Len() < 3){
		wxLogError("Only %d character in the extension (need 3 character)", extension.Len());
		return vrDRIVER_UNKNOWN;
	}
	
	
	for (unsigned int i = 0; i<sizeof(vrDRIVERS_EXTENSION) / sizeof(wxString); i++){
		
		if (vrDRIVERS_EXTENSION[i].IsEmpty()==false){
			if (vrDRIVERS_EXTENSION[i].Find(extension.Lower())!=wxNOT_FOUND){
				return (vrDRIVERS_TYPE)i;
			}
		}
	}
	return vrDRIVER_UNKNOWN;
}



bool vrDrivers::IsSupported(const wxString & extension) {
	if (GetType(extension)==vrDRIVER_UNKNOWN) {
		return false;
	}
	
	return true;
}


