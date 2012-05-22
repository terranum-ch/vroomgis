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

vrShapeEditor::vrShapeEditor(vrViewerDisplay * display) {
	m_Display = display;
	wxASSERT(m_Display);
	m_Geometry = NULL; 
}

vrShapeEditor::~vrShapeEditor() {
	if (m_Geometry != NULL) {
		OGRGeometryFactory::destroyGeometry(m_Geometry);
		m_Geometry = NULL;
	}
}


bool vrShapeEditor::SetGeometry(OGRGeometry * geometry) {
    if (geometry == NULL) {
        return false;
    }
    if (m_Geometry != NULL) {
		OGRGeometryFactory::destroyGeometry(m_Geometry);
		m_Geometry = NULL;
	}
    m_Geometry = geometry->clone();
    return true;
}



OGRGeometry * vrShapeEditor::GetGeometryRef(){
	return m_Geometry;
}



void vrShapeEditor::DrawShapeModify(vrRender * render) {
    vrRenderVector * myOriginRender = (vrRenderVector*) render;
    vrRenderVector myRenderVector(*myOriginRender);
    myRenderVector.SetColorPen(render->GetSelectionColour());
    DrawShapeFinish(&myRenderVector);
}






vrShapeEditorPoint::vrShapeEditorPoint(vrViewerDisplay * display) : vrShapeEditor(display) {
	if (m_Geometry == NULL) {
		m_Geometry = OGRGeometryFactory::createGeometry(wkbPoint);
	}
}



vrShapeEditorPoint::~vrShapeEditorPoint() {
	// geometry destruction will occur in parent.
}



bool vrShapeEditorPoint::AddVertex(const wxPoint2DDouble & point) {
	wxASSERT(m_Geometry);
	OGRPoint * myPt = (OGRPoint*) m_Geometry;
	myPt->setX(point.m_x);
	myPt->setY(point.m_y);
	return true;
}



void vrShapeEditorPoint::DrawShapeFinish(vrRender * render) {
	wxASSERT(m_Display);
	wxASSERT(m_Geometry);
	
	vrRenderVector * myRender = (vrRenderVector*) render;
	wxPen myPen (myRender->GetColorPen(),
				 myRender->GetSize());
	wxPoint myPt;
	OGRPoint * myOGRPt = (OGRPoint*) m_Geometry;
	bool myResult = m_Display->GetCoordinate()->ConvertToPixels(wxPoint2DDouble(myOGRPt->getX(), myOGRPt->getY()), myPt);
	wxASSERT(myResult);
	wxClientDC myDC ((wxWindow*)m_Display);
	myDC.SetPen(myPen);
	
#ifdef __WXMSW__
	myDC.DrawLine(myPt.x, myPt.y, myPt.x + 0.1, myPt.y + 0.1);
#else
	myDC.DrawLine(myPt.x, myPt.y, myPt.x, myPt.y);
#endif
}






vrShapeEditorLine::vrShapeEditorLine(vrViewerDisplay * display) : vrShapeEditor(display) {
    if (m_Geometry == NULL) {
		m_Geometry = OGRGeometryFactory::createGeometry(wkbLineString);
	}
}



vrShapeEditorLine::~vrShapeEditorLine() {
    // geometry destruction in parent
}



bool vrShapeEditorLine::AddVertex(const wxPoint2DDouble & point) {
    OGRPoint myPt;
    myPt.setX(point.m_x);
    myPt.setY(point.m_y);
    
    wxASSERT(m_Geometry);
    OGRLineString * myLine = (OGRLineString*) m_Geometry;
    
    // Check that the vertex that we are trying to add is not equal to the last vertex
    if (myLine->getNumPoints() > 0) {
        OGRPoint myLastPoint;
        myLine->getPoint(myLine->getNumPoints()-1, &myLastPoint);
        wxASSERT(myLastPoint.IsValid() == true);
        if (myLastPoint.Equals(&myPt)==true) {
            wxLogMessage(_("Point: %.3f, %.3f already exists"),point.m_x, point.m_y);
            return false;
        }
    }
    
    myLine->addPoint(&myPt);
    return true;
}



