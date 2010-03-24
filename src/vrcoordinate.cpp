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


bool vrCoordinate::_ComputePixelSize() {

	wxASSERT(m_Viewer);
	m_PxSize = wxNOT_FOUND;
	double myPxWidth = m_Viewer->GetSize().GetWidth();
	
	if(m_WndExtent.m_width <= 0){
		wxLogError("Extent not defined, computing pixel size not possible");
		return false;
	}
	
	double myPixelSize = m_WndExtent.m_width / myPxWidth;
	if (myPixelSize <= 0) {
		wxLogError("Error computing pixel size. It is bellow 0 : %.f", myPixelSize);
		return false;
	}
	
	m_PxSize = myPixelSize;
	return true;
}


vrCoordinate::vrCoordinate(vrViewerDisplay * viewer) {
	wxASSERT(viewer);
	m_Viewer = viewer;
	m_PxSize = wxNOT_FOUND;
}



vrCoordinate::vrCoordinate(const vrCoordinate & source) {
	m_Viewer = source.m_Viewer;
	m_WndExtent = source.m_WndExtent;
	m_LayersExtent = source.m_LayersExtent;
	m_PxSize = source.m_PxSize;
}



vrCoordinate::~vrCoordinate() {
}


vrRealRect vrCoordinate::GetExtent() {
	return m_WndExtent;
}


bool vrCoordinate::UpdateExtent() {
	wxASSERT(m_Viewer);
	if (m_WndExtent.IsOk() == false) {
		wxLogError("Window real extent isn't defined, updating not possible");
		return false;
	}
	
	// ensure sign is kept when size is updated
	double xpositive = 1, ypositive =1; 
	if (m_WndExtent.m_width < 0){
		xpositive = -1;
	}	
	
	if (m_WndExtent.m_height < 0) {
		ypositive = -1;
	}

	wxSize myViewSize = m_Viewer->GetSize();
	wxASSERT(myViewSize.IsFullySpecified());
	
	m_WndExtent.m_width = myViewSize.GetWidth() * fabs(GetPixelSize()) * xpositive;
	m_WndExtent.m_height = myViewSize.GetHeight() * fabs(GetPixelSize()) * ypositive;
	return true;
}


void vrCoordinate::ClearLayersExtent() {
	m_LayersExtent = vrRealRect();
}

void vrCoordinate::AddLayersExtent(const vrRealRect & rect) {
	
	if (m_LayersExtent.IsEmpty()==true) {
		m_LayersExtent = rect;
		return;
	}
	
	vrRealRect myTempRect (m_LayersExtent);
	vrRealRect::Union(myTempRect, rect, &m_LayersExtent);
	
}


bool vrCoordinate::ComputeFullExtent() {
	
	if (m_LayersExtent.IsOk() == false) {
		wxLogError("Extent for layers not specified, unable to compute full extent");
		return false;
	}
	
	wxASSERT(m_Viewer);
	wxSize myWndSizePx = m_Viewer->GetSize();
	vrRealRect myLayerExtent (m_LayersExtent);
	
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
	
	
	
	m_WndExtent = vrRealRect();
	m_WndExtent.m_width = myWndSizePx.GetWidth();
	m_WndExtent.m_height = myWndSizePx.GetHeight();
	
	// scale
	m_WndExtent.m_width *= myDivFactorX;
	m_WndExtent.m_height *= myDivFactorY;
	
	
	// center rect
	wxPoint2DDouble myCenter = myLayerExtent.GetCentre();
	m_WndExtent.SetCentre(myCenter);

	return true;
}


double vrCoordinate::GetPixelSize() {
	
	if (m_PxSize == wxNOT_FOUND) {
		if (_ComputePixelSize() == false) {
			wxLogError("Error computing pixel size");
			return wxNOT_FOUND;
		}
	}
	
	return m_PxSize;
}


bool vrCoordinate::IsOk() {
	wxASSERT(m_Viewer);
	
	return m_WndExtent.IsOk();
}


