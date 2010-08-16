/***************************************************************************
				vrlayervectorc2p.cpp
                    
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

#include "vrlayervectorc2p.h"
#include "vrrendervectorc2p.h"
#include "vrlabel.h"

bool vrLayerVectorC2P::_DrawPoints(wxGraphicsContext * gdc, const wxRect2DDouble & coord,
								   const vrRender * render, const vrLabel * label, double pxsize) {
	wxASSERT(gdc);
	wxStopWatch sw;
	// creating pen
	
	wxASSERT(render->GetType() == vrRENDER_VECTOR_C2P_DIPS);
	vrRenderVectorC2PDips * myRender = (vrRenderVectorC2PDips*) render;
	const int myDipWidth = myRender->GetDipWidth();
	
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
				
		// get direction
		double myDir = myFeat->GetFieldAsDouble(1);
		int myFamily = myFeat->GetFieldAsInteger(3);
		wxPoint myPt = _GetPointFromReal(wxPoint2DDouble(myGeom->getX(),myGeom->getY()),
										 coord.GetLeftTop(),
										 pxsize);
				
		// create pen
		wxPen myPen (myRender->GetDipColour(myFamily),myRender->GetSize());
		gdc->SetPen(myPen);
		
		// Create | line
		wxGraphicsPath myVPath = gdc->CreatePath();		
		myVPath.MoveToPoint(0.0, 0.0);
		myVPath.AddLineToPoint(0.0, -1.0 * myDipWidth);
		// Create -- line
		wxGraphicsPath myHPath = gdc->CreatePath();
		myHPath.MoveToPoint(-1.5 * myDipWidth, 0.0);
		myHPath.AddLineToPoint(1.5 * myDipWidth, 0.0);
		
		// Rotate
		wxGraphicsMatrix myRotateMatrix = gdc->CreateMatrix();
		myRotateMatrix.Rotate((myDir * M_PI) / 180);
		myVPath.Transform(myRotateMatrix);
		myHPath.Transform(myRotateMatrix);
		
		// Translate to position
		wxGraphicsMatrix myTranslateMatrix = gdc->CreateMatrix();
		myTranslateMatrix.Translate(myPt.x, myPt.y);
		myVPath.Transform(myTranslateMatrix);
		myHPath.Transform(myTranslateMatrix);

		// ensure intersecting display
		wxRect2DDouble myPathRect = myVPath.GetBox();
		myPathRect.Union(myHPath.GetBox());
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
		
		gdc->StrokePath(myHPath);
		gdc->StrokePath(myVPath);
		OGRFeature::DestroyFeature(myFeat);
		myFeat = NULL;
	}
	
	if (iCount <= 0) {
		wxLogWarning("No data drawn.");
	}
	wxLogMessage("%ld dips drawed in %ldms", iCount, sw.Time());
	return true;
}



bool vrLayerVectorC2P::_DrawLines(wxGraphicsContext * gdc, const wxRect2DDouble & coord,
								  const vrRender * render, const vrLabel * label, double pxsize) {
  return false;
}



bool vrLayerVectorC2P::_DrawPolygons(wxGraphicsContext * gdc, const wxRect2DDouble & coord,
									 const vrRender * render, const vrLabel * label, double pxsize) {
  return false;
}

vrLayerVectorC2P::vrLayerVectorC2P() {
	wxASSERT(m_Dataset==NULL);
	wxASSERT(m_Layer==NULL);
	m_DriverType = vrDRIVER_VECTOR_C2P;
}


vrLayerVectorC2P::~vrLayerVectorC2P() {
}



bool vrLayerVectorC2P::Open(const wxFileName & filename, bool readwrite) {
	// Need to reimplement Open to allow opening c2p files as if it was sqlite.
	_Close();
	wxASSERT(m_Dataset == NULL);
	
	m_FileName = filename;
	
	OGRSFDriverRegistrar * myRegistar = OGRSFDriverRegistrar::GetRegistrar();
	OGRSFDriver * myDriver = myRegistar->GetDriverByName("SQLite");
	if (myDriver == NULL) {
		wxLogError(_("Unable to load SQLite Driver! GDAL is maybe not compiled with SQLite support"));
		return false;
	}
	
	m_Dataset = myDriver->Open(filename.GetFullPath(), readwrite);
	if( m_Dataset == NULL ){
		wxLogError("Unable to open %s", filename.GetFullName());
		return false;
	}
	m_Dataset->SetDriver(myDriver);
	
	// get layer
	wxASSERT(m_Layer == NULL);
	wxLogMessage("Found %d layer", m_Dataset->GetLayerCount());
	m_Layer = m_Dataset->GetLayer(0);
	if (m_Layer == NULL) {
		wxLogError("Unable to get layer 0, number of layer found : %d", 
				   m_Dataset->GetLayerCount());
		return false;		
	}
	return true;
}

wxFileName vrLayerVectorC2P::GetDisplayName() {
	wxFileName myDisplayName (wxEmptyString, wxEmptyString, "DIPS", wxEmptyString);
	return myDisplayName;
}


