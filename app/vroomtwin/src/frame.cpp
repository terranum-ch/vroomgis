/***************************************************************************
 frame.cpp
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

#ifdef USE_VLD
#include <vld.h>  // Visual Leak Detector (https://vld.codeplex.com/)
#endif

#include <wx/filepicker.h>
#include <wx/msgdlg.h>

#include "../../../vroomgis/art/vroomgis_bmp.cpp"
#include "frame.h"
#include "tmlog.h"  // for double logging process

BEGIN_EVENT_TABLE(vroomTwinFrame, wxFrame)
EVT_MENU(wxID_EXIT, vroomTwinFrame::OnQuit)
EVT_MENU(wxID_ABOUT, vroomTwinFrame::OnAbout)
EVT_MENU(wxID_OPEN, vroomTwinFrame::OnOpenLayer)
EVT_MENU(wxID_REMOVE, vroomTwinFrame::OnCloseLayer)
EVT_MENU(wxID_INFO, vroomTwinFrame::OnShowLog)
EVT_MENU(wxID_ZOOM_IN, vroomTwinFrame::OnToolZoom)
EVT_MENU(wxID_ZOOM_FIT, vroomTwinFrame::OnToolZoomToFit)
EVT_MENU(wxID_MOVE_FRAME, vroomTwinFrame::OnToolPan)
EVT_MENU(vtID_SIGHT_TOOL, vroomTwinFrame::OnToolSight)
EVT_MENU(vtID_SET_SYNCRO_MODE, vroomTwinFrame::OnSyncroToolSwitch)
EVT_MENU(vtID_OVERLAY_TEXT, vroomTwinFrame::OnOverlayText)
EVT_COMMAND(wxID_ANY, vrEVT_TOOL_ZOOM, vroomTwinFrame::OnToolAction)
EVT_COMMAND(wxID_ANY, vrEVT_TOOL_SELECT, vroomTwinFrame::OnToolAction)
EVT_COMMAND(wxID_ANY, vrEVT_TOOL_PAN, vroomTwinFrame::OnToolAction)
EVT_COMMAND(wxID_ANY, vrEVT_TOOL_SIGHT, vroomTwinFrame::OnToolAction)
END_EVENT_TABLE()

IMPLEMENT_APP(vroomTwin)

vroomDropFiles::vroomDropFiles(vroomTwinFrame* parent) {
    wxASSERT(parent);
    m_LoaderFrame = parent;
}

bool vroomDropFiles::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames) {
    if (filenames.GetCount() == 0) {
        return false;
    }

    m_LoaderFrame->OpenLayers(filenames);
    return true;
}

bool vroomTwin::OnInit() {
    if (!wxApp::OnInit()) return false;

    wxImage::AddHandler(new wxPNGHandler);
    vroomgis_initialize_images();

    vroomTwinFrame* frame = new vroomTwinFrame("vroomTwin");
    frame->SetSize(50, 50, 800, 500);
    frame->Show(true);
    return true;
}

void vroomTwinFrame::_CreateControls() {
    wxBoxSizer* bSizer1;
    bSizer1 = new wxBoxSizer(wxVERTICAL);

    wxSplitterWindow* m_splitter2;
    m_splitter2 = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                       wxSP_BORDER | wxSP_LIVE_UPDATE);

    wxPanel* m_panel1;
    m_panel1 = new wxPanel(m_splitter2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* bSizer4;
    bSizer4 = new wxBoxSizer(wxVERTICAL);

    wxStaticText* m_staticText2 = new wxStaticText(m_panel1, wxID_ANY, "Display 1", wxDefaultPosition, wxDefaultSize,
                                                   0);
    m_staticText2->Wrap(-1);
#ifdef __WXOSX__
    m_staticText2->SetFont(*wxSMALL_FONT);
#endif
    bSizer4->Add(m_staticText2, 0, wxALL, 5);

    m_TocCtrl1 = new vrViewerTOCList(m_panel1, wxID_ANY);
    bSizer4->Add(m_TocCtrl1->GetControl(), 1, wxEXPAND, 5);

    wxStaticText* m_staticText1 = new wxStaticText(m_panel1, wxID_ANY, "Display 2", wxDefaultPosition, wxDefaultSize,
                                                   0);
    m_staticText1->Wrap(-1);
#ifdef __WXOSX__
    m_staticText1->SetFont(*wxSMALL_FONT);
#endif
    bSizer4->Add(m_staticText1, 0, wxALL, 5);

    m_TocCtrl2 = new vrViewerTOCList(m_panel1, wxID_ANY);
    bSizer4->Add(m_TocCtrl2->GetControl(), 1, wxEXPAND, 5);

    m_panel1->SetSizer(bSizer4);
    m_panel1->Layout();
    bSizer4->Fit(m_panel1);
    wxPanel* m_panel2;
    m_panel2 = new wxPanel(m_splitter2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* bSizer5;
    bSizer5 = new wxBoxSizer(wxHORIZONTAL);

    m_DisplayCtrl1 = new vrViewerDisplay(m_panel2, wxNewId(), *wxBLACK);
    bSizer5->Add(m_DisplayCtrl1, 1, wxEXPAND | wxRIGHT, 1);

    m_DisplayCtrl2 = new vrViewerDisplay(m_panel2, wxNewId(), *wxWHITE);
    bSizer5->Add(m_DisplayCtrl2, 1, wxEXPAND | wxLEFT, 1);

    m_panel2->SetSizer(bSizer5);
    m_panel2->Layout();
    bSizer5->Fit(m_panel2);
    m_splitter2->SplitVertically(m_panel1, m_panel2, 200);
    bSizer1->Add(m_splitter2, 1, wxEXPAND, 5);

    this->SetSizer(bSizer1);
}

vroomTwinFrame::vroomTwinFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title) {
    m_SyncroTool = true;
    // add icon (windows / linux)
    wxIcon myVroomGISIcon;
    myVroomGISIcon.CopyFromBitmap(*_img_vroomgis_sml);
    SetIcon(myVroomGISIcon);

    // MENU
    wxMenu* fileMenu = new wxMenu;
    wxMenu* helpMenu = new wxMenu;
    wxMenu* toolMenu = new wxMenu;

    helpMenu->Append(wxID_ABOUT, "&About...\tF1", "Show about dialog");
    helpMenu->Append(wxID_INFO, "Show Log Window", "Show log window");
    fileMenu->Append(wxID_OPEN, "&Open\tCtrl+O", "Open GIS layer(s)");
    fileMenu->Append(wxID_REMOVE, "&Close\tCtrl+W", "Close GIS layer(s)");
    fileMenu->Append(wxID_EXIT, "E&xit\tAlt-X", "Quit this program");

    // toolMenu->Append(wxID_DEFAULT, "Select\tCtrl+S", "Select the selection tool");
    toolMenu->Append(wxID_ZOOM_IN, "Zoom\tCtrl+Z", "Select the zoom tool");
    toolMenu->Append(wxID_MOVE_FRAME, "Pan\tCtrl+P", "Select the pan tool");
    toolMenu->Append(wxID_ZOOM_FIT, "Zoom to fit\tCtrl+0", "Zoom view to the full extent of all layers");
    toolMenu->Append(vtID_SIGHT_TOOL, "Viewfinder\tCtrl+F", "Select the viewfinder tool");
    toolMenu->AppendSeparator();
    toolMenu->AppendCheckItem(vtID_SET_SYNCRO_MODE, "Syncronize tools",
                              "When set to true, tools action are working on all display");
    toolMenu->Check(vtID_SET_SYNCRO_MODE, m_SyncroTool);

    wxMenu* overlayMenu = new wxMenu();
    overlayMenu->AppendCheckItem(vtID_OVERLAY_TEXT, _("Overlay Text"));

    wxMenuBar* menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(toolMenu, "&Tools");
    menuBar->Append(overlayMenu, _("&Overlay"));
    menuBar->Append(helpMenu, "&Help");

    SetMenuBar(menuBar);

    // STATUS BAR
    CreateStatusBar(2);
    SetStatusText("Welcome to vroomTwin v." + vrVersion::GetVersion() + "(" + vrVersion::GetGitHash() + ")");

    // CONTROLS
    _CreateControls();

    wxLog* myDlgLog = new tmLogGuiSeverity(wxLOG_Warning);
    delete wxLog::SetActiveTarget(myDlgLog);
    m_LogWnd = new wxLogWindow(this, "vroomTwin Log", true, true);

    // DND
    m_TocCtrl1->GetControl()->SetDropTarget(new vroomDropFiles(this));
    m_TocCtrl2->GetControl()->SetDropTarget(new vroomDropFiles(this));

    // VROOMGIS
    m_LayerManager = new vrLayerManager();
    m_ViewerLayerManager1 = new vrViewerLayerManager(m_LayerManager, this, m_DisplayCtrl1, m_TocCtrl1);
    m_ViewerLayerManager2 = new vrViewerLayerManager(m_LayerManager, this, m_DisplayCtrl2, m_TocCtrl2);
}

vroomTwinFrame::~vroomTwinFrame() {
    // don't delete m_ViewerLayerManager, will be deleted by the manager
    wxDELETE(m_LayerManager);
    delete wxLog::SetActiveTarget(nullptr);

    vroomgis_clear_images();
}

void vroomTwinFrame::OnQuit(wxCommandEvent& WXUNUSED(event)) {
    Close(true);
}

void vroomTwinFrame::OnAbout(wxCommandEvent& WXUNUSED(event)) {
    wxMessageDialog myDlg(this, "Dummy about dialog.");
    myDlg.ShowModal();
}

bool vroomTwinFrame::OpenLayers(const wxArrayString& names) {
    for (unsigned int i = 0; i < names.GetCount(); i++) {
        // open files
        bool myOpen = m_LayerManager->Open(wxFileName(names.Item(i)));
        wxASSERT(myOpen);
    }

    m_ViewerLayerManager1->FreezeBegin();
    m_ViewerLayerManager2->FreezeBegin();
    for (unsigned int j = 0; j < names.GetCount(); j++) {
        // get files
        vrLayer* myLayer = m_LayerManager->GetLayer(wxFileName(names.Item(j)));
        wxASSERT(myLayer);

        // add files to the viewer
        m_ViewerLayerManager1->Add(-1, myLayer);
        m_ViewerLayerManager2->Add(-1, myLayer);
    }
    m_ViewerLayerManager2->FreezeEnd();
    m_ViewerLayerManager1->FreezeEnd();
    return true;
}

void vroomTwinFrame::OnOpenLayer(wxCommandEvent& event) {
    /*
    // TODO: This is temp code for autoloading file
    wxFileName myTestFile("/Users/lucien/DATA/PRJ/COLTOP-GIS/test_data/gwn_combioula.shp");
    m_LayerManager->Open(myTestFile);
    vrLayer * myTestLayer = m_LayerManager->GetLayer(myTestFile);
    wxASSERT(myTestLayer);
    m_ViewerLayerManager->Add(-1, myTestLayer);
    return;
     */

    vrDrivers myDrivers;
    wxFileDialog myFileDlg(this, "Select GIS Layers", wxEmptyString, wxEmptyString, myDrivers.GetWildcards(),
                           wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE | wxFD_CHANGE_DIR);

    wxArrayString myPathsFileName;

    if (myFileDlg.ShowModal() != wxID_OK) {
        return;
    }

    // try to open files
    myFileDlg.GetPaths(myPathsFileName);
    wxASSERT(myPathsFileName.GetCount() > 0);

    OpenLayers(myPathsFileName);
}

