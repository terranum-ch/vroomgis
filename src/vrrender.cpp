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



vrRENDER_TYPE vrRender::GetType() {
	return m_Type;
}



void vrRender::SetTransparency(int value) {
  m_Transparency = value;
}



vrRenderVector::vrRenderVector() {
	m_Type = vrRENDER_VECTOR;
}



vrRenderVector::~vrRenderVector() {
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
