/***************************************************************************
				vrlayervectorsc2dp.cpp
                    
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

#include "vrlayervectorsc2dp.h"
#include "vrrender.h"
#include "vrlabel.h"

bool vrLayerVectorC2DP::_DrawPoints(wxGraphicsContext * gdc, const wxRect2DDouble & coord,
									const vrRender * render, const vrLabel * label, double pxsize) {
	wxLogError("Here we draw the Dips...");
	return false;
}



bool vrLayerVectorC2DP::_DrawLines(wxGraphicsContext * gdc, const wxRect2DDouble & coord,
								   const vrRender * render, const vrLabel * label, double pxsize) {
  return false;
}



bool vrLayerVectorC2DP::_DrawPolygons(wxGraphicsContext * gdc, const wxRect2DDouble & coord,
									  const vrRender * render, const vrLabel * label, double pxsize) {
  return false;
}



vrLayerVectorC2DP::vrLayerVectorC2DP() {
	m_DriverType = vrDRIVER_VECTOR_C2DP;
	m_GeometryType = wkbUnknown;
}



vrLayerVectorC2DP::~vrLayerVectorC2DP() {
	_Close();
}

