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
#include "../../../vroomgis/art/vroomgis_toolbmp.cpp"
#include "../art/vroomdrawer_toolbmp.h"
#include "frame.h"
#include "tmlog.h"  // for double logging process
#include "vrlayervector.h"
#include "vrlayervectorstar.h"
#include "vrmemorylayerdialog.h"
#include "vrrendervectorc2p.h"

IMPLEMENT_APP(vroomDrawer);

vroomDrawer::vroomDrawer() {}

bool vroomDrawer::OnInit() {
#ifdef __WXDEBUG__
    // debugging string for OSX
    // this is needed for viewing string content with Xcode !!
    wxString myTest = _T("Test debugging");
    myTest.ToUTF8().data();
#endif

    if (!wxApp::OnInit()) {
        return false;
    }

    wxInitAllImageHandlers();
    vroomgis_initialize_images();
    vroomgis_initialize_images_toolbar();
    vroomdrawer_initialize_images();

    vroomDrawerFrame* frame = new vroomDrawerFrame("vroomDrawer");
    frame->SetSize(50, 50, 800, 500);
    frame->Show(true);
    return true;
}

void vroomDrawer::OnFatalException() {}

BEGIN_EVENT_TABLE(vroomDrawerFrame, wxFrame)
EVT_MENU(wxID_EXIT, vroomDrawerFrame::OnQuit)
EVT_MENU(wxID_ABOUT, vroomDrawerFrame::OnAbout)

EVT_MENU(wxID_ADD, vroomDrawerFrame::OnLayerAdd)
EVT_MENU(wxID_REMOVE, vroomDrawerFrame::OnLayerRemove)
EVT_MENU(wxID_ZOOM_IN, vroomDrawerFrame::OnToolZoom)
EVT_MENU(ID_MENU_SELECT, vroomDrawerFrame::OnToolSelect)
EVT_MENU(ID_MENU_PAN, vroomDrawerFrame::OnToolPan)
EVT_MENU(wxID_ZOOM_FIT, vroomDrawerFrame::OnZoomToFit)
EVT_MENU(wxID_INFO, vroomDrawerFrame::OnShowLog)
EVT_MENU(wxID_DELETE, vroomDrawerFrame::OnTestCrashSoftware)
EVT_MENU(ID_MENU_ADDMEMORYLAYER, vroomDrawerFrame::OnMemoryLayerAdd)

EVT_COMMAND(wxID_ANY, vrEVT_TOOL_ZOOM, vroomDrawerFrame::OnToolActionZoom)
EVT_COMMAND(wxID_ANY, vrEVT_TOOL_ZOOMOUT, vroomDrawerFrame::OnToolActionZoom)
EVT_COMMAND(wxID_ANY, vrEVT_TOOL_SELECT, vroomDrawerFrame::OnToolActionSelect)
EVT_COMMAND(wxID_ANY, vrEVT_TOOL_PAN, vroomDrawerFrame::OnToolActionPan)
END_EVENT_TABLE()

