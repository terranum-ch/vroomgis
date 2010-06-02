/***************************************************************************
				vrlayerrasterc2d.h
                    
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

#ifndef _VRLAYERRASTERC2D_H
#define _VRLAYERRASTERC2D_H

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
// Include wxWidgets' headers
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif


#include "vrlayerraster.h"

class vrRealRect;
class vrRender;
class vrLabel;

class vrLayerRasterC2D : public vrLayerRasterGDAL {
  public:
    vrLayerRasterC2D();

    virtual ~vrLayerRasterC2D();

    //virtual bool GetData(wxImage * bmp, const vrRealRect & coord, double pxsize, const vrRender * render = NULL, const vrLabel * label = NULL);

};
#endif
