/***************************************************************************
 vrcoordinate.h

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
#ifndef _VRCOORDINATE_H
#define _VRCOORDINATE_H

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "vrrealrect.h"

class vrViewerDisplay;


const int vrCOORDINATE_MARGIN = 10;

class vrCoordinate {
  private:
    vrViewerDisplay * m_Viewer;
	
	vrRealRect m_WndExtent;
    vrRealRect m_LayersExtent;
	double m_PxSize;
	

    bool _ComputePixelSize();
		

  public:
    vrCoordinate(vrViewerDisplay * viewer);
	vrCoordinate(const vrCoordinate & source);
	~vrCoordinate();
	

	vrRealRect GetExtent();
	bool UpdateExtent();

	
    void ClearLayersExtent();
    void AddLayersExtent(const vrRealRect & rect);
    bool ComputeFullExtent();
	
	double GetPixelSize();
	bool IsOk();

};
#endif
