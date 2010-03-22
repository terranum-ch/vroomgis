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



OGRGeometry * vrLayerVector::GetGeometry(long fid) {
	
	if (IsOK() == false){
		return NULL;
	}
		
	OGRFeature * myFeature = m_Layer->GetFeature(fid);
	
	if (myFeature == NULL) {
		wxLogError("Unable to get feature with fid %d", fid);
		return NULL;
	}
	
	OGRGeometry * myGeom = myFeature->GetGeometryRef();
	OGRFeature::DestroyFeature(myFeature);
	return myGeom;
}

OGRGeometry * vrLayerVector::GetNextGeometry(long & oid, bool restart) {
	if (IsOK() == false) {
		return NULL;
	}
	
	if (restart == true) {
		m_Layer->ResetReading();
	}
	
	oid = wxNOT_FOUND;
	
	OGRFeature * myFeature = m_Layer->GetNextFeature();
	if (myFeature == NULL) {
		return NULL;
	}
	
	oid = myFeature->GetFID();
	OGRGeometry * myGeom = myFeature->GetGeometryRef();
	OGRFeature::DestroyFeature(myFeature);	
	
	return myGeom;
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
	
	
	// spatial filter
	m_Layer->SetSpatialFilterRect(coord.GetLeft(), coord.GetBottom(),
								  coord.GetRight(), coord.GetTop());
	m_Layer->ResetReading();
	
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
				lFeatureDrawed);
	return true;
}


