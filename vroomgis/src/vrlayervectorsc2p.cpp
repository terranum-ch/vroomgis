/***************************************************************************
				vrlayervectorsc2p.cpp
                    
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

#include "vrlayervectorsc2p.h"
#include <wxRect2DDouble.h>

#include "vrrender.h"
#include "vrlabel.h"

bool vrLayerVectorC2P::_DrawPoints(wxGraphicsContext * gdc, const wxRect2DDouble & coord, const vrRender * render, const vrLabel * label, double pxsize) {
  return false;
}

bool vrLayerVectorC2P::_DrawLines(wxGraphicsContext * gdc, const wxRect2DDouble & coord, const vrRender * render, const vrLabel * label, double pxsize) {
  return false;
}

bool vrLayerVectorC2P::_DrawPolygons(wxGraphicsContext * gdc, const wxRect2DDouble & coord, const vrRender * render, const vrLabel * label, double pxsize) {
  return false;
}

vrLayerVectorC2P::vrLayerVectorC2P() {
}

vrLayerVectorC2P::~vrLayerVectorC2P() {
}

bool vrLayerVectorC2P::Open(const wxFileName & filename, bool readwrite) {
}

