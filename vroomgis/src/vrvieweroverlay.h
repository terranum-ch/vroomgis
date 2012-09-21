/***************************************************************************
 vrvieweroverlay.h
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
#ifndef _VRVIEWEROVERLAY_H_
#define _VRVIEWEROVERLAY_H_

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

// Include wxWidgets' headers
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include "vrrender.h"
#include <ogrsf_frmts.h>
class vrViewerDisplay;

/***************************************************************************//**
@brief Overlay abstract base class
@author Lucien Schreiber (c) CREALP 2012
@date 18 september 2012
*******************************************************************************/
class vrViewerOverlay {
private:
    wxString m_Name;
    bool m_Visible;
    
    
public:
    vrViewerOverlay(wxString name);
    virtual ~vrViewerOverlay();
    
    inline const wxString GetName() const;
    inline const bool IsVisible() const;
    
    void SetVisible(bool value);
    virtual bool DrawOverlay(wxPaintDC * dc) = 0;
};

inline const wxString vrViewerOverlay::GetName() const {
    return m_Name;
}


inline const bool vrViewerOverlay::IsVisible() const {
    return m_Visible;
}
WX_DECLARE_OBJARRAY(vrViewerOverlay*, vrViewerOverlayArray);





/***************************************************************************//**
@brief Overlay simple text class
@author Lucien Schreiber (c) CREALP 2012
@date 18 septembre 2012
*******************************************************************************/
class vrViewerOverlayText : public vrViewerOverlay {
private:
    wxFont m_Font;
    wxString m_Text;
    wxPoint m_Pos;
    wxColour m_TextColour;

public:
    vrViewerOverlayText(wxString name, wxString text);
    virtual ~vrViewerOverlayText();
    
    virtual bool DrawOverlay(wxPaintDC * dc);

    inline const wxFont GetFont() const;
    void SetFont(wxFont value);
    inline const wxString GetText() const;
    void SetText(wxString value);
    inline const wxPoint GetPosition() const;
    void SetPosition(wxPoint value);
    inline const wxColour GetTextColour() const;
    void SetTextColour(wxColour value);
};


inline const wxFont vrViewerOverlayText::GetFont() const {
    return m_Font;
}

inline const wxString vrViewerOverlayText::GetText() const {
    return m_Text;
}

inline const wxPoint vrViewerOverlayText::GetPosition() const {
    return m_Pos;
}

inline const wxColour vrViewerOverlayText::GetTextColour() const {
    return m_TextColour;
}


/*************************************************************************************//**
@brief Support Geometric polygon overlay
@author Lucien Schreiber copyright CREALP
@date 20 september 2012
*****************************************************************************************/
class vrViewerOverlayGeomPolygon : public vrViewerOverlay {
private:
    OGRPolygon * m_Polygon;
    vrRenderVector m_RenderPolygon;
    vrViewerDisplay * m_Display;
    
public:
    vrViewerOverlayGeomPolygon(const wxString & name, vrViewerDisplay * viewer);
    virtual ~vrViewerOverlayGeomPolygon();
    
    void SetPolygon(OGRPolygon * value);
    void SetRender(vrRenderVector * value);
    virtual bool DrawOverlay(wxPaintDC * dc);
};




#endif
