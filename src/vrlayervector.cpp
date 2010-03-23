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



bool vrLayerVectorOGR::_DrawLines(wxMemoryDC * mdc, const wxRect2DDouble & coord,
								  const vrRender * render, const vrLabel * label, double pxsize) {
	wxASSERT(mdc);
	wxStopWatch sw;
	// creating pen
	// TODO: Update this code for reflecting the vrRender code
	wxPen myPen (wxColour(0,0,255), 2); 
	mdc->SetPen(myPen);
	
	
	// iterating and drawing geometries
	OGRLineString * myGeom = NULL;
	long iCount = 0;
	while (1) {
		OGRFeature * myFeat = GetNextFeature(false);
		if (myFeat == NULL) {
			break;
		}
		iCount++;
		
		myGeom = NULL;
		myGeom = (OGRLineString*) myFeat->GetGeometryRef();
		wxASSERT(myGeom);
		
		
		int iNumVertex = myGeom->getNumPoints();
		wxASSERT(iNumVertex >= 2); // line cannot exists with only one vertex
		
		
		wxPoint * myPts = new wxPoint[iNumVertex];
		for (int i = 0; i< iNumVertex; i++) {
		myPts[i] = _GetPointFromReal(wxPoint2DDouble(myGeom->getX(i),
													 myGeom->getY(i)),
									 coord.GetLeftTop(),
									 pxsize);
		}
		mdc->DrawLines(iNumVertex, myPts);
		delete [] myPts;
		
		// intersection ???
				
		OGRFeature::DestroyFeature(myFeat);
		myFeat = NULL;
	}
	
	if (iCount == 0) {
		wxLogWarning("No data drawn.");
	}
	wxLogMessage("%d lines drawed in %ldms", iCount, sw.Time());
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
	wxBitmap myBmp(bmp->GetSize());
	wxASSERT(myBmp.IsOk());
	wxMemoryDC dc;
	dc.SelectObject(myBmp);
	const wxColour myTransparentColour = vrRenderVector::GetBackgroundMaskColour();
	dc.SetBrush(wxBrush(myTransparentColour));
	dc.Clear();
	dc.SetPen(wxPen (myTransparentColour));
	dc.DrawRectangle(0,0,myBmp.GetWidth(), myBmp.GetHeight());
	
	
	// draw
	bool bReturn = true;
	OGRwkbGeometryType myGeomType = GetGeometryType();
	
	wxLogMessage("Preparing data took %ldms", sw.Time());
		
	switch (myGeomType) {
		case wkbLineString:
			bReturn = _DrawLines(&dc, coord, render, label, pxsize);
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
	
	
	dc.SelectObject(wxNullBitmap);
	*bmp = myBmp.ConvertToImage();
	bmp->SetMaskColour(myTransparentColour.Red(),
					   myTransparentColour.Green(),
					   myTransparentColour.Blue());
	//bool bmask = bmp->HasMask();
	
	
	
	/*
	
	// drawing
	wxBitmap myBmp(bmp->GetSize());
	wxMemoryDC dc (myBmp);
	
	OGRFeature * poFeature;
	long lFeatureDrawed = 0;
	while ((poFeature = m_Layer->GetNextFeature()) != NULL) {
		lFeatureDrawed++;
		
		
		OGRFeature::DestroyFeature(poFeature);
	}
	
	// get transparent colour
	const wxColour myTransparentColour = vrRenderVector::GetBackgroundMaskColour();
	
	
	// FIXME: THIS IS TEMP CODE FOR DRAWING LINE
	
	//dc.SetBrush(wxBrush(myTransparentColour));
	dc.SetBrush(wxBrush(myTransparentColour));
	dc.Clear();
	dc.SetPen(wxPen (myTransparentColour));
	dc.DrawRectangle(0,0,myBmp.GetWidth(), myBmp.GetHeight());
	
	dc.SetPen(*wxRED_PEN);
	dc.DrawLine(0,0, myBmp.GetWidth(), myBmp.GetHeight());

	dc.SelectObject(wxNullBitmap);
	*bmp = myBmp.ConvertToImage();
	bmp->SetMaskColour(myTransparentColour.Red(),
					   myTransparentColour.Green(),
					   myTransparentColour.Blue());
	bool bmask = bmp->HasMask();
	
	
	
	
	
	
	wxLogMessage("%s : %d features drawed",GetName().GetFullName(),
				lFeatureDrawed);*/
	return true;
}






