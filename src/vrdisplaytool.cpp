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
#include "vrviewerdisplay.h"

vrDisplayTool::vrDisplayTool() {
	m_ID = wxNOT_FOUND;
	m_Name = wxEmptyString;
}



vrDisplayTool::vrDisplayTool(vrViewerDisplay * display, int id, wxString name, wxCursor cursor) {
	Create(display, id, name, cursor);
}



void vrDisplayTool::Create(vrViewerDisplay * display, int id, wxString name, wxCursor cursor) {
	wxASSERT(id != wxNOT_FOUND);
	wxASSERT(name.IsEmpty() == false);
	wxASSERT(display != NULL);
	
	m_ID = id;
	m_Name = name;
	m_Cursor = cursor;
	m_Display = display;
}



vrDisplayTool::~vrDisplayTool() {
	
}


bool vrDisplayTool::MouseDown(const wxMouseEvent & event) {
	// don't use this class directly but use the child instead
	wxFAIL;
	return false;
}

bool vrDisplayTool::MouseUp(const wxMouseEvent & event) {
	// don't use this class directly but use the child instead
	wxFAIL;
	return false;
}

bool vrDisplayTool::MouseMove(const wxMouseEvent & event) {
	// don't use this class directly but use the child instead
	wxFAIL;
	return false;
}


vrDisplayToolDefault::vrDisplayToolDefault(vrViewerDisplay * display) {
	Create(display, wxID_DEFAULT, "Select", wxCursor(wxCURSOR_ARROW));
}

vrDisplayToolDefault::~vrDisplayToolDefault() {
}

bool vrDisplayToolDefault::MouseDown(const wxMouseEvent & event) {
	wxLogMessage("Select down");
	return false;
}

bool vrDisplayToolDefault::MouseUp(const wxMouseEvent & event) {
	wxLogMessage("Select up");	
	return false;
}

bool vrDisplayToolDefault::MouseMove(const wxMouseEvent & event) {
	if (event.Dragging()==true) {
		wxLogMessage("Dragging with selection");
	}
	return false;
}




vrDisplayToolZoom::vrDisplayToolZoom(vrViewerDisplay * display) {
	Create(display, wxID_ZOOM_100, "Zoom", wxCursor(wxCURSOR_MAGNIFIER));
}

vrDisplayToolZoom::~vrDisplayToolZoom() {
}

bool vrDisplayToolZoom::MouseDown(const wxMouseEvent & event) {
	wxLogMessage("Zoom down");
	return false;
}

bool vrDisplayToolZoom::MouseUp(const wxMouseEvent & event) {
	wxLogMessage("Zoom up");
	return false;
}

bool vrDisplayToolZoom::MouseMove(const wxMouseEvent & event) {
	if (event.Dragging()==true) {
		wxLogMessage("Zoom dragged");
	}
	return false;
}

