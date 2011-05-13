/***************************************************************************
 vrlayervectorsstar.h
 
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

#ifndef _VRLAYERVECTORSTAR_H_
#define _VRLAYERVECTORSTAR_H_

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
// Include wxWidgets' headers
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include "vrlayervector.h"

class vrRender;
class vrLabel;



class vrLayerVectorStar : public vrLayerVectorOGR {
  protected:
    virtual bool _DrawPoints(wxGraphicsContext * gdc, const wxRect2DDouble & coord,
							 const vrRender * render, const vrLabel * label, double pxsize);
    virtual bool _DrawLines(wxGraphicsContext * gdc, const wxRect2DDouble & coord,
							const vrRender * render, const vrLabel * label, double pxsize);
    virtual bool _DrawPolygons(wxGraphicsContext * gdc, const wxRect2DDouble & coord,
							   const vrRender * render, const vrLabel * label, double pxsize);
	
	void _CreateStarPath(wxGraphicsPath & starpath,const wxPoint & center, int radius);
	inline double _DegToRad(double deg){return (deg * M_PI) / 180.0;}


  public:
    vrLayerVectorStar();
    virtual ~vrLayerVectorStar();

	virtual long AddFeature(OGRGeometry * geometry, void * data = NULL);
};
#endif

