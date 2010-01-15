/***************************************************************************
				vrviewertoc.cpp
                    
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


#include "vrviewertoc.h"
#include "vrviewerlayermanager.h"
#include "vrlayer.h"


BEGIN_EVENT_TABLE(vrViewerTOC, wxCheckListBox)
	EVT_CHECKLISTBOX(wxID_ANY, vrViewerTOC::OnVisibleStatusChanged)
END_EVENT_TABLE()

void vrViewerTOC::OnVisibleStatusChanged(wxCommandEvent & event) {
	
	wxLogMessage("Value changed for item : %d value is %d", event.GetInt(), IsChecked(event.GetInt()));
	
	// change visibility status for layer
	wxASSERT(m_ViewerManager);
	vrRenderer * myItemRenderer = m_ViewerManager->GetRenderer(event.GetInt());
	wxASSERT(myItemRenderer);
	myItemRenderer->SetVisible(IsChecked(event.GetInt()));
	
	// ask for reloading data
	wxCommandEvent myEvt(vrEVT_VLM_RELOAD);
	ProcessWindowEvent(myEvt);
}




vrViewerTOC::vrViewerTOC(wxWindow * parent, wxWindowID id, const wxPoint & pos,
						 const wxSize & size):
wxCheckListBox(parent, id, pos, size){	

	m_FreezeStatus = false;
	
}

vrViewerTOC::~vrViewerTOC() {
}



bool vrViewerTOC::Add(int index, vrRenderer * renderer, int control) {
	
	// TODO: is really usefull to add item not at the begining ?
	wxASSERT(index == -1);
	
	
	
	// prepending item to the list
	if (index == -1) {
		
		Insert(renderer->GetLayer()->GetName().GetFullName(), 0);
		Check(0, renderer->GetVisible());
		
	}
	
	// inserting item
	/*if(IsFrozen()==false){
		if(IsExpanded(m_ParentItem)==false){
			Expand(m_ParentItem);
		}
	}*/
	
	return false;
}



bool vrViewerTOC::Remove(int index) {
	return false;
}

void vrViewerTOC::FreezeBegin() {
	wxASSERT(m_FreezeStatus==false);
	m_FreezeStatus = true;
	Freeze();
}

void vrViewerTOC::FreezeEnd() {
	wxASSERT(m_FreezeStatus == true);
	m_FreezeStatus = false;
	Thaw();
}

void vrViewerTOC::SetViewerLayerManager(vrViewerLayerManager * value) {
	wxASSERT(value);
	m_ViewerManager = value;
}



