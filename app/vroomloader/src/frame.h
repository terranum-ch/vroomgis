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

#include <wx/dnd.h>         // dnd
#include <wx/filename.h>    // filename support
#include <wx/filepicker.h>  // file picker
#include <wx/splitter.h>    //splitter bar
#include <wx/tglbtn.h>      // toggle button

#include "vroomgis.h"

class vrDisplayValueDlg;

class vrShapeEditor;

class vrLayerVectorOGR;

const int vlID_MOVE_LAYER = wxID_HIGHEST + 1;
const int vlID_DISPLAY_VALUE = wxID_HIGHEST + 2;
const int vlID_DISPLAY_VALUE_DLG = wxID_HIGHEST + 3;
const int vlID_PERFORMANCE = wxID_HIGHEST + 4;
const int vlID_THREADED = wxID_HIGHEST + 5;
const int vlID_EDIT_BTN = wxID_HIGHEST + 6;
const int vlID_EDIT_CHOICE = wxID_HIGHEST + 7;
const int vlID_DRAW_MENU = wxID_HIGHEST + 8;
const int vlID_MODIFY_MENU = wxID_HIGHEST + 9;

const int vl_POPUP_OFFSET = 50;
const int vlID_MENU_POPUP_LAYER = wxID_HIGHEST + 2 + vl_POPUP_OFFSET;

class vroomLoader : public wxApp {
  public:
    virtual bool OnInit();
};

DECLARE_APP(vroomLoader);

class vroomLoaderFrame;

class vroomDropFiles : public wxFileDropTarget {
  private:
    vroomLoaderFrame* m_LoaderFrame;

  public:
    vroomDropFiles(vroomLoaderFrame* parent);

    virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames);
};

class vroomLoaderFrame : public wxFrame {
  private:
    // wxTextCtrl* m_LogCtrl;
    wxLogWindow* m_LogWnd;

    vrDisplayValueDlg* m_DisplayValueDlg;

    wxChoice* m_editTypeCtrl;
    wxToggleButton* m_editStartCtrl;

    // controls (vroomgis)
    vrViewerTOCTree* m_TocCtrl;

    // vroomgis engine
    vrLayerManager* m_LayerManager;
    vrViewerLayerManager* m_ViewerLayerManager;

    wxKeyboardState m_KeyBoardState;

    vrViewerDisplay* m_DisplayCtrl;
    wxFileName m_PerfLogFile;

    vrShapeEditor* m_editor;

    void _CreateControls();

    void _StartEdition();

    void _StopEdition();

    bool _SaveEdition(vrLayer* memorylayer);

    vrRenderer* _GetMemoryRenderer();

    virtual void OnRightClick(wxMouseEvent& event) {
        wxLogMessage("right clicked");
        event.Skip();
    }

    void OnKeyDown(wxKeyEvent& event);

    void OnKeyUp(wxKeyEvent& event);

    void OnQuit(wxCommandEvent& event);

    void OnAbout(wxCommandEvent& event);

    void OnOpenLayer(wxCommandEvent& event);

    void OnCloseLayer(wxCommandEvent& event);

    void OnShowLog(wxCommandEvent& event);

    void OnToolSelect(wxCommandEvent& event);

    void OnToolZoom(wxCommandEvent& event);

    void OnToolPan(wxCommandEvent& event);

    void OnToolZoomToFit(wxCommandEvent& event);

    void OnMoveLayer(wxCommandEvent& event);

    void OnToolDisplayValue(wxCommandEvent& event);

    void OnLogPerformance(wxCommandEvent& event);

    void OnEngineThreaded(wxCommandEvent& event);

    void OnToolAction(wxCommandEvent& event);

    void OnStartEditingButton(wxCommandEvent& event);

    void OnUpdateUIEditType(wxUpdateUIEvent& event);

    void OnUpdateUIDrawMenu(wxUpdateUIEvent& event);

    void OnToolModify(wxCommandEvent& event);

    void OnToolModifySearch(wxCommandEvent& event);

    void OnToolModifyUpdate(wxCommandEvent& event);

    void OnToolDraw(wxCommandEvent& event);

    void OnToolDrawAction(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()

  public:
    vroomLoaderFrame(const wxString& title);

    ~vroomLoaderFrame();

    bool OpenLayers(const wxArrayString& names);
};

#endif
