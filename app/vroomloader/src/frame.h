/***************************************************************************
								frame.h
					First test program for VroomGIS 
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


#ifndef _FRAME_H_
#define _FRAME_H_

#include "wx/wxprec.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif


#if !defined(__WXMSW__) && !defined(__WXPM__)
#include "sample.xpm"
#endif

#include <wx/splitter.h>	//splitter bar
//#include <wx/treectrl.h>	// temp treectrl


#include "../../../src/vroomgis.h"

class vroomLoader : public wxApp
{
public:
     virtual bool OnInit();
};





class vroomLoaderFrame : public wxFrame
{
private:
	wxTextCtrl* m_LogCtrl;
	
	// controls (vroomgis)
	vrViewerTOC * m_TocCtrl;
	
	// vroomgis engine
	vrLayerManager m_LayerManager;
	vrViewerLayerManager * m_ViewerLayerManager;
	
	wxScrolledWindow* m_DisplayCtrl;
	
	
	void _CreateControls();
	
	virtual void OnRightClick( wxMouseEvent& event ){ wxLogMessage("right clicked"); event.Skip(); }
	
public:
    vroomLoaderFrame(const wxString& title);
	~vroomLoaderFrame();
	
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
	void OnOpenLayer(wxCommandEvent & event);
	
private:
    DECLARE_EVENT_TABLE()
};




#endif


