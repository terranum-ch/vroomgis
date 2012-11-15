/***************************************************************************
				vrrenderer.cpp
                    
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

#include "vrrenderer.h"
#include "vrlayer.h"
#include "vrrender.h"
#include "vrrendervectorc2p.h"
#include "vrrendercoltop.h"
#include "vrlabel.h"
#include "vrlayervector.h"
#include "vrlayervectorc2p.h"


bool vrRenderer::_IsCorrectRender() {
	wxASSERT(m_Layer);
	if (m_Render == NULL){
		return false;
	}
	
	bool bReturn = false;
	switch (m_Render->GetType())
	{
		case vrRENDER_VECTOR:
			if (m_Layer->GetType() == vrDRIVER_VECTOR_SHP) { bReturn = true; break;}
			if (m_Layer->GetType() == vrDRIVER_VECTOR_MEMORY) { bReturn = true; break;}
			break;
			
		case vrRENDER_RASTER:
			if (m_Layer->GetType() == vrDRIVER_RASTER_TIFF) { bReturn = true; break;}
			if (m_Layer->GetType() == vrDRIVER_RASTER_JPEG) { bReturn = true; break;}
			if (m_Layer->GetType() == vrDRIVER_RASTER_ESRIGRID) { bReturn = true; break;}
			if (m_Layer->GetType() == vrDRIVER_RASTER_EASC) { bReturn = true; break;}
			if (m_Layer->GetType() == vrDRIVER_RASTER_SGRD7) { bReturn = true; break;}
			break;
			
		case vrRENDER_RASTER_C2D:
			if (m_Layer->GetType() == vrDRIVER_RASTER_C2D) { bReturn = true;}
			break;
			
		case vrRENDER_VECTOR_C2P_DIPS:
			if (m_Layer->GetType() == vrDRIVER_VECTOR_C2P) {bReturn = true;}
			break;
			
		default:
			break;
	}
	
	return bReturn;
}


vrRenderer::vrRenderer(vrLayer * layer, vrRender * render, vrLabel * label) {
	wxASSERT(layer);
	m_Layer = layer;
	
	// setting visible status on true
	SetVisible(true);
	
	// create default render and label if null
	if (render == NULL){
		switch (m_Layer->GetType()){
			case vrDRIVER_VECTOR_SHP:
			case vrDRIVER_VECTOR_MEMORY:
				m_Render = new vrRenderVector();
            {
                OGRwkbGeometryType myGeomType = ((vrLayerVector*) GetLayer())->GetGeometryType();
                if (myGeomType == wkbPoint || myGeomType == wkbPoint25D) {
                    ((vrRenderVector*)m_Render)->SetSize(vrRENDERER_VECTOR_POINT_DEFAULT_SIZE);
                }
            }
				break;
				
			case vrDRIVER_RASTER_TIFF:
			case vrDRIVER_RASTER_JPEG:
			case vrDRIVER_RASTER_ESRIGRID:
			case vrDRIVER_RASTER_EASC:
			case vrDRIVER_RASTER_SGRD7:
				m_Render = new vrRenderRaster();
				break;
				
			case vrDRIVER_RASTER_C2D:
				m_Render = new vrRenderRasterColtop();
				break;
				
			case vrDRIVER_VECTOR_C2P:
            {
                CT_LAYER_TYPE myLayerType =((vrLayerVectorC2P*) GetLayer())->GetActiveLayerType();
                if (myLayerType == CT_DIP){
                    m_Render = new vrRenderVectorC2PDips();
                } else if (myLayerType == CT_POLYGON){
                    m_Render = new vrRenderVector();
                }
                else{
                    wxLogError(_("Unsupported geometry type: %d"), myLayerType);
                }
            }
               break;
                
                
			default:
				m_Render = new vrRender();
				break;
		}
	}
	else {
		m_Render = render;
		// assert correct render is passed.
		bool bCorrectRender = _IsCorrectRender();
		wxASSERT(bCorrectRender);
	}
	
	
	if (label == NULL) {
		m_Label = new vrLabel();
	}
	else {
		m_Label = label;
	}
}



vrRenderer::~vrRenderer() {
	wxASSERT(m_Render);
	wxASSERT(m_Label);
	wxDELETE(m_Render);
	wxDELETE(m_Label);
}



bool vrRenderer::GetBitmapDataThread(wxImage * bmp, const vrRealRect & coord,  double pxsize, long & vectorcount) {
	wxASSERT(bmp);
	wxASSERT(GetVisible());
	
	// TODO: GetRender and GetLabel aren't working in thread mode
	// we should create copy and send them to the get layer function
	bool bValue = GetLayer()->GetDataThread(bmp, coord, pxsize, GetRender(), GetLabel());
	
	vectorcount = 0;
	if (GetLayer()->GetType() != vrDRIVER_UNKNOWN && GetLayer()->GetType() < vrDRIVER_RASTER_TIFF){
		vectorcount =  ((vrLayerVector*)GetLayer())->GetObjectDrawn();
	}
	return bValue;
}


bool vrRenderer::GetBitmapData(wxBitmap * bmp, const vrRealRect & coord, double pxsize, long & vectorcount) {
	wxASSERT(bmp);
	wxASSERT(GetVisible());

	bool bValue = GetLayer()->GetData(bmp, coord, pxsize, GetRender(), GetLabel());
	vectorcount = 0;
	if (GetLayer()->GetType() != vrDRIVER_UNKNOWN && GetLayer()->GetType() < vrDRIVER_RASTER_TIFF){
		vectorcount =  ((vrLayerVector*)GetLayer())->GetObjectDrawn();
	}
	return bValue;
}


void vrRenderer::SetRender(vrRender * value) {
	m_Render = value;
}

void vrRenderer::SetLabel(vrLabel * value) {
	m_Label = value;
}


void vrRenderer::SetVisible(bool value) {
	m_Visible = value;
}


#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(vrArrayRenderer);



