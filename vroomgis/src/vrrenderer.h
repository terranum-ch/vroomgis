/***************************************************************************
 vrrenderer.h
 Informations for drawing GIS data (render to apply, label, etc.)
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
#ifndef _VRRENDERER_H
#define _VRRENDERER_H

#include "wx/wxprec.h"

#ifndef WX_PRECOMP

#include <wx/wx.h>

#endif

#include "vrrealrect.h"

class vrLayer;

class vrRender;

class vrLabel;

const int vrRENDERER_VECTOR_POINT_DEFAULT_SIZE = 4;

class vrRenderer {
  private:
    vrLayer* m_layer;
    vrRender* m_render;
    vrLabel* m_label;
    bool m_visible;
    bool m_inEdition;

    bool _IsCorrectRender();

  public:
    vrRenderer(vrLayer* layer, vrRender* render = NULL, vrLabel* label = NULL);

    virtual ~vrRenderer();

    inline vrLayer* GetLayer();

    inline vrRender* GetRender();

    inline vrLabel* GetLabel();

    inline const bool GetVisible() const;

    bool GetBitmapDataThread(wxImage* bmp, const vrRealRect& coord, double pxsize, long& vectorcount);

    bool GetBitmapData(wxBitmap* bmp, const vrRealRect& coord, double pxsize, long& vectorcount, long& drawnvertex,
                       long& skippedvertex);

    void SetRender(vrRender* value);

    void SetLabel(vrLabel* value);

    void SetVisible(bool value);

    inline const bool IsInEdition() const;

    void SetInEdition(bool value);
};

inline vrLayer* vrRenderer::GetLayer() {
    return m_layer;
}

inline vrRender* vrRenderer::GetRender() {
    return m_render;
}

inline vrLabel* vrRenderer::GetLabel() {
    return m_label;
}

inline const bool vrRenderer::GetVisible() const {
    return m_visible;
}

inline const bool vrRenderer::IsInEdition() const {
    return m_inEdition;
}

WX_DECLARE_OBJARRAY(vrRenderer*, vrArrayRenderer);

#endif
