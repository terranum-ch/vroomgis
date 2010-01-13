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
	bool m_Visible;

	
	bool _IsCorrectRender();
	
public:
    vrRenderer(vrLayer * layer, vrRender * render = NULL, vrLabel * label = NULL);
	
    virtual ~vrRenderer();
	
    inline  vrLayer * GetLayer() ;
    inline  vrRender * GetRender() ;
    inline  vrLabel * GetLabel() ;
	inline const bool GetVisible() const;
	
    void SetRender(vrRender * value);
    void SetLabel(vrLabel * value);
	void SetVisible(bool value);

};


inline  vrLayer * vrRenderer::GetLayer() {
	return m_Layer;
}

inline  vrRender * vrRenderer::GetRender()  {
	return m_Render;
}

inline  vrLabel * vrRenderer::GetLabel()  {
	return m_Label;
}

inline const bool vrRenderer::GetVisible() const {
	return m_Visible;
}

WX_DECLARE_OBJARRAY(vrRenderer*, vrArrayRenderer);
#endif
