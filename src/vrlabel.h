/***************************************************************************
								vrlabel.h
				Label definition class
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
#ifndef _VRLABEL_H
#define _VRLABEL_H

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif


class vrLabel {
  private:
    wxFont m_Font;

    wxColor m_Color;


  public:
    vrLabel(const wxFont & font = *wxNORMAL_FONT, const wxColour & color = *wxBLACK);

    virtual ~vrLabel();

    inline const wxFont GetFont() const;

    inline const wxColor GetColor() const;

    void SetFont(wxFont value);

    void SetColor(wxColor value);

};
inline const wxFont vrLabel::GetFont() const {
  return m_Font;
}

inline const wxColor vrLabel::GetColor() const {
  return m_Color;
}

#endif