void vroomTwinFrame::OnCloseLayer(wxCommandEvent& event) {
    wxArrayString myLayersName;
    for (int i = 0; i < m_ViewerLayerManager1->GetCount(); i++) {
        vrRenderer* myRenderer = m_ViewerLayerManager1->GetRenderer(i);
        wxASSERT(myRenderer);
        myLayersName.Add(myRenderer->GetLayer()->GetDisplayName().GetFullName());
    }

    if (myLayersName.IsEmpty()) {
        wxLogError("No layer opened, nothing to close");
        return;
    }

    wxMultiChoiceDialog myChoiceDlg(this, "Select Layer(s) to close", "Close layer(s)", myLayersName);
    if (myChoiceDlg.ShowModal() != wxID_OK) {
        return;
    }

    wxArrayInt myLayerToRemoveIndex = myChoiceDlg.GetSelections();
    if (myLayerToRemoveIndex.IsEmpty()) {
        wxLogWarning("Nothing selected, no layer will be closed");
        return;
    }

    // removing layer(s)
    m_ViewerLayerManager1->FreezeBegin();
    m_ViewerLayerManager2->FreezeBegin();

    for (int j = (signed)myLayerToRemoveIndex.GetCount() - 1; j >= 0; j--) {
        // remove from viewer manager (TOC and Display)
        vrRenderer* myRenderer = m_ViewerLayerManager1->GetRenderer(myLayerToRemoveIndex.Item(j));
        vrLayer* myLayer = myRenderer->GetLayer();
        wxASSERT(myRenderer);
        m_ViewerLayerManager1->Remove(myRenderer);

        vrRenderer* myRenderer2 = m_ViewerLayerManager2->GetRenderer(myLayerToRemoveIndex.Item(j));
        wxASSERT(myRenderer2);
        m_ViewerLayerManager2->Remove(myRenderer2);

        // close layer (not used anymore);
        m_LayerManager->Close(myLayer);
    }
    m_ViewerLayerManager2->FreezeEnd();
    m_ViewerLayerManager1->FreezeEnd();
}