void vroomDrawerFrame::_CreateControls() {
    this->SetSizeHints(wxDefaultSize, wxDefaultSize);

    wxBoxSizer* bSizer1;
    bSizer1 = new wxBoxSizer(wxVERTICAL);

    wxSplitterWindow* m_splitter2;
    m_splitter2 = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                       wxSP_BORDER | wxSP_LIVE_UPDATE);

    wxPanel* m_panel1;
    m_panel1 = new wxPanel(m_splitter2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* bSizer4;
    bSizer4 = new wxBoxSizer(wxVERTICAL);

    m_TocCtrl = new vrViewerTOCTree(m_panel1, wxID_ANY);
    bSizer4->Add(m_TocCtrl->GetControl(), 1, wxEXPAND, 5);

    m_panel1->SetSizer(bSizer4);
    m_panel1->Layout();
    bSizer4->Fit(m_panel1);
    wxPanel* m_panel2;
    m_panel2 = new wxPanel(m_splitter2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* bSizer5;
    bSizer5 = new wxBoxSizer(wxVERTICAL);

    m_DisplayCtrl = new vrViewerDisplay(m_panel2, wxID_ANY, wxColour(120, 120, 120));
    bSizer5->Add(m_DisplayCtrl, 1, wxEXPAND, 5);

    m_panel2->SetSizer(bSizer5);
    m_panel2->Layout();
    bSizer5->Fit(m_panel2);
    m_splitter2->SplitVertically(m_panel1, m_panel2, 200);
    bSizer1->Add(m_splitter2, 1, wxEXPAND, 5);

    this->SetSizer(bSizer1);
    this->Layout();
    wxMenuBar* m_menubar1;
    m_menubar1 = new wxMenuBar(0);
    wxMenu* myFileMenu;
    myFileMenu = new wxMenu();
    myFileMenu->Append(wxID_ADD, _("Add Layer..."));
    myFileMenu->Append(ID_MENU_ADDMEMORYLAYER, _("Add memory layer..."));

    wxMenuItem* m_menuItem2;
    m_menuItem2 = new wxMenuItem(myFileMenu, wxID_REMOVE, wxString(_("Remove Layer...")), wxEmptyString, wxITEM_NORMAL);
    myFileMenu->Append(m_menuItem2);
    myFileMenu->AppendSeparator();
    myFileMenu->Append(wxID_INFO, _("Show Log...\tCtrl+L"));
    myFileMenu->AppendSeparator();
    myFileMenu->Append(wxID_DELETE, _("Generate crash..."));
    myFileMenu->Append(wxID_ABOUT, _("About"));
    myFileMenu->Append(wxID_EXIT, _("Quit"));

    m_menubar1->Append(myFileMenu, _("File"));

    wxMenu* myToolMenu;
    myToolMenu = new wxMenu();
    wxMenuItem* m_menuItem3;
    m_menuItem3 = new wxMenuItem(myToolMenu, wxID_ZOOM_IN, wxString(_("Zoom")) + wxT('\t') + wxT("Ctrl+Z"),
                                 wxEmptyString, wxITEM_NORMAL);
    myToolMenu->Append(m_menuItem3);

    wxMenuItem* m_menuItem4;
    m_menuItem4 = new wxMenuItem(myToolMenu, ID_MENU_SELECT, wxString(_("Select")) + wxT('\t') + wxT("Ctrl+S"),
                                 wxEmptyString, wxITEM_NORMAL);
    myToolMenu->Append(m_menuItem4);

    wxMenuItem* m_menuItem5;
    m_menuItem5 = new wxMenuItem(myToolMenu, ID_MENU_PAN, wxString(_("Pan")) + wxT('\t') + wxT("Ctrl+P"), wxEmptyString,
                                 wxITEM_NORMAL);
    myToolMenu->Append(m_menuItem5);

    wxMenuItem* m_separator1;
    m_separator1 = myToolMenu->AppendSeparator();

    wxMenuItem* m_menuItem6;
    m_menuItem6 = new wxMenuItem(myToolMenu, wxID_ZOOM_FIT, wxString(_("Zoom to fit")) + wxT('\t') + wxT("Ctrl+0"),
                                 wxEmptyString, wxITEM_NORMAL);
    myToolMenu->Append(m_menuItem6);

    m_menubar1->Append(myToolMenu, _("Tools"));

    this->SetMenuBar(m_menubar1);
    this->Centre(wxBOTH);
}

double vroomDrawerFrame::_GetRandomNumber(double min, double max) {
    double multiply = ((double)rand() / (double)RAND_MAX);
    double myNumber = (max - min) * multiply;
    return myNumber + min;
}

vrLayer* vroomDrawerFrame::_GetMemoryLayerPoints(const wxFileName& name, int number, const vrRealRect& extent) {
    vrLayerVectorOGR* myLayer = new vrLayerVectorOGR();
    if (!myLayer->Create(name, wkbPoint)) {
        wxFAIL;
        return nullptr;
    }

    // adding features to the layer
    for (int i = 0; i < number; i++) {
        OGRPoint myPt;
        myPt.setX(_GetRandomNumber(extent.GetLeft(), extent.GetRight()));
        myPt.setY(_GetRandomNumber(extent.GetBottom(), extent.GetTop()));
        myLayer->AddFeature(&myPt, nullptr);
    }
    return myLayer;
}

void vroomDrawerFrame::_CreateToolbar() {
    long myStyle = wxTB_FLAT | wxTB_HORIZONTAL;
    // conditionnal compilation for better look under win32
#ifndef __WXMSW__
    myStyle += wxTB_TEXT;
#endif

    wxToolBar* m_toolBar1;
    m_toolBar1 = this->CreateToolBar(myStyle, wxID_ANY);
    m_toolBar1->SetToolBitmapSize(wxSize(32, 32));
    wxString mySelectName = _("Select");
    m_toolBar1->AddTool(ID_MENU_SELECT, mySelectName, wxBitmap(*_img_toolbar_select), wxNullBitmap, wxITEM_NORMAL,
                        mySelectName, wxEmptyString);
    wxString myZoom2Name = _("Zoom");
    m_toolBar1->AddTool(wxID_ZOOM_IN, myZoom2Name, wxBitmap(*_img_toolbar_zoom), wxNullBitmap, wxITEM_NORMAL,
                        myZoom2Name, wxEmptyString);
    wxString myPanName = _("Pan");
    m_toolBar1->AddTool(ID_MENU_PAN, myPanName, wxBitmap(*_img_toolbar_pan), wxNullBitmap, wxITEM_NORMAL, myPanName,
                        wxEmptyString);
    wxString myDataManagerName = _("Data Manager");
    wxString myZoomName = _("Zoom to fit");
    m_toolBar1->AddTool(wxID_ZOOM_FIT, myZoomName, wxBitmap(*_img_toolbar_zoomfull), wxNullBitmap, wxITEM_NORMAL,
                        myZoomName, wxEmptyString);

    m_toolBar1->AddSeparator();
    wxString myAddName = _("Add memory layer...");
    m_toolBar1->AddTool(ID_MENU_ADDMEMORYLAYER, myAddName, wxBitmap(*_img_toolbar_add_memory), wxNullBitmap,
                        wxITEM_NORMAL, myAddName, wxEmptyString);

    m_toolBar1->Realize();
}

vrLayer* vroomDrawerFrame::_GetMemoryLayerLine(const wxFileName& name, int number, const vrRealRect& extent) {
    vrLayerVectorOGR* myLayer = new vrLayerVectorOGR();
    if (!myLayer->Create(name, wkbLineString)) {
        wxFAIL;
        return nullptr;
    }

    // adding features to the layer
    OGRLineString* myLine = (OGRLineString*)OGRGeometryFactory::createGeometry(wkbLineString);
    for (int i = 0; i < number; i++) {
        myLine->addPoint(_GetRandomNumber(extent.GetLeft(), extent.GetRight()),
                         _GetRandomNumber(extent.GetBottom(), extent.GetTop()));
        if (myLine->getNumPoints() >= 10) {
            myLayer->AddFeature(myLine, nullptr);
            OGRGeometryFactory::destroyGeometry(myLine);
            myLine = (OGRLineString*)OGRGeometryFactory::createGeometry(wkbLineString);
        }
    }
    if (myLine->getNumPoints() > 2) {
        myLayer->AddFeature(myLine, nullptr);
    }
    OGRGeometryFactory::destroyGeometry(myLine);
    return myLayer;
}

vrLayer* vroomDrawerFrame::_GetMemoryLayerStar(const wxFileName& name, int number, const vrRealRect& extent) {
    vrLayerVectorStar* myLayer = new vrLayerVectorStar();
    if (!myLayer->Create(name, wkbPoint)) {
        wxFAIL;
        return nullptr;
    }
    // adding size field
    OGRFieldDefn myFieldStarSize("starsize", OFTInteger);
    myLayer->AddField(myFieldStarSize);

    // adding features to the layer
    for (int i = 0; i < number; i++) {
        int myStarSize = wxRound(_GetRandomNumber(3.0, 20.0));
        OGRPoint myPt;
        myPt.setX(_GetRandomNumber(extent.GetLeft(), extent.GetRight()));
        myPt.setY(_GetRandomNumber(extent.GetBottom(), extent.GetTop()));
        myLayer->AddFeature(&myPt, &myStarSize);
    }
    return myLayer;
}

vroomDrawerFrame::vroomDrawerFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title) {
    wxIcon myVroomGISIcon;
    myVroomGISIcon.CopyFromBitmap(*_img_vroomgis_sml);
    SetIcon(myVroomGISIcon);

    // STATUS BAR
    CreateStatusBar(2);
    SetStatusText("Welcome to vroomDrawer v" + vrVersion::GetVersion() + "(" + vrVersion::GetGitHash() + ")");

    // CONTROLS
    _CreateControls();
    _CreateToolbar();

    wxLog* myDlgLog = new tmLogGuiSeverity(wxLOG_Warning);
    delete wxLog::SetActiveTarget(myDlgLog);
    m_LogWnd = new wxLogWindow(this, "vroomDrawer Log", true, true);

    // Connect Events
    m_DisplayCtrl->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(vroomDrawerFrame::OnKeyDown), nullptr, this);
    m_DisplayCtrl->Connect(wxEVT_KEY_UP, wxKeyEventHandler(vroomDrawerFrame::OnKeyUp), nullptr, this);

    // VROOMGIS
    m_LayerManager = new vrLayerManager();
    m_ViewerLayerManager = new vrViewerLayerManager(m_LayerManager, this, m_DisplayCtrl, m_TocCtrl);
    m_DisplayCtrl->SetStatusCoordinates(GetStatusBar(), 1, _("Coordinates not available"));
}

