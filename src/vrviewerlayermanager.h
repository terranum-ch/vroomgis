/***************************************************************************
								vrviewerlayermanager.h
				List of supported drivers and misc function for them
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
#ifndef _VRVIEWERLAYERMANAGER_H
#define _VRVIEWERLAYERMANAGER_H

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/wx.h>
	
#endif
#include <wx/filename.h>
#include "vrrenderer.h"


class vrLayerManager;
class vrLayer;
class vrViewerDisplay;
class vrViewerTOC;
class vrViewerDisplay;


// Event definition. If needed, move to an other global event file
wxDECLARE_EVENT(vrEVT_VLM_RELOAD, wxCommandEvent);


// Array of images (bitmaps arn't thread safe)
WX_DECLARE_OBJARRAY(wxImage*, wxArrayImage);



class vrViewerLayerManager : public wxEvtHandler {
  private:
    vrLayerManager * m_LayerManager;
    vrArrayRenderer m_Renderers;
    vrViewerDisplay * m_Display;
    vrViewerTOC * m_Toc;
	wxWindow * m_WindowParent;
	wxArrayImage * m_Images;
	
	bool m_FreezeStatus;

	
	// Member function for loading data 
	bool _BitmapArrayInit();
    void _BitmapArrayDelete();
    bool _GetLayersData();
    bool _MergeBitmapData();
    void OnReload(wxCommandEvent & event);
	
	DECLARE_EVENT_TABLE();

  public:
    //if wxWindow is null we cannot link to event handling
	vrViewerLayerManager(vrLayerManager * parent,wxWindow * window, 
						 vrViewerDisplay * viewer, vrViewerTOC * toc = NULL);

    virtual ~vrViewerLayerManager();

    bool Add(long pos, vrLayer * layer, vrRender * render = NULL, vrLabel * label = NULL); 
    bool Remove(const wxFileName & filename);
	vrRenderer * GetRenderer(const unsigned int & index);

	
    void FreezeBegin();
    void FreezeEnd();

};
WX_DECLARE_OBJARRAY(vrViewerLayerManager*, vrArrayViewerLayerManager);





#endif