void vroomTwinFrame::OnShowLog(wxCommandEvent& event) {
    m_LogWnd->Show(true);
}

void vroomTwinFrame::OnOverlayText(wxCommandEvent& event) {
    // left panel
    vrViewerOverlayText* myLeftOverlay = (vrViewerOverlayText*)m_DisplayCtrl1->GetOverlayByName(_T("LEFT"));
    if (myLeftOverlay == nullptr) {
        vrViewerOverlayText* myNewOverlay = new vrViewerOverlayText(_T("LEFT"), _("Left panel"));
        myNewOverlay->SetVisible(false);
        m_DisplayCtrl1->GetOverlayArrayRef()->Add(myNewOverlay);
        wxCommandEvent evt;
        return OnOverlayText(evt);
    }

    // right panel
    vrViewerOverlayText* myRightOverlay = (vrViewerOverlayText*)m_DisplayCtrl2->GetOverlayByName(_T("RIGHT"));
    if (myRightOverlay == nullptr) {
        vrViewerOverlayText* myNewOverlay = new vrViewerOverlayText(_T("RIGHT"), _("Right panel"));
        myNewOverlay->SetVisible(false);
        m_DisplayCtrl2->GetOverlayArrayRef()->Add(myNewOverlay);
        wxCommandEvent evt;
        return OnOverlayText(evt);
    }

    myLeftOverlay->SetTextColour(*wxRED);
    myLeftOverlay->SetVisible(!myLeftOverlay->IsVisible());
    myRightOverlay->SetTextColour(*wxGREEN);
    myRightOverlay->SetVisible(!myRightOverlay->IsVisible());
    m_DisplayCtrl1->Refresh();
    m_DisplayCtrl1->Update();
    m_DisplayCtrl2->Refresh();
    m_DisplayCtrl2->Update();
}