vroomDrawerFrame::~vroomDrawerFrame() {
    // Disconnect Events
    m_DisplayCtrl->Disconnect(wxEVT_KEY_DOWN, wxKeyEventHandler(vroomDrawerFrame::OnKeyDown), nullptr, this);
    m_DisplayCtrl->Disconnect(wxEVT_KEY_UP, wxKeyEventHandler(vroomDrawerFrame::OnKeyUp), nullptr, this);

    // don't delete m_ViewerLayerManager, will be deleted by the manager
    wxDELETE(m_LayerManager);
    delete wxLog::SetActiveTarget(nullptr);
    vroomgis_clear_images();
}

void vroomDrawerFrame::OnQuit(wxCommandEvent& event) {
    Close();
}

void vroomDrawerFrame::OnAbout(wxCommandEvent& WXUNUSED(event)) {
    wxMessageDialog myDlg(this, "Dummy about dialog.");
    myDlg.ShowModal();
}

bool vroomDrawerFrame::OpenLayers(const wxArrayString& names) {
    for (unsigned int i = 0; i < names.GetCount(); i++) {
        // open files
        bool myOpen = m_LayerManager->Open(wxFileName(names.Item(i)));
        wxASSERT(myOpen);
    }

    m_ViewerLayerManager->FreezeBegin();
    for (unsigned int j = 0; j < names.GetCount(); j++) {
        // get files
        vrLayer* myLayer = m_LayerManager->GetLayer(wxFileName(names.Item(j)));
        wxASSERT(myLayer);

        // if type is DIPS
        vrRenderVectorC2PDips* myRender = nullptr;
        if (myLayer->GetType() == vrDRIVER_VECTOR_C2P) {
            // change default colour;
            myRender = new vrRenderVectorC2PDips(*wxBLUE);
            myRender->SetUsingDefaultColour(false);
            // adding support for familly 2
            myRender->AddDipColour(*wxRED, 10, true);
        }

        // add files to the viewer
        m_ViewerLayerManager->Add(-1, myLayer, myRender);
    }
    m_ViewerLayerManager->FreezeEnd();
    return true;
}

