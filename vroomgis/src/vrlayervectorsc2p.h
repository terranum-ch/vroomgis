/***************************************************************************
				vrlayervectorsc2p.h
                    
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

#ifndef _VRLAYERVECTORSC2P_H_
#define _VRLAYERVECTORSC2P_H_

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
// Include wxWidgets' headers
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif


#include "vrlayervector.h"

class vrRender;
class vrLabel;

//For dealing with GIS data stored into C2P projects.
class vrLayerVectorC2P : public vrLayerVectorOGR {
  protected:
    virtual bool _DrawPoints(wxGraphicsContext * gdc, const wxRect2DDouble & coord, const vrRender * render, const vrLabel * label, double pxsize);

    virtual bool _DrawLines(wxGraphicsContext * gdc, const wxRect2DDouble & coord, const vrRender * render, const vrLabel * label, double pxsize);

    virtual bool _DrawPolygons(wxGraphicsContext * gdc, const wxRect2DDouble & coord, const vrRender * render, const vrLabel * label, double pxsize);


  public:
    vrLayerVectorC2P();

    virtual ~vrLayerVectorC2P();

    virtual bool Open(const wxFileName & filename, bool readwrite = false);

};
#endif

