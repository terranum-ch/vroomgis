/***************************************************************************
 vrvieweroverlay.cpp
 -------------------
 copyright            : (C) 2012 CREALP Lucien Schreiber 
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

#include "vrvieweroverlay.h"


/*****************************************************************************
Overlay abstract base class
*****************************************************************************/
vrViewerOverlay::vrViewerOverlay(wxString name) {
    m_Name = name;
    m_Visible = true;
}

vrViewerOverlay::~vrViewerOverlay() {
}

void vrViewerOverlay::SetVisible(bool value) {
  m_Visible = value;
}
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY (vrViewerOverlayArray);







/***************************************************************************//**
Overlay simple text class
*******************************************************************************/
vrViewerOverlayText::vrViewerOverlayText(wxString name, wxString text) :
 vrViewerOverlay(name){
     SetText(text);
     SetPosition(wxPoint(10,10));
     m_Font = *wxNORMAL_FONT;
     m_Font.SetPointSize(m_Font.GetPointSize() + 5);
     SetTextColour(*wxBLACK);
}



vrViewerOverlayText::~vrViewerOverlayText() {
}



bool vrViewerOverlayText::DrawOverlay(wxPaintDC * dc) {
    dc->SetTextForeground(GetTextColour());
    dc->SetFont(GetFont());
    dc->DrawText(GetText(), GetPosition());
    return true;
}



void vrViewerOverlayText::SetFont(wxFont value) {
  m_Font = value;
}



void vrViewerOverlayText::SetText(wxString value) {
  m_Text = value;
}

void vrViewerOverlayText::SetPosition(wxPoint value) {
  m_Pos = value;
}


void vrViewerOverlayText::SetTextColour(wxColour value) {
    m_TextColour = value;
}
