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


bool vrLayerRasterGDAL::GetExtent(wxRect2DDouble & rect) {
	
	if (m_Dataset == NULL) {
		wxLogError("No layer opened");
		return false;
	}
	
	wxASSERT(m_Dataset);
	double        adfGeoTransform[6];
	if( m_Dataset->GetGeoTransform( adfGeoTransform ) != CE_None ){
		wxLogError("Error getting geometric informations for layer : %s",
				   m_FileName.GetFullName());
		return false;
	}
	
	if (adfGeoTransform[5] > 0) {
		wxLogError("vertical pixel size is Wrong (should be negative) : %.2f",
				   adfGeoTransform[5]);
	}
	rect.SetLeft(adfGeoTransform[0]);
	rect.SetTop(adfGeoTransform[3]);
	
	rect.m_height = adfGeoTransform[5]*m_Dataset->GetRasterYSize();	
	rect.m_width = adfGeoTransform[1]*m_Dataset->GetRasterXSize();
	

	
	
	
	if (adfGeoTransform[2] != 0 || adfGeoTransform[4] != 0) {
		wxLogError("Layer %s seams to contain rotation informations (%.2f, %.2f) will not be displayed correctly",
				   m_FileName.GetFullName(),
				   adfGeoTransform[2],
				   adfGeoTransform[4]);
	}

	return true;
	
}

