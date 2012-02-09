/***************************************************************************
 vrlabelc2p.cpp
 -------------------
 copyright            : (C) 2011 CREALP Lucien Schreiber 
 email                : lucien.schreiber at crealp dot vs dot ch
 ***************************************************************************/


#include "vrlabelc2p.h"
#include "vrrendervectorc2p.h"
#include <ogr_geometry.h>


vrLabelC2P::vrLabelC2P(wxFont font, wxColour color) : vrLabel(font, color) {
    m_ActualPt = wxDefaultPosition;
    m_Offset = 5;
    m_ActualRotation = 0.0;
    m_ActualText = wxEmptyString;
    m_ActualSelected = false;
}



vrLabelC2P::~vrLabelC2P() {
}



bool vrLabelC2P::AddFeature(long fid, OGRGeometry * geom, const wxString & text, double rotation) {
    if (geom == NULL) {
        return false;
    }
    
    m_ActualSelected = false;
    if (fid == 1) {
        m_ActualSelected = true;
    }
    
    OGRPoint * myPt = (OGRPoint*) geom;
    m_ActualPt = wxPoint(wxRound(myPt->getX()), wxRound(myPt->getY()));
    m_ActualRotation = rotation;
    m_ActualText = text + wxString("\u00B0"); // degree symbol
    return true;
}



bool vrLabelC2P::Draw(wxGraphicsContext * gdc, const wxRect2DDouble & coord, const vrRender * render, double pixsize) {
    wxASSERT(gdc);
    vrRenderVectorC2PDips * myRender = (vrRenderVectorC2PDips*) render;
    if (m_ActualSelected == true) {
        gdc->SetFont(GetFont(), myRender->GetSelectionColour());
    }
    else{
        gdc->SetFont(GetFont(), GetColor());
    }
    // Rotate and transform matrix
    wxGraphicsMatrix myRotateMatrix = gdc->CreateMatrix();
    myRotateMatrix.Rotate((m_ActualRotation * M_PI) / 180);
    wxGraphicsMatrix myTranslateMatrix = gdc->CreateMatrix();
    myTranslateMatrix.Translate(m_ActualPt.x, m_ActualPt.y);
    
    double labelpos = 2.0;
    if (m_ActualRotation > 90 && m_ActualRotation < 270) {
        labelpos = -2.0;
    }
    
    wxGraphicsPath myTempPath = gdc->CreatePath();
    myTempPath.MoveToPoint(labelpos * myRender->GetDipWidth(), 0.0);
    myTempPath.Transform(myRotateMatrix);
    myTempPath.Transform(myTranslateMatrix);
    
    wxPoint2DDouble myMovedPt = myTempPath.GetCurrentPoint();
    gdc->DrawText(m_ActualText,
                  myMovedPt.m_x,
                  myMovedPt.m_y,
                  0);
    return true;
}

