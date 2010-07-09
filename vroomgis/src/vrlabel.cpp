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
		m_Font = wxFont(12,wxFONTFAMILY_DEFAULT,
						wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL);
	}
	else {
		m_Font = font;
	}

	m_Color = color;
}

vrLabel::~vrLabel() {
}

void vrLabel::SetFont(wxFont value) {
  m_Font = value;
}

void vrLabel::SetColor(wxColor value) {
  m_Color = value;
}