void vroomDrawerFrame::OnLayerAdd(wxCommandEvent& WXUNUSED(event)) {
    vrDrivers myDrivers;
    wxFileDialog myFileDlg(this, "Select GIS Layers", wxEmptyString, wxEmptyString, myDrivers.GetWildcards(),
                           wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE | wxFD_CHANGE_DIR);
    wxArrayString myPathsFileName;
    if (myFileDlg.ShowModal() == wxID_CANCEL) {
        return;
    }
    myFileDlg.GetPaths(myPathsFileName);
    wxASSERT(myPathsFileName.GetCount() > 0);
    OpenLayers(myPathsFileName);
}

void vroomDrawerFrame::OnLayerRemove(wxCommandEvent& event) {
    wxArrayString myLayersName;
    for (int i = 0; i < m_ViewerLayerManager->GetCount(); i++) {
        vrRenderer* myRenderer = m_ViewerLayerManager->GetRenderer(i);
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
    m_ViewerLayerManager->FreezeBegin();
    for (int j = (signed)myLayerToRemoveIndex.GetCount() - 1; j >= 0; j--) {
        // remove from viewer manager (TOC and Display)
        vrRenderer* myRenderer = m_ViewerLayerManager->GetRenderer(myLayerToRemoveIndex.Item(j));
        vrLayer* myLayer = myRenderer->GetLayer();
        wxASSERT(myRenderer);
        m_ViewerLayerManager->Remove(myRenderer);

        // close layer (not used anymore);
        m_LayerManager->Close(myLayer);
    }

    m_ViewerLayerManager->FreezeEnd();
}

void vroomDrawerFrame::OnShowLog(wxCommandEvent& event) {
    m_LogWnd->Show(true);
}

void vroomDrawerFrame::OnKeyDown(wxKeyEvent& event) {
    m_KeyBoardState = wxKeyboardState(event.ControlDown(), event.ShiftDown(), event.AltDown(), event.MetaDown());
    if (m_KeyBoardState.GetModifiers() != wxMOD_CMD) {
        event.Skip();
        return;
    }

    const vrDisplayTool* myTool = m_DisplayCtrl->GetTool();
    if (myTool == nullptr) {
        event.Skip();
        return;
    }

    if (myTool->GetID() == wxID_ZOOM_IN) {
        m_DisplayCtrl->SetToolZoomOut();
    }
    event.Skip();
}

void vroomDrawerFrame::OnKeyUp(wxKeyEvent& event) {
    if (m_KeyBoardState.GetModifiers() != wxMOD_CMD) {
        event.Skip();
        return;
    }

    const vrDisplayTool* myTool = m_DisplayCtrl->GetTool();
    if (myTool == nullptr) {
        event.Skip();
        return;
    }

    if (myTool->GetID() == wxID_ZOOM_OUT || myTool->GetID() == wxID_ZOOM_IN) {
        m_DisplayCtrl->SetToolZoom();
    }
    event.Skip();
}

void vroomDrawerFrame::OnToolSelect(wxCommandEvent& event) {
    m_DisplayCtrl->SetToolDefault();
    event.Skip();
}

void vroomDrawerFrame::OnToolZoom(wxCommandEvent& event) {
    m_DisplayCtrl->SetToolZoom();
    event.Skip();
}

void vroomDrawerFrame::OnToolPan(wxCommandEvent& event) {
    m_DisplayCtrl->SetToolPan();
    event.Skip();
}

void vroomDrawerFrame::OnZoomToFit(wxCommandEvent& event) {
    m_ViewerLayerManager->ZoomToFit(true);
    m_ViewerLayerManager->Reload();
    event.Skip();
}

// crash function for better stack trace
static void bar(const char* p) {
    char* pc = 0;
    *pc = *p;
    printf("bar: %s\n", p);
}

void foo(int n) {
    bar("even");
}

void vroomDrawerFrame::OnTestCrashSoftware(wxCommandEvent& event) {
    foo(31);
}

void vroomDrawerFrame::OnToolActionZoom(wxCommandEvent& event) {
    vrDisplayToolMessage* myMsg = (vrDisplayToolMessage*)event.GetClientData();
    wxASSERT(myMsg);

    vrCoordinate* myCoord = m_ViewerLayerManager->GetDisplay()->GetCoordinate();
    wxASSERT(myCoord);

    // get real rectangle
    vrRealRect myRealRect;
    bool bSuccess = myCoord->ConvertFromPixels(myMsg->m_rect, myRealRect);
    wxASSERT(bSuccess);

    // get fitted rectangle
    vrRealRect myFittedRect = myCoord->GetRectFitted(myRealRect);
    wxASSERT(myFittedRect.IsOk());

    // moving view
    if (myMsg->m_evtType == vrEVT_TOOL_ZOOM) {
        m_ViewerLayerManager->Zoom(myFittedRect);
    } else {
        m_ViewerLayerManager->ZoomOut(myFittedRect);
    }
    wxDELETE(myMsg);
}

void vroomDrawerFrame::OnToolActionSelect(wxCommandEvent& event) {
    vrDisplayToolMessage* myMsg = (vrDisplayToolMessage*)event.GetClientData();
    wxASSERT(myMsg);

    vrCoordinate* myCoord = m_ViewerLayerManager->GetDisplay()->GetCoordinate();
    wxASSERT(myCoord);

    wxPoint myClickedPos = myMsg->m_position;
    if (myClickedPos != wxDefaultPosition) {
        wxPoint2DDouble myRealClickedPos;
        myCoord->ConvertFromPixels(myClickedPos, myRealClickedPos);
        wxMessageBox(
            wxString::Format("Selected coordinate :\nx:\t%.4f\ny:\t%.4f", myRealClickedPos.m_x, myRealClickedPos.m_y),
            "Selected coordinate");
    }

    wxDELETE(myMsg);
}

void vroomDrawerFrame::OnToolActionPan(wxCommandEvent& event) {
    vrDisplayToolMessage* myMsg = (vrDisplayToolMessage*)event.GetClientData();
    wxASSERT(myMsg);

    vrCoordinate* myCoord = m_ViewerLayerManager->GetDisplay()->GetCoordinate();
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
    myCoord->SetExtent(myActExtent);
    m_ViewerLayerManager->Reload();

    wxDELETE(myMsg);
}

void vroomDrawerFrame::OnMemoryLayerAdd(wxCommandEvent& event) {
    vrMemoryLayerDialog myDlg(this, m_LayerManager);
    if (myDlg.ShowModal() != wxID_OK) {
        return;
    }

    // add layer here
    wxFileName myMemoryLayerName("", myDlg.GetLayerName(), "memory");
    wxASSERT(myMemoryLayerName.GetExt() == "memory");

    // check if memory layer already added
    m_ViewerLayerManager->FreezeBegin();
    for (int i = 0; i < m_ViewerLayerManager->GetCount(); i++) {
        if (m_ViewerLayerManager->GetRenderer(i)->GetLayer()->GetFileName() == myMemoryLayerName) {
            vrRenderer* myRenderer = m_ViewerLayerManager->GetRenderer(i);
            vrLayer* myLayer = myRenderer->GetLayer();
            wxASSERT(myRenderer);
            m_ViewerLayerManager->Remove(myRenderer);

            // close layer (not used anymore);
            m_LayerManager->Close(myLayer);
        }
    }

    // if extent not specified, init extent with window size
    vrRealRect myExtent = m_DisplayCtrl->GetCoordinate()->GetExtent();
    if (!myExtent.IsOk()) {
        m_ViewerLayerManager->InitializeExtent(vrRealRect(0, m_DisplayCtrl->GetSize().GetHeight(),
                                                          m_DisplayCtrl->GetSize().GetWidth(),
                                                          m_DisplayCtrl->GetSize().GetHeight() * -1.0));
        myExtent = m_DisplayCtrl->GetCoordinate()->GetExtent();
    }

    // init random engine
    srand((unsigned)time(nullptr));

    vrLayer* myLayer = nullptr;
    vrRenderVector* myRender = new vrRenderVector();

    switch (myDlg.GetFeatureType()) {
        case 1:  // points
            myLayer = _GetMemoryLayerLine(myMemoryLayerName, myDlg.GetFeatureNumber(), myExtent);
            break;

        case 2:  // stars
            myLayer = _GetMemoryLayerStar(myMemoryLayerName, myDlg.GetFeatureNumber(), myExtent);
            myRender->SetSize(1);
            myRender->SetColorPen(*wxBLUE);
            break;

        default:  // points
            myLayer = _GetMemoryLayerPoints(myMemoryLayerName, myDlg.GetFeatureNumber(), myExtent);
            myRender->SetSize(4);
            myRender->SetColorPen(*wxGREEN);
            break;
    }
    wxASSERT(myLayer);
    m_LayerManager->Add(myLayer);
    // vrLayer * myMemoryLayer =  m_LayerManager->GetLayer(myMemoryLayerName);
    m_ViewerLayerManager->Add(-1, myLayer, myRender);
    m_ViewerLayerManager->FreezeEnd();
}
