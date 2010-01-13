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
#include "vrviewertoc.h"
#include "vrviewerdisplay.h"




//if toc is null, we use the default vrViewerTOC. if  viewer is null, we use the default vrViewer.
vrViewerLayerManager::vrViewerLayerManager(vrLayerManager * parent,
										   vrViewerDisplay * viewer,vrViewerTOC * toc) {
	
	//wxASSERT(viewer);
	wxASSERT(parent);
	m_Display = viewer;
	m_Toc = toc;
	m_LayerManager = parent;
	
	if (m_Display==NULL){
		wxLogError("No display attached");
	}
	
	if (m_Toc == NULL){
		wxLogError("No toc defined");
	}

}



vrViewerLayerManager::~vrViewerLayerManager() {
	wxLogMessage("ViewerLayerManager Dtor called");
	if(m_Display){
		delete m_Display;
		m_Display = NULL;
	}
	
	if (m_Toc) {
		delete m_Toc;
		m_Toc = NULL;
	}
	
	// delete renderers
	unsigned int iCount = m_Renderers.GetCount();
	for (unsigned int i = 0; i<iCount; i++) {
		vrRenderer * myRenderer = m_Renderers.Item(0);
		wxASSERT(myRenderer);
		m_Renderers.Detach(0);
		delete myRenderer;
	}
	
}



bool vrViewerLayerManager::Add(long pos, vrLayer * layer, vrRender * render, vrLabel * label) {
	wxASSERT(layer);
	vrRenderer * myRenderer = new vrRenderer(layer, render, label);
	
	if (pos == -1){
		m_Renderers.Add(myRenderer);
	}
	else {
		m_Renderers.Insert(myRenderer, pos);
	}
	
	if (m_Toc) {
		m_Toc->Add(pos, myRenderer, m_Renderers.GetCount());
	}
	
	return true;
}




bool vrViewerLayerManager::Remove(const wxFileName & filename) {
	return false;
}



vrRenderer * vrViewerLayerManager::GetRenderer(const unsigned int & index) {
	// check index into limits
	if (index > m_Renderers.GetCount()-1) {
		wxLogError("Trying to Get Renderer out of bounds");
		return NULL;
	}
	
	vrRenderer * renderer = m_Renderers.Item(index);
	wxASSERT(renderer);
	return renderer;
}


void vrViewerLayerManager::FreezeBegin() {
}

void vrViewerLayerManager::FreezeEnd() {
}

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(vrArrayViewerLayerManager);




