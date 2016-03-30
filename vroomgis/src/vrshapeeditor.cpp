/***************************************************************************
 vrshapeeditor.cpp
 
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

#include "vrshapeeditor.h"
#include "vrviewerlayermanager.h"
#include "vrviewerdisplay.h"
#include "vrcoordinate.h"
#include "vrrender.h"
#include "vrvieweroverlay.h"

vrShapeEditor::vrShapeEditor(vrViewerDisplay *display)
{
    m_display = display;
    wxASSERT(m_display);
    m_geometry = NULL;
    m_type = vrSHAPEEDITOR_TYPE_UNKHNOWN;
}

vrShapeEditor::~vrShapeEditor()
{
    if (m_geometry != NULL) {
        OGRGeometryFactory::destroyGeometry(m_geometry);
        m_geometry = NULL;
    }
}


bool vrShapeEditor::SetGeometry(OGRGeometry *geometry)
{
    if (geometry == NULL) {
        return false;
    }
    if (m_geometry != NULL) {
        OGRGeometryFactory::destroyGeometry(m_geometry);
        m_geometry = NULL;
    }
    m_geometry = geometry->clone();
    return true;
}


OGRGeometry *vrShapeEditor::GetGeometryRef()
{
    return m_geometry;
}


void vrShapeEditor::DrawShapeModify(vrRender *render)
{
    vrRenderVector *myOriginRender = (vrRenderVector *) render;
    vrRenderVector myRenderVector(*myOriginRender);
    myRenderVector.SetColorPen(render->GetSelectionColour());
    DrawShapeFinish(&myRenderVector);
}


vrShapeEditorPoint::vrShapeEditorPoint(vrViewerDisplay *display)
        : vrShapeEditor(display)
{
    if (m_geometry == NULL) {
        m_geometry = OGRGeometryFactory::createGeometry(wkbPoint);
    }
    m_type = vrSHAPEEDITOR_TYPE_POINT;
}


vrShapeEditorPoint::~vrShapeEditorPoint()
{
    // geometry destruction will occur in parent.
}


bool vrShapeEditorPoint::AddVertex(const wxPoint2DDouble &point)
{
    wxASSERT(m_geometry);
    OGRPoint *myPt = (OGRPoint *) m_geometry;
    myPt->setX(point.m_x);
    myPt->setY(point.m_y);
    return true;
}


void vrShapeEditorPoint::DrawShapeFinish(vrRender *render)
{
    wxASSERT(m_display);
    wxASSERT(m_geometry);

    vrRenderVector *myRender = (vrRenderVector *) render;
    wxPen myPen(myRender->GetColorPen(), myRender->GetSize());
    wxPoint myPt;
    OGRPoint *myOGRPt = (OGRPoint *) m_geometry;
    bool myResult = m_display->GetCoordinate()->ConvertToPixels(wxPoint2DDouble(myOGRPt->getX(), myOGRPt->getY()),
                                                                myPt);
    wxASSERT(myResult);
    wxClientDC myDC((wxWindow *) m_display);
    myDC.SetPen(myPen);

#ifdef __WXMSW__
    myDC.DrawLine(myPt.x, myPt.y, myPt.x + 0.1, myPt.y + 0.1);
#else
    myDC.DrawLine(myPt.x, myPt.y, myPt.x, myPt.y);
#endif
}


vrShapeEditorLine::vrShapeEditorLine(vrViewerDisplay *display)
        : vrShapeEditor(display)
{
    if (m_geometry == NULL) {
        m_geometry = OGRGeometryFactory::createGeometry(wkbLineString);
    }
    m_type = vrSHAPEEDITOR_TYPE_LINE;
}


vrShapeEditorLine::~vrShapeEditorLine()
{
    // geometry destruction in parent
}


bool vrShapeEditorLine::AddVertex(const wxPoint2DDouble &point)
{
    OGRPoint myPt;
    myPt.setX(point.m_x);
    myPt.setY(point.m_y);

    wxASSERT(m_geometry);
    OGRLineString *myLine = (OGRLineString *) m_geometry;

    // Check that the vertex that we are trying to add is not equal to the last vertex
    if (myLine->getNumPoints() > 0) {
        OGRPoint myLastPoint;
        myLine->getPoint(myLine->getNumPoints() - 1, &myLastPoint);
        wxASSERT(myLastPoint.IsValid() == true);
        if (myLastPoint.Equals(&myPt) == true) {
            wxLogMessage(_("Point: %.3f, %.3f already exists"), point.m_x, point.m_y);
            return false;
        }
    }

    myLine->addPoint(&myPt);
    return true;
}


void vrShapeEditorLine::DrawShapeEdit(vrRender *render)
{
    wxASSERT(m_display);
    wxASSERT(m_geometry);

    //wxASSERT(render->GetType() == vrRENDER_VECTOR);
    vrRenderVector *myRender = (vrRenderVector *) render;
    wxPen myPen(myRender->GetColorPen(), myRender->GetSize());

    wxClientDC myDC(m_display);
    wxDCOverlay overlaydc(m_overlay, &myDC);
    overlaydc.Clear();

    myDC.SetPen(myPen);
    OGRLineString *myLine = (OGRLineString *) m_geometry;
    if (myLine->getNumPoints() <= 1) {
        return;
    }

    wxPoint *myPts = new wxPoint[myLine->getNumPoints()];
    for (int i = 0; i < myLine->getNumPoints(); i++) {
        wxPoint2DDouble myRealPt(myLine->getX(i), myLine->getY(i));
        wxPoint myPxPt = wxDefaultPosition;
        m_display->GetCoordinate()->ConvertToPixels(myRealPt, myPxPt);
        myPts[i] = myPxPt;
    }
    myDC.DrawLines(myLine->getNumPoints(), myPts);
    wxDELETEA(myPts);
}


void vrShapeEditorLine::ViewChanged()
{
    m_overlay.Reset();

}


/*************************************************************************************//**
@brief Draw temporarily polygons
@author Lucien Schreiber copyright CREALP
@date 02 avril 2012
*****************************************************************************************/
vrShapeEditorPolygon::vrShapeEditorPolygon(vrViewerDisplay *display)
        : vrShapeEditorLine(display)
{
    m_type = vrSHAPEEDITOR_TYPE_POLYGON;
}

vrShapeEditorPolygon::~vrShapeEditorPolygon()
{
}


OGRGeometry *vrShapeEditorPolygon::GetGeometryRef()
{
    wxASSERT(m_geometry);
    OGRLinearRing *myRing = (OGRLinearRing *) m_geometry;
    if (myRing->getNumPoints() < 3) {
        wxLogError(_("Only %d vertex, unable to create polygon"), myRing->getNumPoints());
        return NULL;
    }

    OGRGeometry *myGeom = OGRGeometryFactory::createGeometry(wkbPolygon);
    OGRPolygon *myPolygonGeom = (OGRPolygon *) myGeom;
    myPolygonGeom->addRing(myRing);
    myPolygonGeom->closeRings();
    OGRGeometryFactory::destroyGeometry(m_geometry);
    m_geometry = NULL;
    return myGeom;
}


void vrShapeEditorPolygon::ViewChanged()
{
    vrShapeEditorLine::ViewChanged();
}



