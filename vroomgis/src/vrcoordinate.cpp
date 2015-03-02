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

	wxASSERT(m_viewer);
	ClearPixelSize();
	double myPxWidth = m_viewer->GetSize().GetWidth();
	
	if(m_wndExtent.m_width <= 0){
		wxLogError("Extent not defined, computing pixel size not possible");
		return false;
	}
	
	double myPixelSize = m_wndExtent.m_width / myPxWidth;
	if (myPixelSize <= 0) {
		wxLogError("Error computing pixel size. It is bellow 0 : %.f", myPixelSize);
		return false;
	}
	
	m_pxSize = myPixelSize;
	return true;
}


vrCoordinate::vrCoordinate(vrViewerDisplay * viewer) {
	wxASSERT(viewer);
	m_viewer = viewer;
	ClearPixelSize();
}



vrCoordinate::vrCoordinate(const vrCoordinate & source) {
	m_viewer = source.m_viewer;
	m_wndExtent = source.m_wndExtent;
	m_layersExtent = source.m_layersExtent;
	m_pxSize = source.m_pxSize;
}



vrCoordinate::~vrCoordinate() {
}



vrRealRect vrCoordinate::GetExtent() {
	return m_wndExtent;
}



void vrCoordinate::SetExtent(const vrRealRect & extent) {
	if (extent.IsOk() == false) {
		wxLogError("specified extent isn't valid");
		return;
	}
	ClearPixelSize();
	m_wndExtent = extent;
}


bool vrCoordinate::UpdateExtent() {
	wxASSERT(m_viewer);
	if (m_wndExtent.IsOk() == false) {
		wxLogError("Window real extent isn't defined, updating not possible");
		return false;
	}
	
	// ensure sign is kept when size is updated
	double xpositive = 1, ypositive =1; 
	if (m_wndExtent.m_width < 0){
		xpositive = -1;
	}	
	
	if (m_wndExtent.m_height < 0) {
		ypositive = -1;
	}

	wxSize myViewSize = m_viewer->GetSize();
	wxASSERT(myViewSize.IsFullySpecified());
	
	m_wndExtent.m_width = myViewSize.GetWidth() * fabs(GetPixelSize()) * xpositive;
	m_wndExtent.m_height = myViewSize.GetHeight() * fabs(GetPixelSize()) * ypositive;
	return true;
}


vrRealRect vrCoordinate::GetRectFitted(const vrRealRect & originalrect) {
	
	if (originalrect.IsOk() == false) {
		wxLogError("Specified extent isn't Correct, unable to compute fitted rectangle");
		return originalrect;
	}
	wxASSERT(m_wndExtent.IsOk()==true);
	
	double xfactor = fabs(m_wndExtent.m_width / originalrect.m_width);
	double yfactor = fabs(m_wndExtent.m_height / originalrect.m_height);
	
	double ratiofactor = xfactor;
	if (yfactor < xfactor) {
		ratiofactor = yfactor;
	}
	
	vrRealRect myFittedRect;
	myFittedRect.m_width = fabs(m_wndExtent.m_width / ratiofactor);
	if (m_wndExtent.m_width < 0) {
		wxASSERT(originalrect.m_width < 0);
		myFittedRect.m_width *= -1;
	}
	myFittedRect.m_height = fabs(m_wndExtent.m_height / ratiofactor);
	if (m_wndExtent.m_height < 0) {
		wxASSERT(originalrect.m_height < 0);
		myFittedRect.m_height *= -1;
	}
	
	myFittedRect.SetCentre(originalrect.GetCentre());
	return myFittedRect;
}



void vrCoordinate::ClearLayersExtent() {
	m_layersExtent = vrRealRect();
}



void vrCoordinate::ClearPixelSize() {
	m_pxSize = wxNOT_FOUND;
}



void vrCoordinate::AddLayersExtent(const vrRealRect & rect) {
	
	if (m_layersExtent.IsOk()==false) {
		m_layersExtent = rect;
		return;
	}
	
	//vrRealRect myTempRect (m_layersExtent);
	vrRealRect myResult = m_layersExtent.Union(rect);
	if (myResult.IsOk() == false) {
		wxLogError("Computing rectangle union failed, result invalid");
		return;
	}
	
	m_layersExtent = myResult;
	
}


bool vrCoordinate::ComputeFullExtent() {
	
	if (m_layersExtent.IsOk() == false) {
		wxLogError("Extent for layers not specified, unable to compute full extent");
		return false;
	}
	
	wxASSERT(m_viewer);
	wxSize myWndSizePx = m_viewer->GetSize();
	vrRealRect myLayerExtent (m_layersExtent);
	
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
	
	
	
	m_wndExtent = vrRealRect();
	m_wndExtent.m_width = myWndSizePx.GetWidth();
	m_wndExtent.m_height = myWndSizePx.GetHeight();
	
	// scale
	m_wndExtent.m_width *= myDivFactorX;
	m_wndExtent.m_height *= myDivFactorY;
	
	
	// center rect
	wxPoint2DDouble myCenter = myLayerExtent.GetCentre();
	m_wndExtent.SetCentre(myCenter);

	return true;
}


double vrCoordinate::GetPixelSize() {
	
	if (m_pxSize == wxNOT_FOUND) {
		if (_ComputePixelSize() == false) {
			wxLogError("Error computing pixel size");
			return wxNOT_FOUND;
		}
	}
	
	return m_pxSize;
}


bool vrCoordinate::ConvertFromPixels(wxRect in, vrRealRect & out) {
	out = vrRealRect(0,0,0,0);
	if (in == wxRect(0,0,0,0)) {
        wxLogDebug(_("Rectangle not inited!"));
        return false;
    }
    
    if (in.IsEmpty() == true) {
        in.width = 1;
        in.height = 1;
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
	wxASSERT(m_wndExtent.IsOk());
	
	wxPoint2DDouble myLeftTop = m_wndExtent.GetLeftTop();
	if (m_wndExtent.m_width > 0) {
		out.m_x = myLeftTop.m_x + (in.x * GetPixelSize());
	}else {
		out.m_x = myLeftTop.m_x - (in.x * GetPixelSize());
	}

	if (m_wndExtent.m_height > 0) {
		out.m_y = myLeftTop.m_y + (in.y * GetPixelSize());
	}else {
		out.m_y = myLeftTop.m_y - (in.y * GetPixelSize());
	}
	return true;
}



bool vrCoordinate::ConvertToPixels (const wxPoint2DDouble & in, wxPoint & out){
	out = wxPoint(0,0);
    out.x = wxRound((in.m_x - m_wndExtent.GetLeftTop().m_x) / GetPixelSize());
    out.y = wxRound((m_wndExtent.GetLeftTop().m_y - in.m_y) / GetPixelSize());
    return true;
}


bool vrCoordinate::IsOk() {
	wxASSERT(m_viewer);
	
	return m_wndExtent.IsOk();
}


