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
#include "vrrubberband.h"
#include "vrevent.h"


/***************************************************************************//**
@brief Used for passing message from tool to application
@author Lucien Schreiber (c) CREALP 2010
@date 20 avril 2010
*******************************************************************************/
void vrDisplayToolMessage::_InitMembers() {
	m_EvtType = wxEVT_NULL;
	m_Rect = wxRect();
	wxASSERT(m_Rect.IsEmpty());
	m_Position = wxDefaultPosition;
}

vrDisplayToolMessage::vrDisplayToolMessage(const wxEventType & eventtype, const wxRect & rect) {
	_InitMembers();
	m_EvtType = eventtype;
	m_Rect = rect;
}

vrDisplayToolMessage::vrDisplayToolMessage(const wxEventType & eventtype, const wxPoint & pos) {
	_InitMembers();
	m_EvtType = eventtype;
	m_Position = pos;
	
}

vrDisplayToolMessage::~vrDisplayToolMessage() {
}







/***************************************************************************//**
@brief Base tool class
@author Lucien Schreiber (c) CREALP 2010
@date 20 avril 2010
*******************************************************************************/
vrDisplayTool::vrDisplayTool() {
	m_ID = wxNOT_FOUND;
	m_Name = wxEmptyString;
	m_Rubber = NULL;
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
	m_Rubber = NULL;
}



vrDisplayTool::~vrDisplayTool() {
	wxDELETE(m_Rubber);
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



/***************************************************************************//**
@brief Send a message to the parent
@param void 
@param vrDisplayTool::SendMessage 
@param vrDisplayToolMessage 
@param message valid pointer, will be sent to parent throught SetClientData.
 Should be deleted when intercepting event
@author Lucien Schreiber (c) CREALP 2010
@date 20 avril 2010
 *******************************************************************************/
void vrDisplayTool::SendMessage(vrDisplayToolMessage * message) {
	wxASSERT(m_Display);
	wxASSERT(message);
	wxASSERT(message->m_EvtType != wxEVT_NULL);
	
	wxCommandEvent myEvt (message->m_EvtType);
	myEvt.SetClientData(message);
	
	// send message to the top level window if found
	wxWindow * myTopWin = wxGetTopLevelParent(m_Display);
	if (myTopWin != NULL) {
		myTopWin->ProcessWindowEvent(myEvt);
		return;
	}
	
	// send message to the display if not found
	m_Display->ProcessWindowEvent(myEvt);
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
	wxASSERT(m_Rubber == NULL);
	m_Rubber = new vrRubberBand(GetDisplay());
	wxASSERT(m_Rubber);
	m_Rubber->SetPointFirst(event.GetPosition());
	return false;
}



bool vrDisplayToolZoom::MouseUp(const wxMouseEvent & event) {
	m_Rubber->SetPointLast(event.GetPosition());
	if (m_Rubber->IsValid()==false) {
		wxDELETE(m_Rubber);
		return false;
	}
	
	wxRect myRect = m_Rubber->GetRect();
	wxDELETE(m_Rubber);
	
	wxLogMessage("Selection rect is %d, %d, %d, %d",
				 myRect.GetLeft(),
				 myRect.GetTop(),
				 myRect.GetWidth(),
				 myRect.GetHeight());
	
	vrDisplayToolMessage * myMessage = new vrDisplayToolMessage(vrEVT_TOOL_ZOOM,
																myRect);
	wxASSERT(myMessage);
	SendMessage(myMessage);
	return true;
}



bool vrDisplayToolZoom::MouseMove(const wxMouseEvent & event) {

	if (event.Dragging()==true) {
		wxASSERT(m_Rubber);
		m_Rubber->SetPointLast(event.GetPosition());
		m_Rubber->Update();
	}
	return false;
}

