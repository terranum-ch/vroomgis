/***************************************************************************
				vrrender.cpp
                    
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

#include "vrrender.h"

vrRender::vrRender() {
	m_Type = vrRENDER_UNKNOWN;
	m_Transparency = 0;
}



vrRender::~vrRender() {
}



vrRENDER_TYPE vrRender::GetType() const{
	return m_Type;
}



void vrRender::SetTransparency(int value) {
	wxASSERT(value >= 0 && value <= 100);
	m_Transparency = value;
}


unsigned char vrRender::GetTransparencyChar() {
	int myValue = m_Transparency * 255 / 100;
	return 255 - myValue;
}


vrRenderVector::vrRenderVector() {
	m_Type = vrRENDER_VECTOR;
	m_Size = 1;
	m_ColorPen = *wxBLACK;
	m_ColorBrush = *wxBLACK;
}



vrRenderVector::~vrRenderVector() {
}


void vrRenderVector::SetColorPen(const wxColour & color) {
	m_ColorPen = color;
}

void vrRenderVector::SetColorBrush(const wxColour & color) {
	m_ColorBrush = color;
}


void vrRenderVector::SetSize(int value) {
	m_Size = value;
}

wxColour vrRenderVector::GetColorPen() {
	char myRed = m_ColorPen.Red();
	char myGreen = m_ColorPen.Green();
	char myBlue = m_ColorPen.Blue();
	
	m_ColorPen.Set(myRed, myGreen, myBlue, GetTransparencyChar());	
	return m_ColorPen;
}


wxColour vrRenderVector::GetColorBrush(){
	char myRed = m_ColorBrush.Red();
	char myGreen = m_ColorBrush.Green();
	char myBlue = m_ColorBrush.Blue();
	
	m_ColorBrush.Set(myRed, myGreen, myBlue, GetTransparencyChar());	
	return m_ColorBrush;
}


vrRenderRaster::vrRenderRaster() {
	m_Type = vrRENDER_RASTER;
}

vrRenderRaster::~vrRenderRaster() {
}


vrRenderRasterColtop::vrRenderRasterColtop() {
	m_Type = vrRENDER_RASTER_C2D;
}

vrRenderRasterColtop::~vrRenderRasterColtop() {
}

