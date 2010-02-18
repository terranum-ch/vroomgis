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



vrLayerManager::vrLayerManager() {
	// init all formats from library
	OGRRegisterAll();
	GDALAllRegister();
}



vrLayerManager::~vrLayerManager() {
	
	// manually clearing array of layers, Clear() or Empty() didn't work
	unsigned int iCount = m_Layers.GetCount();
	for (unsigned int i = 0; i<iCount; i++)
	{
		vrLayer * myLayer = m_Layers.Item(0);
		m_Layers.Detach(0);
		wxDELETE(myLayer);
	}
	wxASSERT(m_Layers.GetCount()==0);
	
	// manually clearing array of viewermanagers
	unsigned int iCountVM = m_ViewerManagers.GetCount();
	for (unsigned j = 0; j<iCountVM; j++){
		vrViewerLayerManager * myManager = m_ViewerManagers.Item(0);
		m_ViewerManagers.Detach(0);
		wxDELETE(myManager);
	}
	wxASSERT(m_ViewerManagers.GetCount()==0);
}



bool vrLayerManager::Open(const wxFileName & filename) {
	
	if (filename.IsOk()==false){
		wxLogError("Filename not initialised");
		return false;
	}
	
	// search if layer allready opened
	if (GetLayer(filename) != NULL){
		wxLogMessage("Data %s allready opened", filename.GetFullName());
		return true;
	}
	
	vrDrivers myDriver;
	vrLayer * myLayer = NULL;
	bool bReadWrite = false;
	
	switch (myDriver.GetType(filename.GetExt())){
		case vrDRIVER_VECTOR_SHP:
			myLayer = new vrLayerVectorOGR();
			break;
		
		case vrDRIVER_RASTER_ESRIGRID:
		case vrDRIVER_RASTER_JPEG:
		case vrDRIVER_RASTER_TIFF:
			myLayer = new vrLayerRasterGDAL();
			break;

			
			
		default:
			wxLogError("Extension \"%s\" not supported",filename.GetExt());
			return false;
			break;
	}
	
	wxASSERT(myLayer);
	if (myLayer->Open(filename, bReadWrite)==false)
		return false;
	
	m_Layers.Add(myLayer);
	wxLogMessage("%d layers in the layermanager", m_Layers.GetCount());
	bool bValue = m_Layers.Item(0)->IsOK();
	wxLogMessage("added layer is %d", bValue);
	
	return true;
}


int vrLayerManager::GetCount() {
	return m_Layers.GetCount();
}



vrLayer * vrLayerManager::GetLayer(const wxFileName & filename){
	vrLayer * myLayer = NULL;
	
	for (unsigned int i = 0; i<m_Layers.GetCount(); i++)
	{
		if (m_Layers.Item(i)->GetName().SameAs(filename)){
			myLayer = m_Layers.Item(i);
			break;
		}
	}
	return myLayer;
}



bool vrLayerManager::AddViewerLayerManager(vrViewerLayerManager * manager){	
	wxASSERT(manager);
	
	for (unsigned int i = 0; i< m_ViewerManagers.GetCount(); i++){
		if (m_ViewerManagers.Item(i)==manager){
			wxLogError("This manager was allready added into the vrLayerManager");
			return false;
		}
	}
	
	
	m_ViewerManagers.Add(manager);
	return true;
}


