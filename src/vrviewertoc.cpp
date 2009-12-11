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

vrViewerTOC::vrViewerTOC(wxWindow * parent, wxWindowID id, const wxPoint & pos,
						 const wxSize & size, long style):
wxDataViewTreeCtrl(parent, id, pos, size, style){	
	m_ParentItem = AppendContainer(wxDataViewItem(NULL),"Table of content");
}

vrViewerTOC::~vrViewerTOC() {
}

bool vrViewerTOC::Add(int index, vrRenderer * renderer, int control) {
	AppendItem(m_ParentItem, renderer->GetLayer()->GetName().GetFullName());
	
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

