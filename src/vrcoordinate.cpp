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



vrCoordinate::vrCoordinate(const vrCoordinate & source) {
	m_Viewer = source.m_Viewer;
	m_WndExtent = source.m_WndExtent;
	m_LayersExtent = source.m_LayersExtent;
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
	double dWndheight = myWndSizePx.GetHeight()  - vrCOORDINATE_MARGIN;
	
	// compute shape 
	double dpixelx = myLayerExtent.m_width / dWndwidth;
	double dpixely = myLayerExtent.m_height / dWndheight;
	double myDivFactor = 0.0;
	
	if (fabs(dpixelx) >= fabs(dpixely)) {
		myDivFactor = dpixelx;
	}
	else {
		myDivFactor = dpixely;
	}
	
	
	double myDivFactorX = fabs(myDivFactor);
	double myDivFactorY = fabs(myDivFactor);
	
	if (dpixelx <= 0) {
		myDivFactorX = myDivFactorX * -1;
	}
	
	if (dpixely <= 0) {
		myDivFactorY = myDivFactorY * -1;
	}
	
	
	
	m_WndExtent = wxRect2DDouble();
	m_WndExtent.m_width = myWndSizePx.GetWidth();
	m_WndExtent.m_height = myWndSizePx.GetHeight();
	
	// scale
	m_WndExtent.m_width *= myDivFactorX;
	m_WndExtent.m_height *= myDivFactorY;
	
	
	// center rect
	wxPoint2DDouble myCenter = myLayerExtent.GetCentre();
	m_WndExtent.SetCentre(myCenter);

	// TODO: Remove this 4 lines bellow, only used for debuging
	double dbottom = m_LayersExtent.GetBottom();
	double dTop = m_LayersExtent.GetTop();
	
	double ddbottom = m_WndExtent.GetBottom();
	double ddTop = m_WndExtent.GetTop();
	
	return true;
}


double vrCoordinate::GetPixelSize() {
	wxASSERT(m_Viewer);
	wxSize mySize = m_Viewer->GetSize();
	double myPxWidth = 1;
	
	if(m_WndExtent.m_width <= 0){
		wxLogError("Extent not defined, computing pixel size not possible");
		return wxNOT_FOUND;
	}
	
	double myPixelSize = m_WndExtent.m_width / myPxWidth;
	if (myPixelSize <= 0) {
		wxLogError("Error computing pixel size. It is bellow 0 : %.f", myPixelSize);
		return wxNOT_FOUND;
	}
	
	return myPixelSize;
}

