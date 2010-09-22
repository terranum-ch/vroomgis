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
#include "vrrenderer.h"
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



void vrShapeEditorPoint::DrawShape(vrRenderer * renderer) {
	wxASSERT(m_Display);
	wxASSERT(m_Geometry);
	
	vrRenderVector * myRender = (vrRenderVector*) renderer->GetRender();
	wxPen myPen (myRender->GetColorPen(),
				 myRender->GetSize());
	wxPoint myPt;
	OGRPoint * myOGRPt = (OGRPoint*) m_Geometry;
	bool myResult = m_Display->GetCoordinate()->ConvertToPixels(wxPoint2DDouble(myOGRPt->getX(), myOGRPt->getY()), myPt);
	wxASSERT(myResult);
	wxClientDC myDC ((wxWindow*)m_Display);
	wxGraphicsContext * gdc = wxGraphicsContext::Create(myDC);
	wxASSERT(gdc);
	gdc->SetPen(myPen);
	
#ifdef __WXMSW__
	gdc->StrokeLine (myPt.x, myPt.y, myPt.x + 0.1, myPt.y + 0.1);
#else
	gdc->StrokeLine (myPt.x, myPt.y, myPt.x, myPt.y);
#endif
	wxDELETE(gdc);

}

