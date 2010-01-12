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
	
	 wxVector<wxVariant> data;
	data.push_back(false);
	data.push_back(renderer->GetLayer()->GetName().GetFullName());
	
	
	// prepending item to the list
	if (index == -1) {
		
		PrependItem(data);
		
		//PrependItem(m_ParentItem, renderer->GetLayer()->GetName().GetFullName());
	}
	
	// inserting item
	// TODO: is really usefull to add item not at the begining ?
	
	
	if(IsFrozen()==false){
		if(IsExpanded(m_ParentItem)==false){
			Expand(m_ParentItem);
		}
	}
	return false;
}



bool vrViewerTOC::Remove(int index) {
	return false;
}

void vrViewerTOC::FreezeBegin() {
}

void vrViewerTOC::FreezeEnd() {
}