void vrShapeEditorLine::DrawShapeEdit(vrRender * render) {
    wxASSERT(m_Display);
	wxASSERT(m_Geometry);
    
    wxASSERT(render->GetType() == vrRENDER_VECTOR);
	vrRenderVector * myRender = (vrRenderVector*) render;
	wxPen myPen (myRender->GetColorPen(),
				 myRender->GetSize());
       
	wxClientDC myDC (m_Display);
    wxDCOverlay overlaydc (m_Overlay, &myDC);
	overlaydc.Clear();
   
	myDC.SetPen(myPen);
    OGRLineString * myLine = (OGRLineString*) m_Geometry;
    if (myLine->getNumPoints() <= 1) {
        return;
    }
    
    wxPoint * myPts = new wxPoint[myLine->getNumPoints()];
    for (int i = 0; i<myLine->getNumPoints(); i++) {
        wxPoint2DDouble myRealPt (myLine->getX(i), myLine->getY(i));
        wxPoint myPxPt = wxDefaultPosition;
        m_Display->GetCoordinate()->ConvertToPixels(myRealPt, myPxPt);
        myPts[i] = myPxPt;
    }
    myDC.DrawLines(myLine->getNumPoints(), myPts);
    wxDELETEA(myPts);    
    
    
}



void vrShapeEditorLine::ViewChanged(){
     m_Overlay.Reset();
}


/*************************************************************************************//**
@brief Draw temporarily polygons
@author Lucien Schreiber copyright CREALP
@date 02 avril 2012
*****************************************************************************************/
vrShapeEditorPolygon::vrShapeEditorPolygon(vrViewerDisplay * display): vrShapeEditorLine(display) {
}

vrShapeEditorPolygon::~vrShapeEditorPolygon() {
}



void vrShapeEditorPolygon::DrawShapeFinish(vrRender * render) {
    wxASSERT(m_Display);
	wxASSERT(m_Geometry);
    
    // convert polygon geometry to line
    if (wkbFlatten(m_Geometry->getGeometryType()) == wkbPolygon) {
        OGRGeometry * myPolyExtLine = ((OGRPolygon*) m_Geometry)->getExteriorRing()->clone();
        OGRGeometryFactory::destroyGeometry(m_Geometry);
        m_Geometry = NULL;
        m_Geometry = myPolyExtLine;
    }
    
    
    wxASSERT(render->GetType() == vrRENDER_VECTOR);
	vrRenderVector * myRender = (vrRenderVector*) render;
	wxPen myPen (myRender->GetColorPen(),
				 myRender->GetSize());
    wxBrush myBrush (myRender->GetColorBrush(),
                     myRender->GetBrushStyle());
    
	wxClientDC myDC (m_Display);
	wxDCOverlay overlaydc (m_Overlay, &myDC);
	overlaydc.Clear();
    
	myDC.SetPen(myPen);
    myDC.SetBrush(myBrush);
    
    OGRLineString * myLine = (OGRLineString*) m_Geometry;
    if (myLine->getNumPoints() < 4) {
        return;
    }
    
    wxPoint * myPts = new wxPoint[myLine->getNumPoints()];
    for (int i = 0; i<myLine->getNumPoints(); i++) {
        wxPoint2DDouble myRealPt (myLine->getX(i), myLine->getY(i));
        wxPoint myPxPt = wxDefaultPosition;
        m_Display->GetCoordinate()->ConvertToPixels(myRealPt, myPxPt);
        // first point = last point
        //if (i == 0) {
          //  myPts[myLine->getNumPoints()] = myPxPt;
        //}
        myPts[i] = myPxPt;
    }
    
    myDC.DrawLine(0, 0, 50, 50);
    myDC.DrawPolygon(myLine->getNumPoints(), myPts);    
    wxDELETEA(myPts);    
}



OGRGeometry * vrShapeEditorPolygon::GetGeometryRef(){
    wxASSERT(m_Geometry);
    OGRLinearRing * myRing = (OGRLinearRing*) m_Geometry;
    if (myRing->getNumPoints() < 3) {
        wxLogError(_("Only %d vertex, unable to create polygon"), myRing->getNumPoints());
        return NULL;
    }    
    
    OGRGeometry * myGeom = OGRGeometryFactory::createGeometry(wkbPolygon);
    OGRPolygon * myPolygonGeom = (OGRPolygon*) myGeom;
    myPolygonGeom->addRing(myRing);
    myPolygonGeom->closeRings();
    OGRGeometryFactory::destroyGeometry(m_Geometry);
    m_Geometry = NULL;
	return myGeom;
}

void vrShapeEditorPolygon::ViewChanged(){
    vrShapeEditorLine::ViewChanged();
}




