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
	bdc.SetBrush(*wxWHITE_BRUSH);
	bdc.SetPen(*wxWHITE_PEN);
	bdc.DrawRectangle(0,0,width,height);
	
	// draw rectangle
	bdc.SetBrush(wxBrush(colour));
	wxRect myRect (width / 4, height / 3, width / 2, height / 3);
	bdc.DrawRoundedRectangle(myRect, height / 20);
	
	
	
	// draw text
	wxFont myFont(myRect.GetWidth() / text.Len() , wxFONTFAMILY_DEFAULT, 
				  wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
	wxASSERT(myFont.IsOk());
	bdc.SetFont(myFont);
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
	
	wxCommandEvent myEvt(vrEVT_VLM_RELOAD);
	ProcessWindowEvent(myEvt);
	//_InvalidateView(false);
	event.Skip();
}

void vrViewerDisplay::OnEraseBackground (wxPaintEvent & event){
	// this is used to avoid flickering 
}


vrViewerDisplay::vrViewerDisplay(wxWindow * parent, wxWindowID id, const wxColour & colour) :
wxPanel(parent, id){
	
	m_Coordinate = new vrCoordinate(this);
	m_bmp = NULL;
	
	SetBackgroundColour(colour);
	
	// connect event
	Connect(wxEVT_ERASE_BACKGROUND, wxPaintEventHandler(vrViewerDisplay::OnEraseBackground),
		NULL,this);
	Connect(wxEVT_SIZE, wxSizeEventHandler(vrViewerDisplay::OnSizeChange),
			NULL,this);
	
	Connect( wxEVT_PAINT, 
			wxPaintEventHandler( vrViewerDisplay::OnPaint ),
			NULL, this );
}

vrViewerDisplay::~vrViewerDisplay() {
	
	wxASSERT(m_Coordinate);
	wxDELETE(m_Coordinate);
	
	// disconnect event
	Connect(wxEVT_ERASE_BACKGROUND, wxPaintEventHandler(vrViewerDisplay::OnEraseBackground),
		NULL,this);
	Disconnect(wxEVT_SIZE, wxSizeEventHandler(vrViewerDisplay::OnSizeChange),
			NULL,this);
	Disconnect( wxEVT_PAINT, 
			   wxPaintEventHandler( vrViewerDisplay::OnPaint ),
			   NULL, this );
	
	
	wxDELETE(m_bmp);
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


