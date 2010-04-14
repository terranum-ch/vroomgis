/***************************************************************************
								vrviewertoc.h
				Default TOC
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
#ifndef _VRVIEWERTOC_H
#define _VRVIEWERTOC_H

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif



class vrRenderer;
class vrViewerLayerManager;

// event ID
const int vrID_VIEWERTOC_BASE = 100;
const int vrID_POPUP_REMOVE = wxID_HIGHEST +		1 + vrID_VIEWERTOC_BASE;
const int vrID_POPUP_TRANSPARENCY = wxID_HIGHEST +	2 + vrID_VIEWERTOC_BASE;
const int vrID_POPUP_PEN_COLOR = wxID_HIGHEST +		3 + vrID_VIEWERTOC_BASE;
const int vrID_POPUP_BRUSH_COLOR = wxID_HIGHEST +	4 + vrID_VIEWERTOC_BASE;
const int vrID_POPUP_DRAWING_WIDTH = wxID_HIGHEST +	5 + vrID_VIEWERTOC_BASE;


class vrViewerTOC : public wxCheckListBox {
private:
	
	vrViewerLayerManager * m_ViewerManager;
	bool m_FreezeStatus;
	
	// event function
	void OnVisibleStatusChanged(wxCommandEvent & event);
	void OnMouseRightDown(wxMouseEvent & event);
	// event function popup
	void OnSetColorPen(wxCommandEvent & event);
    void OnSetColorBrush(wxCommandEvent & event);
    void OnSetTransparency(wxCommandEvent & event);
    void OnSetWidth(wxCommandEvent & event);
	
	DECLARE_EVENT_TABLE();
	
	
protected:
    void _ReloadData();
	virtual void _ShowMenuContextual(int id, vrRenderer * renderer);
	
	
	
public:
    vrViewerTOC(wxWindow * parent, wxWindowID id, const wxPoint & pos = wxDefaultPosition,
				const wxSize & size = wxDefaultSize);
	
    virtual ~vrViewerTOC();
	
    bool Add(int index, vrRenderer * renderer, int control);
    bool Remove(int index);
	
    void FreezeBegin();
    void FreezeEnd();
	
	void SetViewerLayerManager(vrViewerLayerManager * value);
	
	
};
#endif
