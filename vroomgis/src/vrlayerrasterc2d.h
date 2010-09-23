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

#include <wx/image.h>
#include "vrlayerraster.h"


class vrRealRect;
class vrRender;
class vrLabel;

class vrLayerRasterC2D : public vrLayerRasterGDAL {
private:
	void _HSVtoRGB (int *r,int *g,int *b, int h, int s, int v);
	
protected:
    virtual bool _GetRasterData(unsigned char ** imgdata,
								const wxSize & outimgpxsize,
								const wxRect & readimgpxinfo, 
								const vrRender * render);
	
public:
    vrLayerRasterC2D();
    virtual ~vrLayerRasterC2D();
};
#endif
