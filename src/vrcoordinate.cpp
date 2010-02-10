/***************************************************************************
 vrcoordinate.cpp

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

#include "vrcoordinate.h"
#include "vrviewerdisplay.h"

vrCoordinate::vrCoordinate(vrViewerDisplay * viewer) {
	wxASSERT(viewer);
	m_Viewer = viewer;
}

vrCoordinate::~vrCoordinate() {
}


wxRect2DDouble vrCoordinate::GetExtent() {
	return m_WndExtent;
}



void vrCoordinate::ClearLayersExtent() {
	m_LayersExtent = wxRect2DDouble();
}

void vrCoordinate::AddLayersExtent(const wxRect2DDouble & rect) {
	
	if (m_LayersExtent.IsEmpty()==true) {
		m_LayersExtent = rect;
		return;
	}
	
	wxRect2DDouble myTempRect = m_LayersExtent;
	wxRect2DDouble::Union(myTempRect, rect, &m_LayersExtent);
	
}


bool vrCoordinate::ComputeFullExtent() {
	
	if (m_LayersExtent.m_width == 0.0 || m_LayersExtent.m_height == 0.0) {
		wxLogError("Extent for layers not specified, unable to compute full extent");
		return false;
	}
	
	wxASSERT(m_Viewer);
	wxSize myWndSizePx = m_Viewer->GetSize();
	wxRect2DDouble myLayerExtent = m_LayersExtent;
	
	double dWndwidth = myWndSizePx.GetWidth() - vrCOORDINATE_MARGIN;
	double dWndheight = myWndSizePx.GetHeight() - vrCOORDINATE_MARGIN;
	
	// compute shape 
	double dpixelx = myLayerExtent.m_width / dWndwidth;
	double dpixely = myLayerExtent.m_height / dWndheight;
	double myDivFactor = 0.0;
	
	if (abs(dpixelx) >= abs(dpixely)) {
		myDivFactor = dpixelx;
	}
	else {
		myDivFactor = dpixely;
	}
	
	m_WndExtent = wxRect2DDouble();
	m_WndExtent.m_width = myWndSizePx.GetWidth();
	m_WndExtent.m_height = myWndSizePx.GetHeight();
	
	m_WndExtent.Scale(myDivFactor);
	
	
	// center rect
	wxPoint2DDouble myCenter = myLayerExtent.GetCentre();
	m_WndExtent.SetCentre(myCenter);

	return true;
}

