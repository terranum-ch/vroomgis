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
	_DrawRoundedMessage("No GIS Data");
}


vrViewerDisplay::vrViewerDisplay(wxWindow * parent, wxWindowID id, long style) :
wxScrolledCanvas(parent, id, wxDefaultPosition, wxDefaultSize, style){
	
	// connect event
	Connect( wxEVT_PAINT, 
						   wxPaintEventHandler( vrViewerDisplay::OnPaint ),
						   NULL, this );
}

vrViewerDisplay::~vrViewerDisplay() {
	// disconnect event
	Disconnect( wxEVT_PAINT, 
							  wxPaintEventHandler( vrViewerDisplay::OnPaint ),
							  NULL, this );

}


void vrViewerDisplay::Refresh() {
	Refresh();
	Update();
}


