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
	#include <wx/dataview.h>
#endif



//#include "vrlayer.h"



class vrRenderer;


class vrViewerTOC : public wxDataViewListCtrl {
private:
	wxDataViewItem m_ParentItem;
	
  public:
    vrViewerTOC(wxWindow * parent, wxWindowID id, const wxPoint & pos = wxDefaultPosition, const wxSize & size = wxDefaultSize, long style = wxDV_ROW_LINES);

    virtual ~vrViewerTOC();

    bool Add(int index, vrRenderer * renderer, int control);

    bool Remove(int index);

    void FreezeBegin();

    void FreezeEnd();

};
#endif
