/***************************************************************************
 vrlayervectorstar.cpp
 
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

#include "vrLayerVectorstar.h"
#include "vrlabel.h"
#include "vrrender.h"



bool vrLayerVectorStar::_DrawPoints(wxGraphicsContext * gdc, const wxRect2DDouble & coord,
								   const vrRender * render, const vrLabel * label, double pxsize) {
	m_ObjectDrawn = 0;
	wxASSERT(gdc);
	wxStopWatch sw;
	
	wxASSERT(render->GetType() == vrRENDER_VECTOR);
	vrRenderVector * myRender = (vrRenderVector*) render;
	wxPen myDefaultPen (myRender->GetColorPen(), myRender->GetSize());
	wxPen mySelPen (myRender->GetSelectionColour(), myRender->GetSize());
	
	// iterating and drawing geometries
	OGRPoint * myGeom = NULL;
	long iCount = 0;
	double myWidth = 0, myHeight = 0;
	gdc->GetSize(&myWidth, &myHeight);
	wxRect2DDouble myWndRect (0,0,myWidth, myHeight);
	while (1) {
		OGRFeature * myFeat = GetNextFeature(false);
		if (myFeat == NULL) {
			break;
		}
		myGeom = NULL;
		myGeom = (OGRPoint*) myFeat->GetGeometryRef();
		wxASSERT(myGeom);
		
		// get star size 
		int myStarSize = myFeat->GetFieldAsInteger(0);
		wxPoint myPt = _GetPointFromReal(wxPoint2DDouble(myGeom->getX(),myGeom->getY()),
										 coord.GetLeftTop(),
										 pxsize);
		
		// Create star
		wxGraphicsPath mySPath = gdc->CreatePath();		
		_CreateStarPath(mySPath, myPt, myStarSize);
		
		// ensure intersecting display
		wxRect2DDouble myPathRect = mySPath.GetBox();
		if (myPathRect.Intersects(myWndRect) ==false) {
			OGRFeature::DestroyFeature(myFeat);
			myFeat = NULL;
			continue;
		}
		
		if (myPathRect.GetSize().x < 1 && myPathRect.GetSize().y < 1){
			OGRFeature::DestroyFeature(myFeat);
			myFeat = NULL;
			continue;			
		}
		iCount++;
		
		// selection pen
		gdc->SetPen(myDefaultPen);
		if (IsFeatureSelected(myFeat->GetFID())==true) {
			gdc->SetPen(mySelPen);
		}		
		gdc->StrokePath(mySPath);
        
		OGRFeature::DestroyFeature(myFeat);
		myFeat = NULL;
	}
	
	m_ObjectDrawn = iCount;
	wxLogMessage("%ld stars drawed in %ldms", iCount, sw.Time());
	if (iCount == 0){
		return false;
	}
	return true;
}



void vrLayerVectorStar::_CreateStarPath(wxGraphicsPath & starpath, const wxPoint & center, int radius){
	const int myAngle = 72;
	const int myNumberPeak = 5;
	
	wxArrayInt myX;
	wxArrayInt myY;
	
	// computing peak coordinates
	int myActualAngle = 0;
	for (int i = 0; i < myNumberPeak; i++) {
		if (myActualAngle >= 360) {
			myActualAngle = 0;
		}
		double x = cos(_DegToRad(myActualAngle));
		double y = sin(_DegToRad(myActualAngle));
		myX.Add(wxRound(x * radius + center.x));
		myY.Add(wxRound(y * radius + center.y));
		myActualAngle = myActualAngle + myAngle;
	}
	
	// drawing star
	starpath.MoveToPoint(myX[0], myY[0]);
	int myPoint = 2;
	for (unsigned int i = 0; i<myX.GetCount(); i++) {
		starpath.AddLineToPoint(myX[myPoint], myY[myPoint]);
		myPoint = myPoint + 2;
		if (myPoint >= myNumberPeak) {
			myPoint = myPoint - myNumberPeak;
		}
	}
}



bool vrLayerVectorStar::_DrawLines(wxGraphicsContext * gdc, const wxRect2DDouble & coord,
								  const vrRender * render, const vrLabel * label, double pxsize) {
	m_ObjectDrawn = 0;
	return false;
}



bool vrLayerVectorStar::_DrawPolygons(wxGraphicsContext * gdc, const wxRect2DDouble & coord,
									 const vrRender * render, const vrLabel * label, double pxsize) {
	m_ObjectDrawn = 0;
	return false;
}

vrLayerVectorStar::vrLayerVectorStar() {
	wxASSERT(m_Dataset==NULL);
	wxASSERT(m_Layer==NULL);
	m_DriverType = vrDRIVER_VECTOR_MEMORY;
}


vrLayerVectorStar::~vrLayerVectorStar() {
}



long vrLayerVectorStar::AddFeature(OGRGeometry * geometry, void * data) {
	wxASSERT(m_Layer);
	OGRFeature * myFeature = OGRFeature::CreateFeature(m_Layer->GetLayerDefn());
	wxASSERT(m_Layer);
	myFeature->SetGeometry(geometry);
	
	if (data != NULL) {
		int * mySize = (int*) data;
		myFeature->SetField(0, *mySize);
	}
	
	if(m_Layer->CreateFeature(myFeature) != OGRERR_NONE){
		wxLogError(_("Error creating feature"));
		OGRFeature::DestroyFeature(myFeature);
		return wxNOT_FOUND;
	}
	long myFeatureID = myFeature->GetFID();
	wxASSERT(myFeatureID != OGRNullFID);
	OGRFeature::DestroyFeature(myFeature);
	return myFeatureID;
}





