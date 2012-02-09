/***************************************************************************
 vrlabelc2p.h
 -------------------
 copyright            : (C) 2012 CREALP Lucien Schreiber 
 email                : lucien.schreiber at crealp dot vs dot ch
 ***************************************************************************/

#ifndef _VRLABELC2P_H_
#define _VRLABELC2P_H_

// wxWidgets include
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include "vrlabel.h"



class vrRender;

class vrLabelC2P : public vrLabel {
private:
    wxPoint m_ActualPt;
    int m_Offset;
    double m_ActualRotation;
    wxString m_ActualText;
    bool m_ActualSelected;
    
public:
    vrLabelC2P(wxFont font = *wxNORMAL_FONT, wxColour color = *wxBLACK);
    virtual ~vrLabelC2P();
    
    virtual bool AddFeature(long fid, OGRGeometry * geom, const wxString & text, double rotation = 0.0);
    bool Draw(wxGraphicsContext * gdc, const wxRect2DDouble & coord, const vrRender * render, double pixsize = 0);
};
#endif

