/***************************************************************************
								vrviewerdisplay.cpp
								Default display
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

#include "vrviewerdisplay.h"

vrViewerDisplay::vrViewerDisplay(wxWindow * parent, wxWindowID id, long style) :
wxScrolledCanvas(parent, id, wxDefaultPosition, wxDefaultSize, style){
}

vrViewerDisplay::~vrViewerDisplay() {
}

void vrViewerDisplay::Refresh() {
}