void vroomTwinFrame::OnSyncroToolSwitch(wxCommandEvent& event) {
    m_SyncroTool = GetMenuBar()->IsChecked(vtID_SET_SYNCRO_MODE);
}

/*
void vroomTwinFrame::OnToolSelect (wxCommandEvent & event){
        m_DisplayCtrl->SetToolDefault();
}
*/

void vroomTwinFrame::OnToolZoom(wxCommandEvent& event) {
    m_DisplayCtrl1->SetToolZoom();
    m_DisplayCtrl2->SetToolZoom();
}

void vroomTwinFrame::OnToolPan(wxCommandEvent& event) {
    m_DisplayCtrl1->SetToolPan();
    m_DisplayCtrl2->SetToolPan();
}

void vroomTwinFrame::OnToolSight(wxCommandEvent& event) {
    m_DisplayCtrl1->SetToolSight();
    m_DisplayCtrl2->SetToolSight();
}

void vroomTwinFrame::OnToolZoomToFit(wxCommandEvent& event) {
    m_ViewerLayerManager1->ZoomToFit(false);
    m_ViewerLayerManager1->Reload();
    m_ViewerLayerManager2->ZoomToFit(false);
    m_ViewerLayerManager2->Reload();
}

void vroomTwinFrame::OnToolAction(wxCommandEvent& event) {
    vrDisplayToolMessage* myMsg = (vrDisplayToolMessage*)event.GetClientData();
    wxASSERT(myMsg);

    if (myMsg->m_evtType == vrEVT_TOOL_ZOOM) {
        // getting rectangle
        vrCoordinate* myCoord = myMsg->m_parentManager->GetDisplay()->GetCoordinate();
        wxASSERT(myCoord);

        // get real rectangle
        vrRealRect myRealRect;
        bool bSuccess = myCoord->ConvertFromPixels(myMsg->m_rect, myRealRect);
        wxASSERT(bSuccess);

        // get fitted rectangle
        vrRealRect myFittedRect = myCoord->GetRectFitted(myRealRect);
        wxASSERT(myFittedRect.IsOk());

        if (!m_SyncroTool) {
            myMsg->m_parentManager->Zoom(myFittedRect);
            myMsg->m_parentManager->Reload();
        } else {
            m_ViewerLayerManager1->Zoom(myFittedRect);
            m_ViewerLayerManager1->Reload();
            m_ViewerLayerManager2->Zoom(myFittedRect);
            m_ViewerLayerManager2->Reload();
        }
    }

    else if (myMsg->m_evtType == vrEVT_TOOL_PAN) {
        vrCoordinate* myCoord = myMsg->m_parentManager->GetDisplay()->GetCoordinate();
        wxASSERT(myCoord);

        wxPoint myMovedPos = myMsg->m_position;
        wxPoint2DDouble myMovedRealPt;
        if (!myCoord->ConvertFromPixels(myMovedPos, myMovedRealPt)) {
            wxLogError("Error converting point : %d, %d to real coordinate", myMovedPos.x, myMovedPos.y);
            wxDELETE(myMsg);
            return;
        }

        vrRealRect myActExtent = myCoord->GetExtent();
        myActExtent.MoveLeftTopTo(myMovedRealPt);

        if (!m_SyncroTool) {
            myCoord->SetExtent(myActExtent);
            myMsg->m_parentManager->Reload();
        } else {
            m_ViewerLayerManager1->GetDisplay()->GetCoordinate()->SetExtent(myActExtent);
            m_ViewerLayerManager1->Reload();
            m_ViewerLayerManager2->GetDisplay()->GetCoordinate()->SetExtent(myActExtent);
            m_ViewerLayerManager2->Reload();
        }

    } else if (myMsg->m_evtType == vrEVT_TOOL_SIGHT) {
        vrViewerLayerManager* myInvertedMgr = m_ViewerLayerManager1;
        if (myInvertedMgr == myMsg->m_parentManager) {
            myInvertedMgr = m_ViewerLayerManager2;
        }

        {
            wxClientDC myDC(myInvertedMgr->GetDisplay());
            wxDCOverlay overlaydc(m_Overlay, &myDC);
            overlaydc.Clear();
        }
        m_Overlay.Reset();

        if (myMsg->m_position != wxDefaultPosition) {
            wxClientDC myDC(myInvertedMgr->GetDisplay());
            wxDCOverlay overlaydc(m_Overlay, &myDC);
            overlaydc.Clear();
            myDC.SetPen(*wxGREEN_PEN);
            myDC.CrossHair(myMsg->m_position);
        }

    }

    /* else if (myMsg->m_evtType == vrEVT_TOOL_SELECT) {
        vrCoordinate * myCoord = m_ViewerLayerManager->GetDispaly()->GetCoordinate();
        wxASSERT(myCoord);

        wxPoint myClickedPos = myMsg->m_Position;
        if (myClickedPos != wxDefaultPosition) {
            wxPoint2DDouble myRealClickedPos;
            myCoord->ConvertFromPixels(myClickedPos, myRealClickedPos);
            wxMessageBox(wxString::Format("Selected coordinate :\nx:\t%.4f\ny:\t%.4f",
                                          myRealClickedPos.m_x,
                                          myRealClickedPos.m_y),
                         "Selected coordinate");
        }

    }
    */

    else {
        wxLogError("Operation not supported now");
    }
    wxDELETE(myMsg);
}
