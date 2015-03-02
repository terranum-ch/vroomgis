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
    vrViewerDisplay * m_viewer;
	
	vrRealRect m_wndExtent;
    vrRealRect m_layersExtent;
	double m_pxSize;
	

    bool _ComputePixelSize();
		

  public:
    vrCoordinate(vrViewerDisplay * viewer);
	vrCoordinate(const vrCoordinate & source);
	~vrCoordinate();
	

	vrRealRect GetExtent();
	void SetExtent(const vrRealRect & extent);
	bool UpdateExtent();
	vrRealRect GetRectFitted(const vrRealRect & originalrect);

	
    void ClearLayersExtent();
	void ClearPixelSize();
    void AddLayersExtent(const vrRealRect & rect);
    bool ComputeFullExtent();
	
	double GetPixelSize();
	
	bool ConvertFromPixels(wxRect in, vrRealRect & out);
    bool ConvertFromPixels(const wxPoint & in, wxPoint2DDouble & out);
	bool ConvertToPixels (const wxPoint2DDouble & in, wxPoint & out);
	
	
	bool IsOk();

};
#endif
