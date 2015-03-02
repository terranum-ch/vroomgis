/***************************************************************************
				vrlabel.cpp
                    
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

#include "vrlabel.h"

vrLabel::vrLabel(const wxFont & font, const wxColour & color) {
	
	// create default font and color if required
	if (font == wxNullFont){
		m_font = wxFont(12,wxFONTFAMILY_DEFAULT,
						wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL);
	}
	else {
		m_font = font;
	}

	m_color = color;
    SetActive(false);
    SetField(wxNOT_FOUND);
}



vrLabel::~vrLabel() {
}



void vrLabel::SetFont(wxFont value) {
  m_font = value;
}



void vrLabel::SetColor(wxColor value) {
  m_color = value;
}


void vrLabel::SetActive(bool value) {
    m_active = value;
}



void vrLabel::SetField(int value) {
    m_field = value;
}



bool vrLabel::Serialize(vrSerialize & serialobj) {
	serialobj.EnterObject();
	if (serialobj.IsStoring()) {
		serialobj << m_font;
		serialobj << m_color;
        serialobj << m_field;
        serialobj << m_active;
	}
	else {
		serialobj >> m_font;
		serialobj >> m_color;
        serialobj >> m_field;
        serialobj >> m_active;
	}
	serialobj.LeaveObject();
	return serialobj.IsOk();
}