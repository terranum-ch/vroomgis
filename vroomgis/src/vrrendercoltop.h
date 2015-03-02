/***************************************************************************
 vrrendercoltop.h
 
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

#ifndef _VRRENDERCOLTOP_H_
#define _VRRENDERCOLTOP_H_

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
// Include wxWidgets' headers
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/image.h>


#include "vrrender.h"

class vrRenderRasterColtop : public vrRenderRaster {
protected:
    int m_northAngle;
	int m_colorStretchMin;
    int m_colorStretchMax;
	bool m_isColorInverted;
    bool m_isLowerHemisphere;

	
public:
    vrRenderRasterColtop();
    virtual ~vrRenderRasterColtop();
	
    virtual wxImage::RGBValue GetColorFromDipDir(double dip, double dipdir);
    virtual wxImage::RGBValue GetColorFromCircleCoord(const wxPoint & coord, int radius);
	
    void SetNorthAngle(int value);
    void SetColorInverted(bool value);
    void SetLowerHemisphere(bool value);
	void SetColorStretchMin(int value);
    void SetColorStretchMax(int value);
	
	
    virtual bool Serialize(vrSerialize & serialobj);	
};
#endif

