/***************************************************************************
				vrrendercoltop.cpp
                    
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

#include "vrrendercoltop.h"

vrRenderRasterColtop::vrRenderRasterColtop() {
	m_Type = vrRENDER_RASTER_C2D;
	m_NorthAngle = 0;
	m_IsColorInverted = false;
	m_IsLowerHemisphere = true;
}



vrRenderRasterColtop::~vrRenderRasterColtop() {
}



wxImage::RGBValue vrRenderRasterColtop::GetColorFromDipDir(double dip, double dipdir) {
	double myDir = dipdir;
	double myDip = dip;
	
	if (m_IsLowerHemisphere == false) {
		myDip = 180.0 - myDip;
	}
	
	myDir = myDir + m_NorthAngle;
	if (myDir > 360.0) {
		myDir = myDir - 360.0;
	}
	wxASSERT(myDir < 360.0);
	
	// normalize
	double myNDip = myDip / 90.0;
	double myNDir = myDir / 360.0;
	
	if (m_IsColorInverted == true) {
		myNDir = 1.0 - myNDir;
	}
	
	wxImage::HSVValue myHSVValue (myNDir,
								  myNDip,
								  1);
	wxImage::RGBValue myRGBValue = wxImage::HSVtoRGB(myHSVValue);
	return myRGBValue;
}



wxImage::RGBValue vrRenderRasterColtop::GetColorFromCircleCoord(const wxPoint & coord, double radius) {
	wxImage::RGBValue myValue;
	return myValue;
}



void vrRenderRasterColtop::SetNorthAngle(int value) {
  m_NorthAngle = value;
}



void vrRenderRasterColtop::SetColorInverted(bool value) {
  m_IsColorInverted = value;
}



void vrRenderRasterColtop::SetLowerHemisphere(bool value) {
  m_IsLowerHemisphere = value;
}



bool vrRenderRasterColtop::Serialize(vrSerialize & serialobj) {
	return false;
}

