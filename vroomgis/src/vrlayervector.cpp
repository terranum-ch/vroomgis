/***************************************************************************
					vrlayervector.cpp
                   
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

//

#include "vrlayervector.h"
#include "vrrender.h"


wxPoint vrLayerVector::_GetPointFromReal(const wxPoint2DDouble & realpt, 
										 const wxPoint2DDouble & origin, double pxsize) {
	wxPoint myPt;
	myPt.x = (realpt.m_x - origin.m_x) / pxsize;
	myPt.y = (origin.m_y - realpt.m_y) / pxsize;
	return myPt;
}


vrLayerVector::vrLayerVector() {
	m_Dataset = NULL;
	m_Layer = NULL;
}

vrLayerVector::~vrLayerVector() {

}


bool vrLayerVector::IsOK() {
	if (m_Dataset == NULL){
		wxLogError("Dataset not inited");
		return false;
	}
	if (m_Layer == NULL)
	{
		wxLogError("Layer Not inited");
		return false;
	}
	return true;
}



OGRFeature * vrLayerVector::GetFeature(long fid) {
	
	if (IsOK() == false){
		return NULL;
	}
		
	OGRFeature * myFeature = m_Layer->GetFeature(fid);
	
	if (myFeature == NULL) {
		wxLogError("Unable to get feature with fid %d", fid);
		return NULL;
	}
	
	return myFeature;
}

OGRFeature * vrLayerVector::GetNextFeature(bool restart) {
	if (IsOK() == false) {
		return NULL;
	}
	
	if (restart == true) {
		m_Layer->ResetReading();
	}
	
	
	OGRFeature * myFeature = m_Layer->GetNextFeature();
	if (myFeature == NULL) {
		return NULL;
	}
	
	return myFeature;
}




OGRwkbGeometryType  vrLayerVector::GetGeometryType() {
	
	// geometry type exists, return it. 
	if (m_GeometryType != wkbUnknown) {
		return m_GeometryType;
	}
	
	// geometry type doesn't exists try to get it from feature 0;
	if (m_Layer == NULL) {
		wxLogError("Layer not inited, call open() first");
		return wkbUnknown;
	}
	
	OGRFeature * myFeature = m_Layer->GetFeature(0);
	
	if (myFeature == NULL) {
		wxLogError("Unable to get feature with fid %d", 0);
		return wkbUnknown;
	}
	
	m_GeometryType = myFeature->GetGeometryRef()->getGeometryType();
	OGRFeature::DestroyFeature(myFeature);
	return m_GeometryType;
}









vrLayerVectorOGR::vrLayerVectorOGR() {
	m_DriverType = vrDRIVER_VECTOR_SHP;
	m_GeometryType = wkbUnknown;
}

vrLayerVectorOGR::~vrLayerVectorOGR() {
	_Close();
}


bool vrLayerVectorOGR::Open(const wxFileName & filename, bool readwrite){
	// try to close if open
	_Close();
	wxASSERT(m_Dataset == NULL);
	
	m_FileName = filename;

	// open datasource
	m_Dataset = OGRSFDriverRegistrar::Open( filename.GetFullPath(), readwrite);
	wxLogMessage(filename.GetFullPath());
    if( m_Dataset == NULL ){
		wxLogError("Unable to open %s, maybe driver not registred - OGRRegisterAll()", filename.GetFullName());
		return false;
	}
	
	// try to get layer
	wxASSERT(m_Layer == NULL);
	m_Layer = m_Dataset->GetLayer(0);
	if (m_Layer == NULL){
		wxLogError("Unable to get layer 0, number of layer found : %d", 
				   m_Dataset->GetLayerCount());
		return false;
	}
		
	
	return true;
}



bool vrLayerVectorOGR::_DrawLines(wxGraphicsContext * gdc, const wxRect2DDouble & coord,
								  const vrRender * render, const vrLabel * label, double pxsize) {
	wxASSERT(gdc);
	wxStopWatch sw;
	// creating pen
	
	wxASSERT(render->GetType() == vrRENDER_VECTOR);
	vrRenderVector * myRender = (vrRenderVector*) render;
	
	wxPen myPen (myRender->GetColorPen(),
				 myRender->GetSize());
	gdc->SetPen(myPen);
	
	
	// iterating and drawing geometries
	OGRLineString * myGeom = NULL;
	long iCount = 0;
	while (1) {
		OGRFeature * myFeat = GetNextFeature(false);
		if (myFeat == NULL) {
			break;
		}
		
		myGeom = NULL;
		myGeom = (OGRLineString*) myFeat->GetGeometryRef();
		wxASSERT(myGeom);
		
		
		int iNumVertex = myGeom->getNumPoints();
		wxASSERT(iNumVertex >= 2); // line cannot exists with only one vertex
		
		// draw geometry
		wxGraphicsPath myPath = gdc->CreatePath();
		myPath.MoveToPoint(_GetPointFromReal(wxPoint2DDouble(myGeom->getX(0),
															 myGeom->getY(0)),
											 coord.GetLeftTop(),
											 pxsize));
		for (int i = 1; i< iNumVertex; i++) {
			myPath.AddLineToPoint(_GetPointFromReal(wxPoint2DDouble(myGeom->getX(i),
																	myGeom->getY(i)),
													coord.GetLeftTop(),
													pxsize));
		}
		double myWidth = 0, myHeight = 0;
		gdc->GetSize(&myWidth, &myHeight);
		wxRect2DDouble myWndRect (0,0,myWidth, myHeight);
		wxRect2DDouble myPathRect = myPath.GetBox();
		
		if(myPathRect.Intersects(myWndRect) == false){
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
		gdc->StrokePath(myPath);
		OGRFeature::DestroyFeature(myFeat);
		myFeat = NULL;
	}
	
	if (iCount <= 0) {
		wxLogWarning("No data drawn.");
	}
	wxLogMessage("%ld lines drawed in %ldms", iCount, sw.Time());
	return true;
}



bool vrLayerVectorOGR::_DrawPoints(wxGraphicsContext * gdc, const wxRect2DDouble & coord,
								   const vrRender * render, const vrLabel * label, double pxsize) {
	wxASSERT(gdc);
	wxStopWatch sw;
	// creating pen
	
	wxASSERT(render->GetType() == vrRENDER_VECTOR);
	vrRenderVector * myRender = (vrRenderVector*) render;
	
	wxPen myPen (myRender->GetColorPen(),
				 myRender->GetSize());
	gdc->SetPen(myPen);
	
	
	// iterating and drawing geometries
	OGRPoint * myGeom = NULL;
	long iCount = 0;
	while (1) {
		OGRFeature * myFeat = GetNextFeature(false);
		if (myFeat == NULL) {
			break;
		}
		
		myGeom = NULL;
		myGeom = (OGRPoint*) myFeat->GetGeometryRef();
		wxASSERT(myGeom);
		
		
		// convert and check intersection
		wxPoint myPt = _GetPointFromReal(wxPoint2DDouble(myGeom->getX(),myGeom->getY()),
										 coord.GetLeftTop(),
										 pxsize);
		double myWidth = 0, myHeight = 0;
		gdc->GetSize(&myWidth, &myHeight);
		wxRect myWndRect(0,0,myWidth, myHeight);
		if (myWndRect.Contains(myPt)==false) {
			OGRFeature::DestroyFeature(myFeat);
			myFeat = NULL;
			continue;
		}
		iCount++;

#ifdef __WXMSW__
		gdc->StrokeLine (myPt.x, myPt.y, myPt.x + 0.1, myPt.y + 0.1);
#else
		gdc->StrokeLine (myPt.x, myPt.y, myPt.x, myPt.y);
#endif
		
		OGRFeature::DestroyFeature(myFeat);
		myFeat = NULL;
	}
	
	if (iCount <= 0) {
		wxLogWarning("No data drawn.");
	}
	wxLogMessage("%d points drawed in %ldms", iCount, sw.Time());
	return true;
}




bool vrLayerVectorOGR::_Close() {
	m_GeometryType = wkbUnknown;
	if(m_Dataset==NULL){
		return false;
	}
	
	wxLogMessage("Closing data NOW");
	OGRDataSource::DestroyDataSource( m_Dataset );
	m_Dataset = NULL;
	// layer destroyed with the data source
	m_Layer = NULL;
	return true;
}



bool vrLayerVectorOGR::_DrawPolygons(wxGraphicsContext * gdc, const wxRect2DDouble & coord,
									 const vrRender * render, const vrLabel * label, double pxsize) {
	wxASSERT(gdc);
	wxStopWatch sw;
	
	wxASSERT(render->GetType() == vrRENDER_VECTOR);
	vrRenderVector * myRender = (vrRenderVector*) render;
	
	// creating brush and pen
	wxPen myPen (myRender->GetColorPen(),myRender->GetSize());
	// TODO: Add support for different brush style in vrRender
	wxBrush myBrush (myRender->GetColorBrush(), wxBRUSHSTYLE_SOLID); 
	gdc->SetPen(myPen);
	gdc->SetBrush(myBrush);
	
	
	// iterating and drawing geometries
	OGRPolygon * myGeom = NULL;
	long iCount = 0;
	while (1) {
		OGRFeature * myFeat = GetNextFeature(false);
		if (myFeat == NULL) {
			break;
		}
		//iCount++;
		
		myGeom = NULL;
		myGeom = (OGRPolygon*) myFeat->GetGeometryRef();
		wxASSERT(myGeom);
		
		
		int iNumRing = myGeom->getNumInteriorRings() + 1;
		wxASSERT(iNumRing >= 1); // Polygon should have at least one ring
		
		// create path for polygon
		wxGraphicsPath myPath = gdc->CreatePath();
		for (int i = 0; i < iNumRing; i++) {
			wxGraphicsPath myPolyPath = gdc->CreatePath();
			OGRLineString * myRing  = NULL;
			if (i == 0) {
				myRing = myGeom->getExteriorRing();
			}
			else {
				myRing = myGeom->getInteriorRing(i -1);
			}
			wxASSERT(myRing);
			int iNumVertex = myRing->getNumPoints();
			wxASSERT(iNumVertex > 1);
			myPolyPath.MoveToPoint(_GetPointFromReal(wxPoint2DDouble(myRing->getX(0),
																	 myRing->getY(0)),
													 coord.GetLeftTop(),
													 pxsize));
			for (int v = 0; v < iNumVertex; v++) {
				myPolyPath.AddLineToPoint(_GetPointFromReal(wxPoint2DDouble(myRing->getX(v),
																		 myRing->getY(v)),
														 coord.GetLeftTop(),
														 pxsize));
			}
			myPolyPath.CloseSubpath();
			myPath.AddPath(myPolyPath);
		}
		
		// check intersection and minimum size
		double myWidth = 0, myHeight = 0;
		gdc->GetSize(&myWidth, &myHeight);
		wxRect2DDouble myWndRect (0,0,myWidth, myHeight);
		wxRect2DDouble myPathRect = myPath.GetBox();
		if(myPathRect.Intersects(myWndRect)==false){
			OGRFeature::DestroyFeature(myFeat);
			myFeat = NULL;
			continue;
		}
		
		if (myPathRect.GetSize().x < 1 && myPathRect.GetSize().y < 1){
			OGRFeature::DestroyFeature(myFeat);
			myFeat = NULL;
			continue;
		}
		
		// draw path
		iCount++;
		gdc->DrawPath(myPath);
		OGRFeature::DestroyFeature(myFeat);
		myFeat = NULL;
	}
	
	if (iCount <= 0) {
		wxLogWarning("No data drawn.");
	}
	wxLogMessage("%d Polygon drawed in %ldms", iCount, sw.Time());
	return true;
}





bool vrLayerVectorOGR::Create(const wxFileName & filename) {
	return false;
}


bool vrLayerVectorOGR::GetExtent(vrRealRect & rect) {
	if (m_Layer == NULL) {
		wxLogError("Layer isn't inited");
		return false;
	}
	
	wxASSERT(m_Layer);
	rect = vrRealRect();
	OGREnvelope myEnveloppe;
	if (m_Layer->GetExtent(&myEnveloppe, true)==OGRERR_FAILURE) {
		wxLogError("Unable to compute extent for layer %s", m_FileName.GetFullName());
		return false;
	}
	
	rect.SetLeftBottom(wxPoint2DDouble(myEnveloppe.MinX, myEnveloppe.MinY));
	rect.SetRightTop(wxPoint2DDouble(myEnveloppe.MaxX, myEnveloppe.MaxY));
	return true;
}



bool vrLayerVectorOGR::GetData(wxImage * bmp, const vrRealRect & coord,  double pxsize,
							   const vrRender * render, const vrLabel * label) {
	wxASSERT(m_Layer);
	wxASSERT(render);
	
	wxStopWatch sw;
	// setting spatial filter
	m_Layer->SetSpatialFilterRect(coord.GetLeft(), coord.GetBottom(),
								  coord.GetRight(), coord.GetTop());
	m_Layer->ResetReading();
	
	
	
	// prepare bitmap for drawing
	if (bmp->HasAlpha() == false){
		bmp->InitAlpha();
		wxLogMessage("Initing alpha");
	}

	// adding transparency
	char myBackgroundTransparency = 0;
	unsigned char * alphachar = NULL;
	unsigned int myimglen = bmp->GetWidth() * bmp->GetHeight();
	alphachar= (unsigned char*)malloc(myimglen);
	if (alphachar == NULL)
	{
		wxLogError(_T("Error creating transparency bitmap"));
		return false;
	}
	
	
	for ( unsigned int i = 0; i< myimglen; i++) {
		*(alphachar + i) =  myBackgroundTransparency;
	}
	bmp->SetAlpha(alphachar, false);
	

	wxBitmap myBmp(*bmp);
	wxASSERT(myBmp.IsOk());

	// empty bitmap
	/*wxSize myBmpSize = bmp->GetSize();
	wxMemoryDC myDC;
	myDC.SelectObject(myBmp);
	wxGraphicsContext * myPgdc = wxGraphicsContext::Create(myDC);
	myPgdc->SetBrush(wxBrush(wxColour(255,0,0,30)));
	myPgdc->SetPen(wxPen(wxColour(255,0,0,30)));
	wxGraphicsPath path = myPgdc->CreatePath();
	path.AddRectangle(0,0,myBmpSize.x,myBmpSize.y);
	myPgdc->DrawPath(path);
	wxDELETE(myPgdc);
	myDC.SelectObject(wxNullBitmap);*/

	// draw
	wxMemoryDC dc;
	dc.SelectObject(myBmp);
	wxGraphicsContext * pgdc = wxGraphicsContext::Create(dc);

	
	bool bReturn = true;
	OGRwkbGeometryType myGeomType = GetGeometryType();
	
	wxLogMessage("Preparing data took %ldms", sw.Time());
		
	switch (myGeomType) {
		case wkbLineString:
			bReturn = _DrawLines(pgdc, coord, render, label, pxsize);
			break;
			
		case wkbPoint:
			bReturn = _DrawPoints(pgdc, coord, render, label, pxsize);
			break;
			
		case wkbPolygon:
			bReturn = _DrawPolygons(pgdc, coord, render, label, pxsize);
			break;
		
		default: // unsupported case
			wxLogError("Geometry type of %s isn't supported (%d)",
					   m_FileName.GetFullName(), myGeomType);
			bReturn = false;
			break;
	}
	
	if (bReturn == false) {
		return false;
	}
	
	wxDELETE(pgdc);
	dc.SelectObject(wxNullBitmap);
	*bmp = myBmp.ConvertToImage();

	return true;
}





