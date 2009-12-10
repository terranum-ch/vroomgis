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


class vrLayer;
class vrRender;
class vrLabel;

class vrRenderer {
  private:
    vrLayer * m_Layer;

    vrRender * m_Render;

    vrLabel * m_Label;


  public:
    vrRenderer(vrLayer * layer, vrRender * render = NULL, vrLabel * label = NULL);

    virtual ~vrRenderer();

    inline const vrLayer * get_m_Layer() const;

    inline const vrRender * get_m_Render() const;

    inline const vrLabel * get_m_Label() const;

    void set_m_Render(vrRender * value);

    void set_m_Label(vrLabel * value);

};
inline const vrLayer * vrRenderer::get_m_Layer() const {
  return m_Layer;
}

inline const vrRender * vrRenderer::get_m_Render() const {
  return m_Render;
}

inline const vrLabel * vrRenderer::get_m_Label() const {
  return m_Label;
}

#endif
