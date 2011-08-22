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
#include "../art/vroomgis_bmp.h"


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
	m_ParentManager = NULL;
}

vrDisplayToolMessage::vrDisplayToolMessage(const wxEventType & eventtype,
										   vrViewerDisplay * parent, const wxRect & rect) {
	_InitMembers();
	m_EvtType = eventtype;
	m_Rect = rect;
	m_ParentManager = parent->GetViewerLayerManager();
}

vrDisplayToolMessage::vrDisplayToolMessage(const wxEventType & eventtype,
										   vrViewerDisplay * parent, const wxPoint & pos) {
	_InitMembers();
	m_EvtType = eventtype;
	m_Position = pos;
	m_ParentManager = parent->GetViewerLayerManager();

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


bool vrDisplayTool::MouseDClickLeft(const wxMouseEvent & event) {
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

	vrDisplayToolMessage * myMsG = new vrDisplayToolMessage(vrEVT_TOOL_SELECT,
															GetDisplay(),
															event.GetPosition());
	SendMessage(myMsG);
	return true;
}

bool vrDisplayToolDefault::MouseUp(const wxMouseEvent & event) {

	return true;
}

bool vrDisplayToolDefault::MouseMove(const wxMouseEvent & event) {
	if (event.Dragging()==true) {
		//wxLogMessage("Dragging with selection");
	}
	return true;
}




vrDisplayToolSelect::vrDisplayToolSelect(vrViewerDisplay * display) {
	Create(display, wxID_DEFAULT, "Select", wxCursor(wxCURSOR_ARROW));
}



vrDisplayToolSelect::~vrDisplayToolSelect() {
}


bool vrDisplayToolSelect::MouseDown(const wxMouseEvent & event) {
	wxASSERT(m_Rubber == NULL);
	m_Rubber = new vrRubberBand(GetDisplay());
	wxASSERT(m_Rubber);
	m_Rubber->SetPointFirst(event.GetPosition());
	return false;
}



bool vrDisplayToolSelect::MouseUp(const wxMouseEvent & event) {
	if (m_Rubber == NULL) {
		return true;
	}

	m_Rubber->SetPointLast(event.GetPosition());

	wxRect myRect;
	myRect.SetLeftTop(event.GetPosition());
	myRect.SetRightBottom(event.GetPosition());
	if (m_Rubber->IsValid()==true) {
		myRect = m_Rubber->GetRect();
	}
	wxDELETE(m_Rubber);

	/*wxLogMessage("Selection rect is %d, %d, %d, %d",
				 myRect.GetLeft(),
				 myRect.GetTop(),
				 myRect.GetWidth(),
				 myRect.GetHeight());*/

	vrDisplayToolMessage * myMessage = new vrDisplayToolMessage(vrEVT_TOOL_SELECT,
																GetDisplay(),
																myRect);
	wxASSERT(myMessage);
	SendMessage(myMessage);
	return true;

}

bool vrDisplayToolSelect::MouseMove(const wxMouseEvent & event) {
	if (event.Dragging()==true && m_Rubber != NULL) {
		m_Rubber->SetPointLast(event.GetPosition());
		m_Rubber->Update();
	}
	return false;
}




vrDisplayToolZoom::vrDisplayToolZoom(vrViewerDisplay * display) {
	Create(display, wxID_ZOOM_IN, "Zoom", wxCursor(_img_cursor_zoomin->ConvertToImage()));
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

    wxRect myRect;

	if (!m_Rubber) {
        return false;
    }

    m_Rubber->SetPointLast(event.GetPosition());
	if (m_Rubber->IsValid()==false) { // e.g. if user only clicks
		wxDELETE(m_Rubber);

        // Set a rectangle centered with a size 1.5 times smaller
        myRect = wxRect(event.GetPosition().x - GetDisplay()->GetSize().GetWidth()/3,
                              event.GetPosition().y - GetDisplay()->GetSize().GetHeight()/3,
                              GetDisplay()->GetSize().GetWidth()/1.5,
                              GetDisplay()->GetSize().GetHeight()/1.5);
	}
    else {
        myRect = m_Rubber->GetRect();
        wxDELETE(m_Rubber);

        // Check that no dimension is too small
        if(myRect.GetWidth()<5) {
            myRect.SetWidth(5);
        }
        if(myRect.GetHeight()<5) {
            myRect.SetHeight(5);
        }

        /*wxLogMessage("Selection rect is %d, %d, %d, %d",
                     myRect.GetLeft(),
                     myRect.GetTop(),
                     myRect.GetWidth(),
                     myRect.GetHeight());*/
    }

	vrDisplayToolMessage * myMessage = new vrDisplayToolMessage(vrEVT_TOOL_ZOOM,
																GetDisplay(),
																myRect);
	wxASSERT(myMessage);
	SendMessage(myMessage);
	return true;
}



bool vrDisplayToolZoom::MouseMove(const wxMouseEvent & event) {
	if (!m_Rubber) {
        return false;
    }

	if (event.Dragging()==true) {
		m_Rubber->SetPointLast(event.GetPosition());
		m_Rubber->Update();
	}
	return false;
}


vrDisplayToolZoomOut::vrDisplayToolZoomOut(vrViewerDisplay * display)
:vrDisplayToolZoom(display) {
	Create(display, wxID_ZOOM_OUT, "Zoom out", wxCursor(_img_cursor_zoomout->ConvertToImage()));
}

vrDisplayToolZoomOut::~vrDisplayToolZoomOut() {

}


bool vrDisplayToolZoomOut::MouseUp(const wxMouseEvent & event) {

	wxRect myRect;

	if (!m_Rubber) {
        return false;
    }

    m_Rubber->SetPointLast(event.GetPosition());
    if (m_Rubber->IsValid()==false) { // e.g. if user only clicks
        wxDELETE(m_Rubber);

        // Set a rectangle centered with a size 1.5 times smaller
        myRect = wxRect(event.GetPosition().x - GetDisplay()->GetSize().GetWidth()/3,
                              event.GetPosition().y - GetDisplay()->GetSize().GetHeight()/3,
                              GetDisplay()->GetSize().GetWidth()/1.5,
                              GetDisplay()->GetSize().GetHeight()/1.5);
    }
    else {
        myRect = m_Rubber->GetRect();
        wxDELETE(m_Rubber);

        // Check that no dimension is too small
        if(myRect.GetWidth()<5) {
            myRect.SetWidth(5);
        }
        if(myRect.GetHeight()<5) {
            myRect.SetHeight(5);
        }

        /*wxLogMessage("Selection rect is %d, %d, %d, %d",
                     myRect.GetLeft(),
                     myRect.GetTop(),
                     myRect.GetWidth(),
                     myRect.GetHeight());*/
    }

	vrDisplayToolMessage * myMessage = new vrDisplayToolMessage(vrEVT_TOOL_ZOOMOUT,
																GetDisplay(),
																myRect);
	wxASSERT(myMessage);
	SendMessage(myMessage);
	return true;
}




/***************************************************************************//**
@brief Default pan tool
@author Lucien Schreiber (c) CREALP 2010
@date 28 avril 2010
 *******************************************************************************/
vrDisplayToolPan::vrDisplayToolPan(vrViewerDisplay * display) {
	Create(display, wxID_DEFAULT, "Pan", wxCursor(wxCURSOR_HAND));
	m_Point = wxDefaultPosition;
	m_PanBitmap = NULL;
}



vrDisplayToolPan::~vrDisplayToolPan() {
}



bool vrDisplayToolPan::MouseDown(const wxMouseEvent & event) {
	wxASSERT(m_Point == wxDefaultPosition);
	m_Point = event.GetPosition();

	wxASSERT(m_PanBitmap == NULL);

	wxClientDC myDc (GetDisplay());
	m_PanBitmap = new wxBitmap (myDc.GetSize());
	wxASSERT(m_PanBitmap);

	wxMemoryDC memdc;
	memdc.SelectObject(*m_PanBitmap);
	memdc.Blit(0,0,
			   m_PanBitmap->GetSize().GetWidth(),
			   m_PanBitmap->GetSize().GetHeight(),
			   &myDc,
			   0,0);
	memdc.SelectObject(wxNullBitmap);
	return true;
}



bool vrDisplayToolPan::MouseUp(const wxMouseEvent & event) {
    if (m_PanBitmap == NULL) {
        return false;
    }

	// compute the new raster origin
	wxPoint myNewPos(m_Point.x - event.GetPosition().x,
					 m_Point.y - event.GetPosition().y);


	vrDisplayToolMessage * myMessage = new vrDisplayToolMessage(vrEVT_TOOL_PAN,
																GetDisplay(),
																myNewPos);
	wxASSERT(myMessage);
	SendMessage(myMessage);

	wxDELETE(m_PanBitmap);
	m_Point = wxDefaultPosition;
	return true;
}



bool vrDisplayToolPan::MouseMove(const wxMouseEvent & event) {
	if (event.Dragging()==false || m_PanBitmap == NULL) {
		return false;
	}

	// compute the new raster origin
	wxPoint myNewPos(event.GetPosition().x - m_Point.x,
					 event.GetPosition().y - m_Point.y);

	if (myNewPos.x == 0 && myNewPos.y == 0) {
		return false;
	}

	wxMemoryDC memdc;
	wxBitmap * myMovedBmp = new wxBitmap(m_PanBitmap->GetSize());
	wxASSERT(myMovedBmp);
	memdc.SelectObject(*myMovedBmp);
	memdc.SetBrush(GetDisplay()->GetBackgroundColour());
	memdc.SetPen(GetDisplay()->GetBackgroundColour());
	memdc.DrawRectangle (0,0, m_PanBitmap->GetSize().x,m_PanBitmap->GetSize().y);
	memdc.DrawBitmap (*m_PanBitmap, myNewPos.x, myNewPos.y);
	memdc.SelectObject(wxNullBitmap);

	GetDisplay()->SetBitmap(myMovedBmp);
	wxDELETE(myMovedBmp);
	return true;
}


bool vrDisplayToolPan::MouseDClickLeft(const wxMouseEvent & event) {
    // compute middle pixel
    wxSize myDisplaySize = GetDisplay()->GetSize();
    if (myDisplaySize.IsFullySpecified() == false || myDisplaySize == wxSize(0,0)) {
        wxLogError("Error getting display size!");
        return false;
    }
    wxPoint myMiddlePoint(wxRound(myDisplaySize.GetWidth() / 2.0),
                          wxRound(myDisplaySize.GetHeight() / 2.0));
    // computing moved point
    wxPoint myMovedPoint (event.GetPosition() - myMiddlePoint);

    vrDisplayToolMessage * myMessage = new vrDisplayToolMessage(vrEVT_TOOL_PAN,
																GetDisplay(),
																myMovedPoint);
	wxASSERT(myMessage);
	SendMessage(myMessage);
    return true;
}



/***************************************************************************//**
@brief Default viewfinder tool
@author Lucien Schreiber (c) CREALP 2010
@date 29 avril 2010
 *******************************************************************************/
vrDisplayToolSight::vrDisplayToolSight(vrViewerDisplay * display) {
	Create(display, wxID_DEFAULT, "Sight", wxCursor(wxCURSOR_CROSS));
}


vrDisplayToolSight::~vrDisplayToolSight() {
}

bool vrDisplayToolSight::MouseDown(const wxMouseEvent & event) {
	wxClientDC myDC (GetDisplay());
	wxDCOverlay overlaydc (m_Overlay, &myDC);
	overlaydc.Clear();
	myDC.SetPen(*wxRED_PEN);
	myDC.CrossHair(event.GetPosition());


	vrDisplayToolMessage * myMsg = new vrDisplayToolMessage(vrEVT_TOOL_SIGHT,
															GetDisplay(),
															event.GetPosition());
	SendMessage(myMsg);
	return true;
}

bool vrDisplayToolSight::MouseUp(const wxMouseEvent & event) {
	{
		wxClientDC myDC (GetDisplay());
		wxDCOverlay overlaydc (m_Overlay, &myDC);
		overlaydc.Clear();
	}
	m_Overlay.Reset();

	vrDisplayToolMessage * myMsg = new vrDisplayToolMessage(vrEVT_TOOL_SIGHT,
															GetDisplay(),
															wxDefaultPosition);
	SendMessage(myMsg);

	return true;
}

bool vrDisplayToolSight::MouseMove(const wxMouseEvent & event) {
	if (event.Dragging() == false) {
		return false;
	}


	{
		wxClientDC myDC (GetDisplay());
		wxDCOverlay overlaydc (m_Overlay, &myDC);
		overlaydc.Clear();
	}
	m_Overlay.Reset();


	wxClientDC myDC (GetDisplay());
	wxDCOverlay overlaydc (m_Overlay, &myDC);
	overlaydc.Clear();
	myDC.SetPen(*wxRED_PEN);
	myDC.CrossHair(event.GetPosition());


	vrDisplayToolMessage * myMsg = new vrDisplayToolMessage(vrEVT_TOOL_SIGHT,
															GetDisplay(),
															event.GetPosition());
	SendMessage(myMsg);
	return true;
}



