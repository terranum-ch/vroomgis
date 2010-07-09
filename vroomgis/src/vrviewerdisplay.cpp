/***************************************************************************
								vrviewerdisplay.cpp
								Default display
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

#include "vrviewerdisplay.h"
#include "vrcoordinate.h"
#include "vrevent.h"
#include "vrdisplaytool.h"
#include "vrviewerlayermanager.h"


bool vrViewerDisplay::_DrawRoundedMessage(const wxString & text, const wxColour & colour) {
	if (text.IsEmpty()) {
		wxLogError("No text specified, specify a text");
		return false;
	}
	
	wxClientDC dc(this);
	wxBufferedDC bdc(&dc, dc.GetSize());
	wxASSERT(bdc.IsOk());
	int width = 0;
	int height = 0;
	dc.GetSize(&width, &height);
	if (width < 20 || height < 20) {
		wxLogError("Windows size is too small for drawing");
		return false;
	}
	// erase screen 
	bdc.Clear();
	bdc.SetBrush(wxBrush(GetBackgroundColour()));
	bdc.SetPen(wxPen(GetBackgroundColour()));
	
	//bdc.SetBrush(*wxWHITE_BRUSH);
	//bdc.SetPen(*wxWHITE_PEN);
	bdc.DrawRectangle(0,0,width,height);
	
	// draw rectangle
	bdc.SetBrush(wxBrush(colour));
	wxRect myRect (width / 4, height / 3, width / 2, height / 3);
	bdc.DrawRoundedRectangle(myRect, height / 20);
	
	
	
	// draw text
	wxFont myFont(myRect.GetWidth() / text.Len() , wxFONTFAMILY_DEFAULT, 
				  wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
	wxASSERT(myFont.IsOk());

	//TODO: Submit bug for this problem
#ifndef __WXMAC__
	bdc.SetFont(myFont);
#endif

	bdc.SetTextForeground(*wxWHITE);


	wxSize myTextSize = bdc.GetTextExtent(text);
	wxRect myTextRectComputed(myTextSize);
	myTextRectComputed = myTextRectComputed.CenterIn(myRect, wxBOTH);
	bdc.DrawText(text, myTextRectComputed.GetX(), myTextRectComputed.GetY());
	return true;
}


void vrViewerDisplay::OnPaint(wxPaintEvent & event) {
	wxPaintDC dc(this);
	
	if (m_bmp == NULL) {
		_DrawRoundedMessage("No GIS Data");
		return;
	}
	
	dc.DrawBitmap(*m_bmp, 0,0, true);
}


void vrViewerDisplay::OnSizeChange(wxSizeEvent & event) {
	
	if (event.GetSize().GetWidth() < 2 && event.GetSize().GetHeight() < 2) {
		event.Skip();
		return;
	}
	
	if (m_ViewerManager == NULL){
		return;
	}
	
	m_ViewerManager->Reload();
		
	wxASSERT(m_Coordinate);
	if (m_Coordinate->IsOk() == true) {
		m_Coordinate->UpdateExtent();
	}
	
	_InvalidateView(false);
	
	event.Skip();
}

void vrViewerDisplay::OnEraseBackground (wxPaintEvent & event){
	// this is used to avoid flickering 
}


void vrViewerDisplay::OnMouseDown(wxMouseEvent & event) {
	CaptureMouse();
	if (m_Tool != NULL) {
		m_Tool->MouseDown(event);
	}
}


void vrViewerDisplay::OnMouseUp(wxMouseEvent & event) {
	ReleaseMouse();
	if (m_Tool != NULL) {
		m_Tool->MouseUp(event);
	}
}


void vrViewerDisplay::OnMouseMove(wxMouseEvent & event) {
	if (m_Tool != NULL) {
		m_Tool->MouseMove(event);
	}
}


void vrViewerDisplay::OnMouseCaptureLost(wxMouseEvent & event){
	// Only used under Windows. Mouse capture is lost when a dialog is displayed.
	if (HasCapture() == true){
		ReleaseMouse();
	}
}



vrViewerDisplay::vrViewerDisplay(){
	m_Coordinate = NULL;
	m_bmp = NULL;
	m_Tool = NULL;
	m_ViewerManager = NULL;
	wxLogError("Don't use this vrViewerDisplay ctor, only here for tests");
}



vrViewerDisplay::vrViewerDisplay(wxWindow * parent, wxWindowID id, const wxColour & colour) :
wxPanel(parent, id){
	
	m_Coordinate = new vrCoordinate(this);
	m_bmp = NULL;
	m_Tool = NULL;
	m_ViewerManager = NULL;
	
	SetBackgroundColour(colour);
	
	// connect event
	Connect(wxEVT_ERASE_BACKGROUND, wxPaintEventHandler(vrViewerDisplay::OnEraseBackground),NULL,this);
	Connect(wxEVT_SIZE, wxSizeEventHandler(vrViewerDisplay::OnSizeChange),NULL,this);
	Connect( wxEVT_PAINT, wxPaintEventHandler( vrViewerDisplay::OnPaint ),NULL, this );
	
	// connect mouse event
	Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(vrViewerDisplay::OnMouseDown),NULL,this);
	Connect(wxEVT_LEFT_UP, wxMouseEventHandler(vrViewerDisplay::OnMouseUp),NULL,this);
	Connect(wxEVT_MOTION, wxMouseEventHandler(vrViewerDisplay::OnMouseMove),NULL,this);
	Connect(wxEVT_MOUSE_CAPTURE_LOST, wxMouseEventHandler(vrViewerDisplay::OnMouseCaptureLost),NULL,this);
	
}

vrViewerDisplay::~vrViewerDisplay() {

	
	// disconnect event
	Disconnect (wxEVT_ERASE_BACKGROUND, wxPaintEventHandler(vrViewerDisplay::OnEraseBackground),NULL,this);
	Disconnect(wxEVT_SIZE, wxSizeEventHandler(vrViewerDisplay::OnSizeChange),NULL,this);
	Disconnect( wxEVT_PAINT,wxPaintEventHandler( vrViewerDisplay::OnPaint ),NULL, this );
	
	// disconnect mouse event
	Disconnect(wxEVT_LEFT_DOWN, wxMouseEventHandler(vrViewerDisplay::OnMouseDown),NULL,this);
	Disconnect(wxEVT_LEFT_UP, wxMouseEventHandler(vrViewerDisplay::OnMouseUp),NULL,this);
	Disconnect(wxEVT_MOTION, wxMouseEventHandler(vrViewerDisplay::OnMouseMove),NULL,this);
	Disconnect(wxEVT_MOUSE_CAPTURE_LOST, wxMouseEventHandler(vrViewerDisplay::OnMouseCaptureLost),NULL,this);
	
	wxDELETE(m_Coordinate);
	wxDELETE(m_Tool);
	wxDELETE(m_bmp);
}


void vrViewerDisplay::SetViewerLayerManager(vrViewerLayerManager * value) {
	wxASSERT(value);
	m_ViewerManager = value;
}


 vrViewerLayerManager * vrViewerDisplay::GetViewerLayerManager() {
	return m_ViewerManager;
}


void vrViewerDisplay::SetBitmap(wxBitmap * bmp) {
	
	wxDELETE(m_bmp);
	
	// reference copy, this isn't expensive
	if (bmp != NULL) {
		m_bmp = new wxBitmap(*bmp);
	}
	_InvalidateView(true);
}


void vrViewerDisplay::_InvalidateView(bool updatenow) {
	this->Refresh();
	
	if (updatenow){
		this->Update();
	}
}


void vrViewerDisplay::SetToolDefault() {
	SetTool(new vrDisplayToolDefault(this));
}

void vrViewerDisplay::SetToolZoom() {
	SetTool(new vrDisplayToolZoom(this));
}



void vrViewerDisplay::SetToolPan() {
	SetTool(new vrDisplayToolPan(this));
}



void vrViewerDisplay::SetToolSight() {
	SetTool(new vrDisplayToolSight(this));
}


void vrViewerDisplay::SetTool(vrDisplayTool * tool) {
	wxDELETE(m_Tool);
	
	wxASSERT(m_Tool==NULL);
	if (tool == NULL) {
		return;
	}
	m_Tool = tool;
	SetCursor(m_Tool->GetCursor());
}




