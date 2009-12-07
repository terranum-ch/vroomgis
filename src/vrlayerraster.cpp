/***************************************************************************
				vrlayerraster.cpp
                    
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


#include "vrlayerraster.h"

vrLayerRaster::vrLayerRaster() {
	m_Dataset = NULL;
}

vrLayerRaster::~vrLayerRaster() {
}


bool vrLayerRaster::IsOK() {
	if (m_Dataset==NULL){
		wxLogError("Dataset not inited");
		return false;
	}
	return true;
}







bool vrLayerRasterGDAL::_Close() {
	if (m_Dataset==NULL){
		return false;
	}
	
	wxLogMessage("Closing raster data NOW");
	GDALClose(m_Dataset);
	m_Dataset = NULL;
	return true;
}



vrLayerRasterGDAL::vrLayerRasterGDAL() {
}



vrLayerRasterGDAL::~vrLayerRasterGDAL() {
	_Close();
}



bool vrLayerRasterGDAL::Create(const wxFileName & filename) {
	return false;
}



bool vrLayerRasterGDAL::Open(const wxFileName & filename, bool readwrite) {
	// try to close
	_Close();
	wxASSERT(m_Dataset == NULL);
	
	// init filename and type
	m_FileName = filename;
	vrDrivers myDriver;
	m_DriverType = myDriver.GetType(filename.GetExt());
	
	GDALAccess myAcces = GA_ReadOnly;
	if (readwrite)
		myAcces = GA_Update;
	
	m_Dataset = (GDALDataset *) GDALOpen( filename.GetFullPath(), GA_ReadOnly );
	if(m_Dataset == NULL){
		wxLogError("Unable to open %s, maybe driver not regristred -  GDALAllRegister()", filename.GetFullName());
		return false;
	}
	
	return true;
}
