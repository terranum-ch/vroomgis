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






vrLayerVectorOGR::vrLayerVectorOGR() {
	m_DriverType = vrDRIVER_VECTOR_SHP;
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


bool vrLayerVectorOGR::GetExtent(wxRect2DDouble & rect) {
	if (m_Layer == NULL) {
		wxLogError("Layer isn't inited");
		return false;
	}
	
	wxASSERT(m_Layer);
	rect = wxRect2DDouble();
	OGREnvelope myEnveloppe;
	if (m_Layer->GetExtent(&myEnveloppe, true)==OGRERR_FAILURE) {
		wxLogError("Unable to compute extent for layer %s", m_FileName.GetFullName());
		return false;
	}
	
	rect.SetLeftBottom(wxPoint2DDouble(myEnveloppe.MinX, myEnveloppe.MinY));
	rect.SetRightTop(wxPoint2DDouble(myEnveloppe.MaxX, myEnveloppe.MaxY));
	return true;
}




bool vrLayerVectorOGR::GetData(wxImage * bmp, const wxRect2DDouble & coord,
							   const vrRender * render, const vrLabel * label) {
	wxASSERT(m_Layer);
	
	
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
	
	// FIXME: THIS IS TEMP CODE FOR DRAWING LINE
	dc.Clear();
	dc.SetBrush(*wxWHITE_BRUSH);
	dc.SetPen(*wxWHITE_PEN);
	dc.DrawRectangle(0,0,myBmp.GetWidth(), myBmp.GetHeight());
	
	dc.SetPen(*wxRED_PEN);
	dc.DrawLine(0,0, myBmp.GetWidth(), myBmp.GetHeight());

	dc.SelectObject(wxNullBitmap);
	*bmp = myBmp.ConvertToImage();
	
	
	// FIXME: Add transparency should go somewhere else
	/*unsigned char * alphachar = NULL;
	int translucencypercent = 50;
	unsigned int myimglen = bmp->GetSize().GetWidth() * bmp->GetSize().GetHeight();
	alphachn= (unsigned char*)CPLMalloc(myimglen);
	if (alphachar == NULL)
	{
		wxLogError(_T("Error creating translucency"));
		return false;
	}
	
	// convert percent to 0-255 and invert 
	int myTransValue = translucencypercent * 255 / 100;
	myTransValue = 255 - myTransValue;
	
	
	for (unsigned int i = 0; i < myimglen;i ++)
	{
		*(alphachar + i) =  (char) myTransValue;
	}
	bmp->SetAlpha(alphachar);*/
	
	
	
	wxLogMessage("%s : %d features drawed",GetName().GetFullName(),
				lFeatureDrawed);
	return true;
}


