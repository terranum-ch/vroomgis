/***************************************************************************
 fake_vrviewerdisplay.cpp

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


#include "fake_vrviewerdisplay.h"

FakevrViewerDisplay::FakevrViewerDisplay(const wxSize & size) {
}

FakevrViewerDisplay::~FakevrViewerDisplay() {
	int i = 0;
}

void FakevrViewerDisplay::DoGetSize(int * w, int *h) const{
	* w = 600;
	* h = 400;
}

