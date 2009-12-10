/***************************************************************************
				vrrenderer.cpp
                    
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

#include "vrrenderer.h"
#include "vrlayer.h"
#include "vrrender.h"
#include "vrlabel.h"

vrRenderer::vrRenderer(vrLayer * layer, vrRender * render, vrLabel * label) {
}

vrRenderer::~vrRenderer() {
}

void vrRenderer::set_m_Render(vrRender * value) {
  m_Render = value;
}

void vrRenderer::set_m_Label(vrLabel * value) {
  m_Label = value;
}

