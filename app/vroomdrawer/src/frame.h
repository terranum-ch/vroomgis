/***************************************************************************
 frame.h
 Drawer test program for VroomGIS 
 -------------------
 copyright            : (C) 2011 CREALP Lucien Schreiber 
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

#include <wx/filename.h>
#include <wx/splitter.h>	//splitter bar
#include <wx/stdpaths.h>
#include "vroomgis.h"


const int ID_MENU_SELECT = wxID_HIGHEST + 1;
const int ID_MENU_PAN = wxID_HIGHEST + 2;
const int ID_MENU_ADDMEMORYLAYER = wxID_HIGHEST + 3;


class vroomDrawer
        : public wxApp
{
public:
    vroomDrawer();

    virtual bool OnInit();

    virtual void OnFatalException();
};

DECLARE_APP(vroomDrawer);


class vroomDrawerFrame
        : public wxFrame
{
private:
    //wxTextCtrl* m_LogCtrl;
    wxLogWindow *m_LogWnd;

    // controls (vroomgis)
    vrViewerTOCTree *m_TocCtrl;
    vrViewerDisplay *m_DisplayCtrl;
    vrLayerManager *m_LayerManager;
    vrViewerLayerManager *m_ViewerLayerManager;

    wxKeyboardState m_KeyBoardState;
    wxSlider *m_NbStarCtrl;
    wxRadioBox *m_FeatureTypeCtrl;

    void _CreateControls();
    void _CreateToolbar();

    double _GetRandomNumber(double min, double max);

    vrLayer *_GetMemoryLayerPoints(const wxFileName &name, int number, const vrRealRect &extent);

    vrLayer *_GetMemoryLayerLine(const wxFileName &name, int number, const vrRealRect &extent);

    vrLayer *_GetMemoryLayerStar(const wxFileName &name, int number, const vrRealRect &extent);

    void OnKeyDown(wxKeyEvent &event);

    void OnKeyUp(wxKeyEvent &event);

    void OnQuit(wxCommandEvent &event);

    void OnAbout(wxCommandEvent &event);

    void OnLayerAdd(wxCommandEvent &event);

    void OnLayerRemove(wxCommandEvent &event);

    void OnToolZoom(wxCommandEvent &event);

    void OnToolSelect(wxCommandEvent &event);

    void OnToolPan(wxCommandEvent &event);

    void OnZoomToFit(wxCommandEvent &event);

    void OnTestCrashSoftware(wxCommandEvent &event);

    void OnShowLog(wxCommandEvent &event);

    void OnToolActionZoom(wxCommandEvent &event);

    void OnToolActionPan(wxCommandEvent &event);

    void OnToolActionSelect(wxCommandEvent &event);

    void OnStarLayerAdd(wxCommandEvent &event);

    void OnMemoryLayerAdd(wxCommandEvent &event);

DECLARE_EVENT_TABLE()

public:
    vroomDrawerFrame(const wxString &title);

    ~vroomDrawerFrame();

    bool OpenLayers(const wxArrayString &names);
};


#endif


