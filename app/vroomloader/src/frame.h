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
#include "../../../art/vroomgis.xpm"
#endif

#include <wx/splitter.h>	//splitter bar
#include <wx/dnd.h>			// dnd
//#include <wx/treectrl.h>	// temp treectrl


#include "../../../src/vroomgis.h"
class vrDisplayValueDlg;


const int vlID_MOVE_LAYER		= wxID_HIGHEST + 1;
const int vlID_DISPLAY_VALUE    = wxID_HIGHEST + 2;
const int vlID_DISPLAY_VALUE_DLG= wxID_HIGHEST + 3;

const int vl_POPUP_OFFSET = 50;
const int vlID_MENU_POPUP_LAYER = wxID_HIGHEST + 2 + vl_POPUP_OFFSET; 


class vroomLoader : public wxApp
{
public:
     virtual bool OnInit();
};

class vroomLoaderFrame;
class vroomDropFiles : public wxFileDropTarget {
private:
	vroomLoaderFrame * m_LoaderFrame;
	
public:
	vroomDropFiles(vroomLoaderFrame * parent);
	virtual bool OnDropFiles(wxCoord x, wxCoord y, 
							 const wxArrayString & filenames);
};



class vroomLoaderFrame : public wxFrame
{
private:
	//wxTextCtrl* m_LogCtrl;
	wxLogWindow * m_LogWnd;
	
	vrDisplayValueDlg * m_DisplayValueDlg;
	
	// controls (vroomgis)
	vrViewerTOC * m_TocCtrl;
	
	// vroomgis engine
	vrLayerManager * m_LayerManager;
	vrViewerLayerManager * m_ViewerLayerManager;
	
	vrViewerDisplay * m_DisplayCtrl;
	
	
	void _CreateControls();
	
	virtual void OnRightClick( wxMouseEvent& event ){ wxLogMessage("right clicked"); event.Skip(); }
	
public:
    vroomLoaderFrame(const wxString& title);
	~vroomLoaderFrame();
	
	bool OpenLayers (const wxArrayString & names);
	
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
	void OnOpenLayer(wxCommandEvent & event);
	
	void OnCloseLayer(wxCommandEvent & event);
	void OnShowLog (wxCommandEvent & event);
	
	void OnToolSelect (wxCommandEvent & event);
	void OnToolZoom (wxCommandEvent & event);
	void OnToolPan (wxCommandEvent & event);
	void OnToolZoomToFit (wxCommandEvent & event);
	void OnMoveLayer (wxCommandEvent & event);
	void OnToolDisplayValue (wxCommandEvent & event);
	
	void OnToolAction (wxCommandEvent & event);
	
private:
    DECLARE_EVENT_TABLE()
};




#endif


