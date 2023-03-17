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

#include "vrserialize.h"

enum vrRENDER_TYPE {
    vrRENDER_VECTOR = 0,
    vrRENDER_VECTOR_C2P_DIPS,
    vrRENDER_VECTOR_C2P_POLY,
    vrRENDER_RASTER,
    vrRENDER_RASTER_C2D,
    vrRENDER_UNKNOWN
};

class vrRender {
  protected:
    vrRENDER_TYPE m_type;
    int m_transparency;
    wxColour m_selectionColour;
    static wxColour m_defaultSelectionColour;

    unsigned char GetTransparencyChar();

  public:
    vrRender();

    virtual ~vrRender();

    virtual vrRENDER_TYPE GetType() const;

    inline const int GetTransparency() const;

    virtual void SetTransparency(int value);

    wxColour GetSelectionColour();

    void SetSelectionColour(wxColour value);

    static void SetDefaultSelectionColour(wxColour value);

    virtual bool Serialize(vrSerialize& serialobj);
};

inline const int vrRender::GetTransparency() const {
    return m_transparency;
}

class vrRenderVector : public vrRender {
  private:
    wxColour m_colorPen;
    wxColour m_colorBrush;
    wxBrushStyle m_brushStyle;
    int m_size;
    bool m_useFastAndUglyDC;

  public:
    vrRenderVector();

    virtual ~vrRenderVector();

    wxColour GetColorPen();

    void SetColorPen(const wxColour& color);

    wxColour GetColorBrush();

    void SetColorBrush(const wxColour& color);

    wxBrushStyle GetBrushStyle();

    void SetBrushStyle(wxBrushStyle brushstyle);

    inline virtual const int GetSize() const;

    virtual void SetSize(int value);

    inline const bool IsUsingFastAndUglyDC() const;

    void SetUseFastAndUglyDC(bool value);

    virtual bool Serialize(vrSerialize& serialobj);
};

inline const int vrRenderVector::GetSize() const {
    return m_size;
}

inline const bool vrRenderVector::IsUsingFastAndUglyDC() const {
    return m_useFastAndUglyDC;
}

class vrRenderRaster : public vrRender {
  public:
    vrRenderRaster();

    virtual ~vrRenderRaster();
};

#endif
