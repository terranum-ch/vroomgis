/***************************************************************************
 vrdisplaytool.cpp

 -------------------
 copyright            : (C) 2010 CREALP Lucien Schreiber 
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


#include "vrdisplaytool.h"

vrDisplayTool::vrDisplayTool() {
	m_ID = wxNOT_FOUND;
	m_Name = wxEmptyString;
}



vrDisplayTool::vrDisplayTool(int id, wxString name, wxCursor cursor) {
	Create(id, name, cursor);
}



void vrDisplayTool::Create(int id, wxString name, wxCursor cursor) {
	wxASSERT(id != wxNOT_FOUND);
	wxASSERT(name.IsEmpty() == false);
	
	m_ID = id;
	m_Name = name;
	m_Cursor = cursor;
}



vrDisplayTool::~vrDisplayTool() {
	
}


bool vrDisplayTool::MouseDown() {
	return false;
}

bool vrDisplayTool::MouseUp() {
	return false;
}

bool vrDisplayTool::MouseMove() {
	return false;
}


vrDisplayToolDefault::vrDisplayToolDefault() {
	Create(wxID_DEFAULT, "Select", wxCursor(wxCURSOR_ARROW));
}

vrDisplayToolDefault::~vrDisplayToolDefault() {
}

bool vrDisplayToolDefault::MouseDown() {
	return false;
}

bool vrDisplayToolDefault::MouseUp() {
	return false;
}

bool vrDisplayToolDefault::MouseMove() {
	return false;
}

vrDisplayToolZoom::vrDisplayToolZoom() {
	Create(wxID_ZOOM_100, "Zoom", wxCursor(wxCURSOR_MAGNIFIER));
}

vrDisplayToolZoom::~vrDisplayToolZoom() {
}

bool vrDisplayToolZoom::MouseDown() {
	return false;
}

bool vrDisplayToolZoom::MouseUp() {
	return false;
}

bool vrDisplayToolZoom::MouseMove() {
	return false;
}

