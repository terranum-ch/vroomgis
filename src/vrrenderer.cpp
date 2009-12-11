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
#include "vrlabel.h"


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
			break;
			
		case vrRENDER_RASTER_C2D:
			if (m_Layer->GetType() == vrDRIVER_RASTER_C2D) { bReturn = true;}
			break;
			
		default:
			break;
	}
	
	return bReturn;
}


vrRenderer::vrRenderer(vrLayer * layer, vrRender * render, vrLabel * label) {
	wxASSERT(layer);
	m_Layer = layer;
	
	// create default render and label if null
	if (render == NULL){
		switch (m_Layer->GetType()){
			case vrDRIVER_VECTOR_SHP:
			case vrDRIVER_VECTOR_MEMORY:
				m_Render = new vrRenderVector();
				break;
				
			case vrDRIVER_RASTER_TIFF:
			case vrDRIVER_RASTER_JPEG:
			case vrDRIVER_RASTER_ESRIGRID:
				m_Render = new vrRenderRaster();
				break;
				
			case vrDRIVER_RASTER_C2D:
				m_Render = new vrRenderRasterColtop();
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
	delete m_Render;
	delete m_Label;
}

void vrRenderer::SetRender(vrRender * value) {
	m_Render = value;
}

void vrRenderer::SetLabel(vrLabel * value) {
	m_Label = value;
}


#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(vrArrayRenderer);



