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
#include "vrserialize.h"

class OGRGeometry;
class vrRender;


class vrLabel {
private:
    wxFont m_Font;
    wxColor m_Color;
    bool m_Active;
    int m_Field;
    
public:
    vrLabel(const wxFont & font = wxNullFont , const wxColour & color = *wxBLACK);
    virtual ~vrLabel();
    
    inline const wxFont GetFont() const;
    inline const wxColor GetColor() const;
    
    void SetFont(wxFont value);
    void SetColor(wxColor value);
    
    inline const bool IsActive() const;
    void SetActive(bool value);
    
    inline const int GetField() const;
    void SetField(int value);
    
	virtual bool Serialize(vrSerialize & serialobj);
    
    virtual bool AddFeature(long fid, OGRGeometry * geom, const wxString & text, double rotation = 0.0){return false;}
    virtual bool Draw(wxGraphicsContext * gdc, const wxRect2DDouble & coord, const vrRender * render, double pixsize = 0){return false;}
};


inline const wxFont vrLabel::GetFont() const {
    return m_Font;
}



inline const wxColor vrLabel::GetColor() const {
    return m_Color;
}



inline const bool vrLabel::IsActive() const {
    return m_Active;
}



inline const int vrLabel::GetField() const {
    return m_Field;
}


#endif
