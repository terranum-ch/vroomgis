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
#include "vrrenderer.h"
#include "vrlayer.h"


BEGIN_EVENT_TABLE(vrViewerTOC, wxDataViewListCtrl)
	EVT_DATAVIEW_ITEM_VALUE_CHANGED(wxID_ANY, vrViewerTOC::OnVisibleStatusChanged)
END_EVENT_TABLE()

void vrViewerTOC::OnVisibleStatusChanged(wxDataViewEvent & event) {
	wxASSERT(event.GetColumn() == 0);
	
	const wxDataViewListStore * myModel = GetStore();
	int myRow = myModel->GetRow(event.GetItem());
	
	wxVariant myCheck;
	GetValue(myCheck, myRow, 0);
	
	wxLogMessage("Value is %d - row = %d", myCheck.GetBool(), myRow);
	
}




vrViewerTOC::vrViewerTOC(wxWindow * parent, wxWindowID id, const wxPoint & pos,
						 const wxSize & size, long style):
wxDataViewListCtrl(parent, id, pos, size, style){	
	AppendToggleColumn("Visible", wxDATAVIEW_CELL_INERT, 50, 	
					   wxALIGN_CENTER);
	wxSize iCtrlSize = GetSize();
	AppendTextColumn("Name", wxDATAVIEW_CELL_INERT, 200);
	//m_ParentItem = AppendContainer(wxDataViewItem(NULL),"Table of content");
	
}

vrViewerTOC::~vrViewerTOC() {
}



bool vrViewerTOC::Add(int index, vrRenderer * renderer, int control) {
	
	// TODO: is really usefull to add item not at the begining ?
	wxASSERT(index == -1);
	
	wxVector<wxVariant> data;
	data.push_back(renderer->GetVisible()); // is layer visible
	data.push_back(renderer->GetLayer()->GetName().GetFullName());
	
	
	// prepending item to the list
	if (index == -1) {
		
		PrependItem(data);
		
		//PrependItem(m_ParentItem, renderer->GetLayer()->GetName().GetFullName());
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
}

void vrViewerTOC::FreezeEnd() {
}

