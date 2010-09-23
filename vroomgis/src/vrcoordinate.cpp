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
	ClearPixelSize();
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
	ClearPixelSize();
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



void vrCoordinate::SetExtent(const vrRealRect & extent) {
	if (extent.IsOk() == false) {
		wxLogError("specified extent isn't valid");
		return;
	}
	ClearPixelSize();
	m_WndExtent = extent;
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


vrRealRect vrCoordinate::GetRectFitted(const vrRealRect & originalrect) {
	
	if (originalrect.IsOk() == false) {
		wxLogError("Specified extent isn't Correct, unable to compute fitted rectangle");
		return originalrect;
	}
	wxASSERT(m_WndExtent.IsOk()==true);
	
	double xfactor = fabs(m_WndExtent.m_width / originalrect.m_width);
	double yfactor = fabs(m_WndExtent.m_height / originalrect.m_height);
	
	double ratiofactor = xfactor;
	if (yfactor > xfactor) {
		ratiofactor = yfactor;
	}
	
	vrRealRect myFittedRect;
	myFittedRect.m_width = fabs(m_WndExtent.m_width / ratiofactor);
	if (m_WndExtent.m_width < 0) {
		wxASSERT(originalrect.m_width < 0);
		myFittedRect.m_width *= -1;
	}
	myFittedRect.m_height = fabs(m_WndExtent.m_height / ratiofactor);
	if (m_WndExtent.m_height < 0) {
		wxASSERT(originalrect.m_height < 0);
		myFittedRect.m_height *= -1;
	}
	
	myFittedRect.SetCentre(originalrect.GetCentre());
	return myFittedRect;
}



void vrCoordinate::ClearLayersExtent() {
	m_LayersExtent = vrRealRect();
}



void vrCoordinate::ClearPixelSize() {
	m_PxSize = wxNOT_FOUND;
}



void vrCoordinate::AddLayersExtent(const vrRealRect & rect) {
	
	if (m_LayersExtent.IsOk()==false) {
		m_LayersExtent = rect;
		return;
	}
	
	//vrRealRect myTempRect (m_LayersExtent);
	vrRealRect myResult = m_LayersExtent.Union(rect);
	if (myResult.IsOk() == false) {
		wxLogError("Computing rectangle union failed, result invalid");
		return;
	}
	
	m_LayersExtent = myResult;
	
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
		myDivFactorX = myDivFactorX * -1.0;
	}
	
	if (dpixely <= 0) {
		myDivFactorY = myDivFactorY * -1.0;
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


bool vrCoordinate::ConvertFromPixels(const wxRect & in, vrRealRect & out) {
	out = vrRealRect(0,0,0,0);
	if (in.IsEmpty() == true) {
		wxLogError("Unable to convert empty rectangle");
		return false;
	}
	
	wxPoint2DDouble myTopLeft, myRightBottom;
	if (ConvertFromPixels(in.GetLeftTop(), myTopLeft) == false) {
		return false;
	}
	
	if (ConvertFromPixels(in.GetBottomRight(), myRightBottom)==false) {
		return false;
	}
	
	out.SetLeftTop(myTopLeft);
	out.SetRightBottom(myRightBottom);
	return true;
}



bool vrCoordinate::ConvertFromPixels(const wxPoint & in, wxPoint2DDouble & out) {
	out = wxPoint2DDouble(0,0);
	if (in == wxDefaultPosition) {
		wxLogError("Unable to convert point (%d, %d)", in.x, in.y);
		return false;
	}
	wxASSERT(m_WndExtent.IsOk());
	
	wxPoint2DDouble myLeftTop = m_WndExtent.GetLeftTop();
	if (m_WndExtent.m_width > 0) {
		out.m_x = myLeftTop.m_x + (in.x * GetPixelSize());
	}else {
		out.m_x = myLeftTop.m_x - (in.x * GetPixelSize());
	}

	if (m_WndExtent.m_height > 0) {
		out.m_y = myLeftTop.m_y + (in.y * GetPixelSize());
	}else {
		out.m_y = myLeftTop.m_y - (in.y * GetPixelSize());
	}
	return true;
}



bool vrCoordinate::ConvertToPixels (const wxPoint2DDouble & in, wxPoint & out){
	out = wxPoint(0,0);
	out.x = wxRound(abs((in.m_x - m_WndExtent.m_x)) / GetPixelSize());
	out.y = wxRound(abs((in.m_y - m_WndExtent.m_y)) / GetPixelSize());
	return true;
}


bool vrCoordinate::IsOk() {
	wxASSERT(m_Viewer);
	
	return m_WndExtent.IsOk();
}


