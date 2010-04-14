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
	virtual vrRENDER_TYPE GetType() const;

    inline const int GetTransparency() const;
    void SetTransparency(int value);

};

inline const int vrRender::GetTransparency() const {
  return m_Transparency;
}




class vrRenderVector : public vrRender {
private:
	wxColour m_ColorPen;
    wxColour m_ColorBrush;
	int m_Size;


  public:
    vrRenderVector();
    virtual ~vrRenderVector();
	
	inline const wxColour GetColorPen() const;
    void SetColorPen(const wxColour & color);
	
    inline const wxColour GetColorBrush() const;
    void SetColorBrush(const wxColour & color);
		
    inline const int GetSize() const;
    void SetSize(int value);
	
};

inline const wxColour vrRenderVector::GetColorPen() const {
	return m_ColorPen;
}

inline const wxColour vrRenderVector::GetColorBrush() const {
	return m_ColorBrush;
}

inline const int vrRenderVector::GetSize() const {
	return m_Size;
}


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
