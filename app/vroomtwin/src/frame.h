/***************************************************************************
 vroomTwin.h
 Two Windows test program for VroomGIS
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

#ifndef _VROOMTWIN_TEST_H_
#define _VROOMTWIN_TEST_H_

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif
#ifndef WX_PRECOMP

#include "wx/wx.h"

#endif

#include <wx/dnd.h>
#include <wx/splitter.h>

#include "vroomgis.h"

const int vtID_SET_SYNCRO_MODE = wxID_HIGHEST + 1;
const int vtID_SIGHT_TOOL = wxID_HIGHEST + 2;
const int vtID_OVERLAY_TEXT = wxID_HIGHEST + 3;

class vroomTwinFrame;

class vroomTwin : public wxApp {
  public:
    virtual bool OnInit();
};

class vroomDropFiles : public wxFileDropTarget {
  private:
    vroomTwinFrame* m_LoaderFrame;

  public:
    vroomDropFiles(vroomTwinFrame* parent);

    virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames);
};

class vroomTwinFrame : public wxFrame {
  private:
    wxLogWindow* m_LogWnd;
    bool m_SyncroTool;
    wxOverlay m_Overlay;

    // vroomgis engine
    vrLayerManager* m_LayerManager;

    vrViewerTOCList* m_TocCtrl1;
    vrViewerLayerManager* m_ViewerLayerManager1;
    vrViewerDisplay* m_DisplayCtrl1;

    vrViewerTOCList* m_TocCtrl2;
    vrViewerLayerManager* m_ViewerLayerManager2;
    vrViewerDisplay* m_DisplayCtrl2;

    void _CreateControls();

    DECLARE_EVENT_TABLE();

  public:
    vroomTwinFrame(const wxString& title);

    ~vroomTwinFrame();

    bool OpenLayers(const wxArrayString& names);

    void OnQuit(wxCommandEvent& event);

    void OnAbout(wxCommandEvent& event);

    void OnOpenLayer(wxCommandEvent& event);

    void OnCloseLayer(wxCommandEvent& event);

    void OnShowLog(wxCommandEvent& event);

    void OnOverlayText(wxCommandEvent& event);

    void OnSyncroToolSwitch(wxCommandEvent& event);

    // void OnToolSelect (wxCommandEvent & event);
    void OnToolZoom(wxCommandEvent& event);

    void OnToolPan(wxCommandEvent& event);

    void OnToolSight(wxCommandEvent& event);

    void OnToolZoomToFit(wxCommandEvent& event);

    void OnToolAction(wxCommandEvent& event);
};

#endif
