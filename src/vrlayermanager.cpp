/***************************************************************************
								vrlayermanager.cpp
				Manage the layers. Keep a list of all opened layers
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


#include "vrlayermanager.h"



bool vrLayerManager::Open(const wxFileName & filename) {
	
	if (filename.IsOk()==false){
		wxLogError("Filename not initialised");
		return false;
	}
	
	vrDrivers myDriver;
	switch (myDriver.GetType(filename.GetExt())){
		case vrDRIVER_VECTOR_SHP:
			break;
			
		default:
			wxLogError("Extension \"%s\" not supported",filename.GetExt());
			return false;
			break;
	}
	

	return true;
}