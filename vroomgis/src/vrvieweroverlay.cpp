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
#include "vrviewerdisplay.h"
#include "vrcoordinate.h"


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



/*************************************************************************************//**
Support Geometric polygon overlay
*****************************************************************************************/
vrViewerOverlayGeomPolygon::vrViewerOverlayGeomPolygon(const wxString & name, vrViewerDisplay * viewer) : vrViewerOverlay(name) {
    m_Display = viewer;
    m_Polygon = NULL;
}



vrViewerOverlayGeomPolygon::~vrViewerOverlayGeomPolygon() {
    OGRGeometryFactory::destroyGeometry(m_Polygon);
}



void vrViewerOverlayGeomPolygon::SetPolygon(OGRPolygon * value) {
    m_Polygon = (OGRPolygon*) value->clone();
}



void vrViewerOverlayGeomPolygon::SetRender(vrRenderVector * value) {
    m_RenderPolygon = *value;
}



bool vrViewerOverlayGeomPolygon::DrawOverlay(wxPaintDC * dc) {
    if (m_Polygon == NULL) {
        return false;
    }
    
	wxPen myPen (m_RenderPolygon.GetSelectionColour(),m_RenderPolygon.GetSize());
    wxBrush myBrush (m_RenderPolygon.GetColorBrush(),m_RenderPolygon.GetBrushStyle());
    
	dc->SetPen(myPen);
    dc->SetBrush(myBrush);
    
    OGRLineString * myLine = (OGRLineString*) m_Polygon->getExteriorRing();
    if (myLine->getNumPoints() < 4) {
        return false;
    }
    
    wxPoint * myPts = new wxPoint[myLine->getNumPoints()];
    for (int i = 0; i<myLine->getNumPoints(); i++) {
        wxPoint2DDouble myRealPt (myLine->getX(i), myLine->getY(i));
        wxPoint myPxPt = wxDefaultPosition;
        m_Display->GetCoordinate()->ConvertToPixels(myRealPt, myPxPt);
        myPts[i] = myPxPt;
    }
    
    dc->DrawPolygon(myLine->getNumPoints(), myPts);
    
    myPen.SetWidth(myPen.GetWidth()+4);
    dc->SetPen(myPen);
    for (int i  = 0; i< myLine->getNumPoints(); i++) {
        wxPoint myPt = myPts[i];
#ifdef __WXMSW__
        dc->DrawLine(myPt.x, myPt.y, myPt.x + 0.1, myPt.y + 0.1);
#else
        dc->DrawLine(myPt.x, myPt.y, myPt.x, myPt.y);
#endif
    }
    wxDELETEA(myPts);
    return true;
}


