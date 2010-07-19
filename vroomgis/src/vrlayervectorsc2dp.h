/***************************************************************************
				vrlayervectorsc2dp.h
                    
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

#ifndef _VRLAYERVECTORSC2DP_H_
#define _VRLAYERVECTORSC2DP_H_

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
// Include wxWidgets' headers
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif


#include "vrlayervector.h"
class vrRender;
class vrLabel;

//For dealing with dip layer.
class vrLayerVectorC2DP : public vrLayerVectorOGR {
  protected:
    virtual bool _DrawPoints(wxGraphicsContext * gdc, const wxRect2DDouble & coord,
							 const vrRender * render, const vrLabel * label, double pxsize);
    virtual bool _DrawLines(wxGraphicsContext * gdc, const wxRect2DDouble & coord,
							const vrRender * render, const vrLabel * label, double pxsize);
    virtual bool _DrawPolygons(wxGraphicsContext * gdc, const wxRect2DDouble & coord,
							   const vrRender * render, const vrLabel * label, double pxsize);


  public:
    vrLayerVectorC2DP();
    virtual ~vrLayerVectorC2DP();

};
#endif

