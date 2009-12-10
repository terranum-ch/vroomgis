/***************************************************************************
				vrviewerlayermanager.cpp
                    
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


#include "vrviewerlayermanager.h"
#include "vrlayermanager.h"
#include "vrlayer.h"



//if toc is null, we use the default vrViewerTOC. if  viewer is null, we use the default vrViewer.
vrViewerLayerManager::vrViewerLayerManager(vrLayerManager * parent, vrViewerDisplay * viewer){//, vrViewerTOC * toc) {
	
	//wxASSERT(viewer);
	wxASSERT(parent);
	m_Display = viewer;
	m_LayerManager = parent;
	
	if (m_Display==NULL){
		wxLogError("No display attached");
	}

}



vrViewerLayerManager::~vrViewerLayerManager() {
	wxLogMessage("ViewerLayerManager Dtor called");
	if(m_Display)
		delete m_Display;
	
}



bool vrViewerLayerManager::Add(long pos, vrLayer * layer){//, vrRender * render, vrLabel * label) {
	return false;
}

bool vrViewerLayerManager::Remove(const wxFileName & filename) {
	return false;
}

void vrViewerLayerManager::FreezeBegin() {
}

void vrViewerLayerManager::FreezeEnd() {
}

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(vrArrayViewerLayerManager);







vrViewerDisplay::vrViewerDisplay(wxWindow * parent, wxWindowID id, long style) :
wxScrolledCanvas(parent, id, wxDefaultPosition, wxDefaultSize, style){
}

vrViewerDisplay::~vrViewerDisplay() {
}

void vrViewerDisplay::Refresh() {
}

