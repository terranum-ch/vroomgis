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
#include "vrlayervector.h"
#include "vrlayerrasterc2d.h"
#include "vrlayervectorc2p.h"
#include "vrlayerraster.h"



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
		wxDELETE(myLayer);
		m_Layers.RemoveAt(0);
	}
	wxASSERT(m_Layers.GetCount()==0);

	// manually clearing array of viewermanagers
	int iCountVM = m_ViewerManagers.GetCount();
	for (int j = iCountVM - 1; j >= 0; j--){
		vrViewerLayerManager * myManager = m_ViewerManagers.Item(0);
		wxDELETE(myManager);
		m_ViewerManagers.RemoveAt(0);
	}
	wxASSERT(m_ViewerManagers.GetCount()==0);
}



bool vrLayerManager::Open(const wxFileName & filename, bool readwrite) {

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

	switch (myDriver.GetType(filename.GetExt())){
		case vrDRIVER_VECTOR_SHP:
			myLayer = new vrLayerVectorOGR();
			break;

		case vrDRIVER_RASTER_ESRIGRID:
			myLayer = new vrLayerRasterEGRID();
			break;

		case vrDRIVER_RASTER_JPEG:
		case vrDRIVER_RASTER_TIFF:
			myLayer = new vrLayerRasterGDAL();
			break;

		case vrDRIVER_RASTER_C2D:
			myLayer = new vrLayerRasterC2D();
			break;

		case vrDRIVER_VECTOR_C2P:
			myLayer = new vrLayerVectorC2P();
			break;

        case vrDRIVER_RASTER_EASC:
            myLayer = new vrLayerRasterGDAL();
            break;

        case vrDRIVER_RASTER_SGRD7:
            myLayer = new vrLayerRasterGDAL();
            break;

        case vrDRIVER_RASTER_WMS:
            myLayer = new vrLayerRasterGDAL();
            break;


		default:
			wxLogError("Extension \"%s\" not supported",filename.GetExt());
			return false;
			break;
	}

	wxASSERT(myLayer);
	if (myLayer->Open(filename, readwrite)==false){
		return false;
	}

	m_Layers.Add(myLayer);
	//7wxLogMessage("%ld layers in the layermanager", m_Layers.GetCount());
	//bool bValue = m_Layers.Item(0)->IsOK();
	//wxLogMessage("added layer is %d", bValue);
	return true;
}



bool vrLayerManager::Add(vrLayer * layer) {
	wxASSERT(layer);
	if (layer == NULL) {
		return false;
	}

	if (GetLayer(layer->GetFileName()) != NULL) {
		wxLogMessage(_("Layer with same name allready exists '%s'"),layer->GetFileName().GetFullName());
		return false;
	}

	m_Layers.Add(layer);
	return true;
}


// true if layer isn't any more used. false otherwise
bool vrLayerManager::Close(vrLayer * layer) {
	wxASSERT(layer);

	//check all viewerlayermanagers to ensure that this layer
	//isn't open anywhere.
	for (unsigned int i = 0; i<m_ViewerManagers.GetCount(); i++) {
		for (int j = 0; j<m_ViewerManagers.Item(i)->GetCount(); j++) {
			vrRenderer * myRenderer = m_ViewerManagers.Item(i)->GetRenderer(j);
			if (layer == myRenderer->GetLayer()) {
				wxLogError("Unable to close '%s', layer still in use",
						   layer->GetDisplayName().GetFullName());
				return false;
			}
		}
	}

	// Layer isn't anymore used we can close now
	int iRemoveIndex = wxNOT_FOUND;
	for (unsigned int i = 0; i< m_Layers.GetCount(); i++) {
		if (m_Layers.Item(i) == layer) {
			iRemoveIndex = i;
		}
	}

	if (iRemoveIndex == wxNOT_FOUND) {
		wxLogError("Unable to close '%s', layer not present into layermanager",
				   layer->GetDisplayName().GetFullName());
		return false;
	}

	vrLayer * myLayer = m_Layers.Item(iRemoveIndex);
	wxASSERT(myLayer);
	wxDELETE(myLayer);
	m_Layers.RemoveAt(iRemoveIndex);
	return true;
}


// return false if still in use
bool vrLayerManager::Erase(const wxFileName & filename) {
    if (GetLayer(filename) != NULL) {
        wxLogError(_("Unable to Delete: '%s'. This file is still used!"), filename.GetFullName());
        return false;
    }
    
    vrDrivers myDriver;
    vrDRIVERS_TYPE myDriverType = myDriver.GetType(filename.GetExt());
    wxString myDriverName = vrDRIVERS_GDAL_NAMES[myDriverType];

	switch (myDriverType){
		case vrDRIVER_VECTOR_SHP:
        {
            OGRSFDriver * poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName((const char *) myDriverName.mb_str(wxConvUTF8));
            wxASSERT(poDriver);
            if(poDriver->DeleteDataSource(filename.GetFullPath().mb_str(wxConvUTF8)) != OGRERR_NONE){
                wxLogError(_("Deleting '%s' Failed!"), filename.GetFullName());
                return false;
            }
        }
			break;
            
		case vrDRIVER_RASTER_ESRIGRID:
		case vrDRIVER_RASTER_JPEG:
		case vrDRIVER_RASTER_TIFF:
		case vrDRIVER_RASTER_C2D:
        case vrDRIVER_RASTER_EASC:
        case vrDRIVER_RASTER_SGRD7:
        case vrDRIVER_RASTER_WMS:
        {
            GDALDriver *myGDALDriver = GetGDALDriverManager()->GetDriverByName( myDriverName.mb_str(wxConvUTF8));
            wxASSERT(myGDALDriver);
            if(myGDALDriver->Delete(filename.GetFullPath().mb_str(wxConvUTF8)) != CE_None){
                wxLogError(_("Deleting '%s' Failed!"), filename.GetFullName());
                return false;
            }
        }
            break;
            
		case vrDRIVER_VECTOR_C2P:
            wxLogError(_("Unable to delete C2P file!"));
            return false;
			break;
              
		default:
			wxLogError("Extension \"%s\" not supported",filename.GetExt());
			return false;
			break;
	}
    return true;
}



int vrLayerManager::GetCount() {
	return m_Layers.GetCount();
}



vrLayer * vrLayerManager::GetLayer(const wxFileName & filename){
	vrLayer * myLayer = NULL;

	for (unsigned int i = 0; i<m_Layers.GetCount(); i++)
	{
		//wxLogMessage("'%s' filename, '%s' layername", filename.GetFullPath(), m_Layers.Item(i)->GetFileName().GetFullPath());
		if (m_Layers.Item(i)->GetFileName().SameAs(filename)){
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
			wxLogError("This manager was already added into the vrLayerManager");
			return false;
		}
	}

	m_ViewerManagers.Add(manager);
	return true;
}


bool vrLayerManager::RemoveViewerLayerManager (vrViewerLayerManager * manager){
    if (manager == NULL) {
        return false;
    }
    
    for (unsigned int i = 0; i< m_ViewerManagers.GetCount(); i++) {
        vrViewerLayerManager * myManager = m_ViewerManagers.Item(i);
		if (myManager == manager) {
            wxDELETE(myManager);
            m_ViewerManagers.RemoveAt(i);
            return true;
        }
    }
    return false;
}


