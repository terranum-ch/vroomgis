/***************************************************************************
								vrrender.h
				Decorator base class 
                             -------------------
    copyright            : (C) 2009 CREALP Lucien Schreiber 
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
#ifndef _VRRENDER_H
#define _VRRENDER_H

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif


enum vrRENDER_TYPE {
  vrRENDER_VECTOR,
  vrRENDER_RASTER,
  vrRENDER_RASTER_C2D,
  vrRENDER_UNKNOWN

};


class vrRender {
  protected:
    vrRENDER_TYPE  m_Type;

    int m_Transparency;


  public:
    vrRender();

    virtual ~vrRender();

    vrRENDER_TYPE GetType();

    inline const int GetTransparency() const;

    void SetTransparency(int value);

};

inline const int vrRender::GetTransparency() const {
  return m_Transparency;
}




class vrRenderVector : public vrRender {
private:
   static const int m_BgMaskColor[3];

	
  public:
    vrRenderVector();
    virtual ~vrRenderVector();
	
	static const wxColour GetBackgroundMaskColour();

};



class vrRenderRaster : public vrRender {
  public:
    vrRenderRaster();

    virtual ~vrRenderRaster();

};



class vrRenderRasterColtop : public vrRenderRaster {
  public:
    vrRenderRasterColtop();

    virtual ~vrRenderRasterColtop();

};


#endif
