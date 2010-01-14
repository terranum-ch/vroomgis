/***************************************************************************
								vrviewerdisplay.h
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
#ifndef _VRVIEWERDISPLAY_H
#define _VRVIEWERDISPLAY_H

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/wx.h>
	#include <wx/dcbuffer.h>
#endif


class vrViewerDisplay : public wxScrolledCanvas {
private:
    //vrCoordinate * m_Coordinate;
	
	bool _DrawRoundedMessage(const wxString & text, const wxColour & colour = *wxLIGHT_GREY);
	
	 void OnPaint(wxPaintEvent & event);
	
public:
    vrViewerDisplay(wxWindow * parent, wxWindowID id = wxID_ANY, long style = wxHSCROLL | wxVSCROLL);
	
    virtual ~vrViewerDisplay();
	
    void Refresh();
	
};

#endif