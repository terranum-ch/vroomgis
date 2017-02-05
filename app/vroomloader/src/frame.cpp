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
#include <vld.h> // Visual Leak Detector (https://vld.codeplex.com/)
#endif

#include "frame.h"
#include "tmlog.h"              // for double logging process
#include "vrdisplayvalue.h"     // for displaying raster values
#include <wxhgversion_dlg.h>
#include "vrrendervectorc2p.h"
#include "vrlayervector.h"
#include "vrshapeeditor.h"
#include "vrdisplaytool.h"
#include "vroomgis_bmp.h"

IMPLEMENT_APP(vroomLoader);

bool vroomLoader::OnInit()
{
    if (!wxApp::OnInit())
        return false;

    wxInitAllImageHandlers();
    vroomgis_initialize_images();

    vroomLoaderFrame *frame = new vroomLoaderFrame("vroomLoader");
    //frame->CenterOnScreen(wxBOTH);
    frame->SetSize(50, 50, 800, 500);
    frame->Show(true);

    return true;
}


BEGIN_EVENT_TABLE(vroomLoaderFrame, wxFrame)
    EVT_MENU(wxID_EXIT, vroomLoaderFrame::OnQuit) EVT_MENU(wxID_ABOUT, vroomLoaderFrame::OnAbout)
    EVT_MENU(wxID_OPEN, vroomLoaderFrame::OnOpenLayer) EVT_MENU(wxID_REMOVE, vroomLoaderFrame::OnCloseLayer)
    EVT_MENU (wxID_INFO, vroomLoaderFrame::OnShowLog)
    EVT_MENU (wxID_DEFAULT, vroomLoaderFrame::OnToolSelect)
    EVT_MENU (wxID_ZOOM_IN, vroomLoaderFrame::OnToolZoom)
    EVT_MENU (wxID_ZOOM_FIT, vroomLoaderFrame::OnToolZoomToFit)
    EVT_MENU (wxID_ZOOM_100, vroomLoaderFrame::OnToolZoomToFit)
    EVT_MENU (wxID_MOVE_FRAME, vroomLoaderFrame::OnToolPan)
    EVT_MENU (vlID_MOVE_LAYER, vroomLoaderFrame::OnMoveLayer)
    EVT_MENU (vlID_DISPLAY_VALUE, vroomLoaderFrame::OnToolDisplayValue)
    EVT_MENU (vlID_PERFORMANCE, vroomLoaderFrame::OnLogPerformance)
    EVT_MENU (vlID_THREADED, vroomLoaderFrame::OnEngineThreaded) EVT_KEY_DOWN(vroomLoaderFrame::OnKeyDown)
    EVT_KEY_UP(vroomLoaderFrame::OnKeyUp)

    EVT_COMMAND(wxID_ANY, vrEVT_TOOL_ZOOM, vroomLoaderFrame::OnToolAction)
    EVT_COMMAND(wxID_ANY, vrEVT_TOOL_ZOOMOUT, vroomLoaderFrame::OnToolAction)
    EVT_COMMAND(wxID_ANY, vrEVT_TOOL_SELECT, vroomLoaderFrame::OnToolAction)
    EVT_COMMAND(wxID_ANY, vrEVT_TOOL_PAN, vroomLoaderFrame::OnToolAction)
    EVT_COMMAND(wxID_ANY, vrEVT_TOOL_EDIT, vroomLoaderFrame::OnToolDrawAction)
    EVT_COMMAND(wxID_ANY, vrEVT_TOOL_EDIT_FINISHED, vroomLoaderFrame::OnToolDrawAction)

    EVT_COMMAND(wxID_ANY, vrEVT_TOOL_MODIFY, vroomLoaderFrame::OnToolModifySearch)
    EVT_COMMAND(wxID_ANY, vrEVT_TOOL_MODIFY_FINISHED, vroomLoaderFrame::OnToolModifyUpdate)

    EVT_TOGGLEBUTTON(vlID_EDIT_BTN, vroomLoaderFrame::OnStartEditingButton)
    EVT_UPDATE_UI(vlID_EDIT_CHOICE, vroomLoaderFrame::OnUpdateUIEditType)
    EVT_MENU(vlID_DRAW_MENU, vroomLoaderFrame::OnToolDraw)
    EVT_MENU(vlID_MODIFY_MENU, vroomLoaderFrame::OnToolModify)

    EVT_UPDATE_UI_RANGE(vlID_DRAW_MENU, vlID_MODIFY_MENU,  vroomLoaderFrame::OnUpdateUIDrawMenu)
END_EVENT_TABLE()


vroomDropFiles::vroomDropFiles(vroomLoaderFrame *parent)
{
    wxASSERT(parent);
    m_LoaderFrame = parent;
}


bool vroomDropFiles::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString &filenames)
{
    if (filenames.GetCount() == 0) {
        return false;
    }

    m_LoaderFrame->OpenLayers(filenames);
    return true;
}


void  vroomLoaderFrame::_CreateControls()
{
    wxBoxSizer *bSizer1;
    bSizer1 = new wxBoxSizer(wxVERTICAL);

    wxSplitterWindow *m_splitter2;
    m_splitter2 = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                       wxSP_BORDER | wxSP_LIVE_UPDATE);
    wxPanel *m_panel1;
    m_panel1 = new wxPanel(m_splitter2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer *bSizer4;
    bSizer4 = new wxBoxSizer(wxVERTICAL);

    // Edition panel
    wxBoxSizer *bSizer41;
    bSizer41 = new wxBoxSizer(wxHORIZONTAL);

    wxString m_editTypeCtrlChoices[] = {_("Points"), _("Lines"), _("Polygons")};
    int m_editTypeCtrlNChoices = sizeof(m_editTypeCtrlChoices) / sizeof(wxString);
    m_editTypeCtrl = new wxChoice(m_panel1, vlID_EDIT_CHOICE, wxDefaultPosition, wxDefaultSize, m_editTypeCtrlNChoices,
                                  m_editTypeCtrlChoices, 0);
    m_editTypeCtrl->SetSelection(0);
    bSizer41->Add(m_editTypeCtrl, 1, wxALL | wxALIGN_CENTER_VERTICAL, 5);

    m_editStartCtrl = new wxToggleButton(m_panel1, vlID_EDIT_BTN, _("Start editing"));
    bSizer41->Add(m_editStartCtrl, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    bSizer4->Add(bSizer41, 0, wxEXPAND, 5);


    m_TocCtrl = new vrViewerTOCTree(m_panel1, wxID_ANY);
    bSizer4->Add(m_TocCtrl->GetControl(), 2, wxEXPAND, 5);
    m_TocCtrl->SetUseGroupMenu(true);

    m_panel1->SetSizer(bSizer4);
    m_panel1->Layout();
    bSizer4->Fit(m_panel1);
    wxPanel *m_panel2;
    m_panel2 = new wxPanel(m_splitter2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer *bSizer5;
    bSizer5 = new wxBoxSizer(wxVERTICAL);

    m_DisplayCtrl = new vrViewerDisplay(m_panel2, wxID_ANY, wxColour(120, 120, 120));

    bSizer5->Add(m_DisplayCtrl, 1, wxEXPAND, 5);

    m_panel2->SetSizer(bSizer5);
    m_panel2->Layout();
    bSizer5->Fit(m_panel2);
    m_splitter2->SplitVertically(m_panel1, m_panel2, 300);
    bSizer1->Add(m_splitter2, 1, wxEXPAND, 5);

    this->SetSizer(bSizer1);
}


vroomLoaderFrame::vroomLoaderFrame(const wxString &title)
        : wxFrame(NULL, wxID_ANY, title)
{
    m_PerfLogFile = wxFileName(wxGetHomeDir(), "vroomgis-performance.txt");

    wxIcon myVroomGISIcon;
    myVroomGISIcon.CopyFromBitmap(*_img_vroomgis_sml);
    SetIcon(myVroomGISIcon);

    m_DisplayValueDlg = NULL;
    m_editor = NULL;

    // MENU
    wxMenu *fileMenu = new wxMenu;
    wxMenu *helpMenu = new wxMenu;
    wxMenu *toolMenu = new wxMenu;

    helpMenu->Append(wxID_ABOUT, "&About...\tF1", "Show about dialog");
    helpMenu->Append(wxID_INFO, "Show Log Window", "Show log window");
    fileMenu->Append(wxID_OPEN, "", "Open GIS layer(s)");
    fileMenu->Append(wxID_REMOVE, "&Close\tCtrl+W", "Close GIS layer(s)");
    fileMenu->Append(wxID_EXIT);

    toolMenu->Append(wxID_DEFAULT, "Select\tCtrl+S", "Select the selection tool");
    toolMenu->Append(wxID_ZOOM_IN, "Zoom\tCtrl+Z", "Select the zoom tool");
    toolMenu->Append(wxID_MOVE_FRAME, "Pan\tCtrl+P", "Select the pan tool");
    toolMenu->AppendSeparator();
    toolMenu->Append(vlID_DRAW_MENU, _("Draw\tCtrl+D"), _("Select the draw tool!"));
    toolMenu->Append(vlID_MODIFY_MENU, _("Modify\tCtrl+M"), _("Select the modify tool!"));
    toolMenu->AppendSeparator();
    toolMenu->Append(wxID_ZOOM_FIT, "Zoom to All layers\tCtrl+0", "Zoom view to the full extent of all layers");
    toolMenu->Append(wxID_ZOOM_100, "Zoom to visible layers", "Zoom view to the full extent of all visible layers");
    toolMenu->AppendSeparator();
    toolMenu->Append(vlID_MOVE_LAYER, "Move layer...\tCtrl+M", "Move the selected layer");
    toolMenu->AppendSeparator();
    toolMenu->Append(vlID_DISPLAY_VALUE, "Display raster value...\tCtrl+I", "Display pixel values for raster");
    toolMenu->AppendSeparator();
    toolMenu->AppendCheckItem(vlID_PERFORMANCE, "Enable performance logging\tCtrl+T",
                              "Log vroomgis performance into: " + m_PerfLogFile.GetFullPath());
    toolMenu->Check(vlID_PERFORMANCE, false);
    toolMenu->AppendCheckItem(vlID_THREADED, "Enable mono-image rendering (most compatible)");
    toolMenu->Check(vlID_THREADED, true);


    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(toolMenu, "&Tools");
    menuBar->Append(helpMenu, "&Help");

    SetMenuBar(menuBar);


    // STATUS BAR
    CreateStatusBar(2);
    SetStatusText("Welcome to vroomLoader");


    // CONTROLS
    _CreateControls();

    wxLog *myDlgLog = new tmLogGuiSeverity(wxLOG_Warning);
    delete wxLog::SetActiveTarget(myDlgLog);
    m_LogWnd = new wxLogWindow(this, "vroomLoader Log", true, true);


    // Connect Events
    m_DisplayCtrl->Connect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(vroomLoaderFrame::OnRightClick), NULL, this);
    m_DisplayCtrl->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(vroomLoaderFrame::OnKeyDown), NULL, this);
    m_DisplayCtrl->Connect(wxEVT_KEY_UP, wxKeyEventHandler(vroomLoaderFrame::OnKeyUp), NULL, this);

    // DND
    m_TocCtrl->GetControl()->SetDropTarget(new vroomDropFiles(this));

    // VROOMGIS
    m_LayerManager = new vrLayerManager();
    m_ViewerLayerManager = new vrViewerLayerManager(m_LayerManager, this, m_DisplayCtrl, m_TocCtrl);
    m_DisplayCtrl->SetStatusCoordinates(GetStatusBar(), 1, _("Coordinates not available"));
}


vroomLoaderFrame::~vroomLoaderFrame()
{
    // Disconnect Events
    m_DisplayCtrl->Disconnect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(vroomLoaderFrame::OnRightClick), NULL, this);
    m_DisplayCtrl->Disconnect(wxEVT_KEY_DOWN, wxKeyEventHandler(vroomLoaderFrame::OnKeyDown), NULL, this);
    m_DisplayCtrl->Disconnect(wxEVT_KEY_UP, wxKeyEventHandler(vroomLoaderFrame::OnKeyUp), NULL, this);

    // don't delete m_ViewerLayerManager, will be deleted by the manager
    wxDELETE(m_LayerManager);

    delete wxLog::SetActiveTarget(NULL);
    vroomgis_clear_images();
}


void vroomLoaderFrame::OnQuit(wxCommandEvent &event)
{
    event.Skip();
}


void vroomLoaderFrame::OnAbout(wxCommandEvent & WXUNUSED(event))
{
    wxHgVersionDlg myDlg(this, wxID_ANY, "About");
    myDlg.SetBitmapLogo(*_img_vroomgis);
    myDlg.ShowModal();
}


bool vroomLoaderFrame::OpenLayers(const wxArrayString &names)
{
    for (unsigned int i = 0; i < names.GetCount(); i++) {
        // open files
        bool myOpen = m_LayerManager->Open(wxFileName(names.Item(i)));
        wxASSERT(myOpen);
    }

    m_ViewerLayerManager->FreezeBegin();
    for (unsigned int j = 0; j < names.GetCount(); j++) {
        // get files
        vrLayer *myLayer = m_LayerManager->GetLayer(wxFileName(names.Item(j)));
        wxASSERT(myLayer);

        // if type is DIPS
        vrRenderVectorC2PDips *myRender = NULL;
        if (myLayer->GetType() == vrDRIVER_VECTOR_C2P) {
            // change default colour;
            myRender = new vrRenderVectorC2PDips(*wxBLUE);
            myRender->SetUsingDefaultColour(false);
            //adding support for familly 2
            myRender->AddDipColour(*wxRED, 10, true);
        }

        // add files to the viewer
        m_ViewerLayerManager->Add(-1, myLayer, myRender);
    }
    m_ViewerLayerManager->FreezeEnd();
    return true;

}

void vroomLoaderFrame::OnOpenLayer(wxCommandEvent &event)
{
    /*
        wxArrayString myTestFile;
        myTestFile.Add("/home/lucien/programmation/ColtopGIS/test_data/mntmo.c2d");
        OpenLayers(myTestFile);
        return;
    */
    vrDrivers myDrivers;
    wxFileDialog myFileDlg(this, "Select GIS Layers", wxEmptyString, wxEmptyString, myDrivers.GetWildcards(),
                           wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE | wxFD_CHANGE_DIR);

    wxArrayString myPathsFileName;

    if (myFileDlg.ShowModal() == wxID_OK) {
        // try to open files

        myFileDlg.GetPaths(myPathsFileName);
        wxASSERT(myPathsFileName.GetCount() > 0);


        OpenLayers(myPathsFileName);
    }
}


void vroomLoaderFrame::OnCloseLayer(wxCommandEvent &event)
{
    wxArrayString myLayersName;
    for (int i = 0; i < m_ViewerLayerManager->GetCount(); i++) {
        vrRenderer *myRenderer = m_ViewerLayerManager->GetRenderer(i);
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
    for (int j = (signed) myLayerToRemoveIndex.GetCount() - 1; j >= 0; j--) {
        // remove from viewer manager (TOC and Display)
        vrRenderer *myRenderer = m_ViewerLayerManager->GetRenderer(myLayerToRemoveIndex.Item(j));
        vrLayer *myLayer = myRenderer->GetLayer();
        wxASSERT(myRenderer);
        m_ViewerLayerManager->Remove(myRenderer);

        // close layer (not used anymore);
        m_LayerManager->Close(myLayer);
    }

    m_ViewerLayerManager->FreezeEnd();
}


void vroomLoaderFrame::OnShowLog(wxCommandEvent &event)
{
    m_LogWnd->Show(true);
}


void vroomLoaderFrame::OnKeyDown(wxKeyEvent &event)
{
    m_KeyBoardState = wxKeyboardState(event.ControlDown(), event.ShiftDown(), event.AltDown(), event.MetaDown());
    if (m_KeyBoardState.GetModifiers() != wxMOD_CMD) {
        event.Skip();
        return;
    }

    const vrDisplayTool *myTool = m_DisplayCtrl->GetTool();
    if (myTool == NULL) {
        event.Skip();
        return;
    }

    if (myTool->GetID() == wxID_ZOOM_IN) {
        m_DisplayCtrl->SetToolZoomOut();
    }
    event.Skip();
}


void vroomLoaderFrame::OnKeyUp(wxKeyEvent &event)
{
    if (m_KeyBoardState.GetModifiers() != wxMOD_CMD) {
        event.Skip();
        return;
    }

    const vrDisplayTool *myTool = m_DisplayCtrl->GetTool();
    if (myTool == NULL) {
        event.Skip();
        return;
    }

    if (myTool->GetID() == wxID_ZOOM_OUT || myTool->GetID() == wxID_ZOOM_IN) {
        m_DisplayCtrl->SetToolZoom();
    }
    event.Skip();
}


void vroomLoaderFrame::OnToolSelect(wxCommandEvent &event)
{
    m_DisplayCtrl->SetToolDefault();
}


void vroomLoaderFrame::OnToolZoom(wxCommandEvent &event)
{
    m_DisplayCtrl->SetToolZoom();
}


void vroomLoaderFrame::OnToolPan(wxCommandEvent &event)
{
    m_DisplayCtrl->SetToolPan();
}


void vroomLoaderFrame::OnToolZoomToFit(wxCommandEvent &event)
{
    if (event.GetId() == wxID_ZOOM_100) {
        m_ViewerLayerManager->ZoomToFit(true);
    } else {
        m_ViewerLayerManager->ZoomToFit(false);
    }

    m_ViewerLayerManager->Reload();
}


void vroomLoaderFrame::OnMoveLayer(wxCommandEvent &event)
{
    if (m_ViewerLayerManager->GetCount() <= 1) {
        wxLogError("Moving layer not possible with less than 2 layers");
        return;
    }

    int iOldPos = m_TocCtrl->GetSelection();
    if (iOldPos == wxNOT_FOUND) {
        wxLogError("No layer selected, select a layer first");
        return;
    }

    wxMenu myPosMenu;
    myPosMenu.SetTitle("Move layer to following position");
    for (int i = 0; i < m_ViewerLayerManager->GetCount(); i++) {
        myPosMenu.Append(vlID_MENU_POPUP_LAYER + i, wxString::Format("%d - %s", i + 1,
                                                                     m_ViewerLayerManager->GetRenderer(
                                                                             i)->GetLayer()->GetDisplayName().GetFullName()));
    }
    wxPoint myPos = wxGetMousePosition();

    int iNewID = GetPopupMenuSelectionFromUser(myPosMenu, ScreenToClient(myPos));
    if (iNewID == wxID_NONE) {
        return;
    }

    int iNewPos = iNewID - vlID_MENU_POPUP_LAYER;
    if (iNewPos == iOldPos) {
        return;
    }

    m_ViewerLayerManager->Move(iOldPos, iNewPos);
}


void vroomLoaderFrame::OnToolDisplayValue(wxCommandEvent &event)
{
    m_DisplayValueDlg = (vrDisplayValueDlg *) wxWindow::FindWindowById(vlID_DISPLAY_VALUE_DLG);
    if (m_DisplayValueDlg != NULL) {
        m_DisplayValueDlg->Raise();
    } else {
        m_DisplayValueDlg = new vrDisplayValueDlg(this, m_ViewerLayerManager, vlID_DISPLAY_VALUE_DLG);
        m_DisplayValueDlg->Show();
    }

    vrDisplayValueTool *myDisplayTool = new vrDisplayValueTool(m_DisplayCtrl, m_DisplayValueDlg);
    m_DisplayCtrl->SetTool(myDisplayTool);
}


void vroomLoaderFrame::OnLogPerformance(wxCommandEvent &event)
{
    if (event.IsChecked()) {
        m_ViewerLayerManager->StartPerfMonitor(m_PerfLogFile);
    } else {
        m_ViewerLayerManager->StopPerfMonitor();
    }
}


void vroomLoaderFrame::OnEngineThreaded(wxCommandEvent &event)
{
    m_ViewerLayerManager->SetEngineThreaded(!event.IsChecked());
}


void vroomLoaderFrame::OnToolAction(wxCommandEvent &event)
{
    vrDisplayToolMessage *myMsg = (vrDisplayToolMessage *) event.GetClientData();
    wxASSERT(myMsg);

    if (myMsg->m_evtType == vrEVT_TOOL_ZOOM || myMsg->m_evtType == vrEVT_TOOL_ZOOMOUT) {
        // getting rectangle
        vrCoordinate *myCoord = m_ViewerLayerManager->GetDisplay()->GetCoordinate();
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
    } else if (myMsg->m_evtType == vrEVT_TOOL_SELECT) {
        vrCoordinate *myCoord = m_ViewerLayerManager->GetDisplay()->GetCoordinate();
        wxASSERT(myCoord);

        wxPoint myClickedPos = myMsg->m_position;
        if (myClickedPos != wxDefaultPosition) {
            wxPoint2DDouble myRealClickedPos;
            myCoord->ConvertFromPixels(myClickedPos, myRealClickedPos);
            wxMessageBox(wxString::Format("Selected coordinate :\nx:\t%.4f\ny:\t%.4f", myRealClickedPos.m_x,
                                          myRealClickedPos.m_y), "Selected coordinate");
        }

    } else if (myMsg->m_evtType == vrEVT_TOOL_PAN) {
        vrCoordinate *myCoord = m_ViewerLayerManager->GetDisplay()->GetCoordinate();
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
    }


    else {
        wxLogError("Operation not supported now");
    }


    wxDELETE(myMsg);
}


void vroomLoaderFrame::OnStartEditingButton(wxCommandEvent &event)
{
    if (event.IsChecked()) {
        _StartEdition();
    } else {
        _StopEdition();
    }
}


void vroomLoaderFrame::OnUpdateUIEditType(wxUpdateUIEvent &event)
{
    event.Enable(!m_editStartCtrl->GetValue());
}


void vroomLoaderFrame::_StartEdition()
{
    wxLogMessage("Edition Started!");

    // add a layer of selected type into memory
    wxFileName myMemoryLayerName("", m_editTypeCtrl->GetString(m_editTypeCtrl->GetSelection()), "memory");
    wxASSERT(myMemoryLayerName.GetExt() == "memory");

    m_ViewerLayerManager->FreezeBegin();
    vrLayerVectorOGR *myLayer = new vrLayerVectorOGR();
    int mySpatialTypes[] = {wkbPoint, wkbLineString, wkbPolygon};
    if (!myLayer->Create(myMemoryLayerName, mySpatialTypes[m_editTypeCtrl->GetSelection()])) {
        wxFAIL;
    }
    m_LayerManager->Add(myLayer);
    m_ViewerLayerManager->Add(-1, myLayer);
    m_ViewerLayerManager->FreezeEnd();
}


void vroomLoaderFrame::_StopEdition()
{
    wxLogMessage("Edition Stopped!");

    m_ViewerLayerManager->FreezeBegin();
    for (int i = 0; i < m_ViewerLayerManager->GetCount(); i++) {
        if (m_ViewerLayerManager->GetRenderer(i)->GetLayer()->GetType() == vrDRIVER_VECTOR_MEMORY) {
            vrRenderer *myRenderer = m_ViewerLayerManager->GetRenderer(i);
            vrLayer *myLayer = myRenderer->GetLayer();
            // asks if the edited data must be saved
            vrLayerVectorOGR *myMemoryLayer = (vrLayerVectorOGR *) myLayer;
            if (myMemoryLayer->GetFeatureCount() > 0) {
                if (wxMessageBox(_("Save the edited data to a shapefile?"), _("Save"),
                                 wxYES_NO | wxNO_DEFAULT | wxCENTRE, this) == wxYES) {
                    _SaveEdition(myLayer);
                }
            }
            wxASSERT(myRenderer);
            m_ViewerLayerManager->Remove(myRenderer);
            // close layer (not used anymore);
            m_LayerManager->Close(myLayer);
        }
    }
    m_ViewerLayerManager->FreezeEnd();

    // change tool
    m_DisplayCtrl->SetToolDefault();
}


bool vroomLoaderFrame::_SaveEdition(vrLayer *memoryLayer)
{
    vrLayerVectorOGR *myMemoryLayer = (vrLayerVectorOGR *) memoryLayer;
    wxASSERT(myMemoryLayer);

    wxString mySaveFileName = wxFileSelector(_("Choose a filename for the shapefile"), wxEmptyString, wxEmptyString,
                                             _T("shp"), _T("ESRI Shapefiles (*.shp)|*.shp"), wxFD_SAVE, this);
    if (mySaveFileName.IsEmpty()) {
        wxLogMessage(_("Saving canceled!"));
        return false;
    }

    wxFileName mySaveFile(mySaveFileName);
    mySaveFile.SetExt(_T("shp"));

    vrLayerVectorOGR myShapefile;
    if (!myShapefile.Create(mySaveFile, myMemoryLayer->GetGeometryType())) {
        wxLogError(_("Creating '%s' failed!"), mySaveFile.GetFullPath());
        return false;
    }


    bool restart = false;
    myMemoryLayer->ClearSpatialFilter();
    for (long i = 0; i < myMemoryLayer->GetFeatureCount(); i++) {
        restart = false;
        if (i == 0) {
            restart = true;
        }
        OGRFeature *myFeature = myMemoryLayer->GetNextFeature(restart);
        wxASSERT(myFeature);
        myShapefile.AddFeature(myFeature->GetGeometryRef());
        OGRFeature::DestroyFeature(myFeature);
    }
    return true;
}


void vroomLoaderFrame::OnUpdateUIDrawMenu(wxUpdateUIEvent &event)
{
    event.Enable(m_editStartCtrl->GetValue());
}


vrRenderer *vroomLoaderFrame::_GetMemoryRenderer()
{
    vrRenderer *myMemoryRenderer = NULL;
    for (int i = 0; i < m_ViewerLayerManager->GetCount(); i++) {
        if (m_ViewerLayerManager->GetRenderer(i)->GetLayer()->GetType() == vrDRIVER_VECTOR_MEMORY) {
            myMemoryRenderer = m_ViewerLayerManager->GetRenderer(i);
            break;
        }
    }
    wxASSERT(myMemoryRenderer);
    return myMemoryRenderer;
}


void vroomLoaderFrame::OnToolModify(wxCommandEvent &event)
{
    m_DisplayCtrl->SetTool(new vrDisplayToolModify(m_DisplayCtrl));
}


void vroomLoaderFrame::OnToolModifySearch(wxCommandEvent &event)
{
    vrDisplayToolMessage *myMsg = (vrDisplayToolMessage *) event.GetClientData();
    wxASSERT(myMsg);

    vrRenderer *myMemoryRenderer = _GetMemoryRenderer();
    wxASSERT(myMemoryRenderer);
    vrLayerVectorOGR *myMemoryLayer = (vrLayerVectorOGR *) myMemoryRenderer->GetLayer();
    wxASSERT(myMemoryLayer);

    vrRealRect myRealRect;
    m_DisplayCtrl->GetCoordinate()->ConvertFromPixels(myMsg->m_rect, myRealRect);
    wxDELETE(myMsg);
    myMemoryLayer->Select(myRealRect);
    wxArrayLong *mySelectedIDs = myMemoryLayer->GetSelectedIDs();
    wxASSERT(mySelectedIDs);
    if (mySelectedIDs->GetCount() != 0) {
        wxLogMessage(_T("Selected Geometries ID: %ld (number: %ld)"), mySelectedIDs->Item(0),
                     mySelectedIDs->GetCount());
        // copy geometry vertex to tool
        OGRFeature *myFeature = myMemoryLayer->GetFeature(mySelectedIDs->Item(0));
        vrDisplayToolModify *myModifyTool = (vrDisplayToolModify *) m_DisplayCtrl->GetTool();
        wxASSERT(myModifyTool);
        myModifyTool->SetActiveGeometry(myFeature->GetGeometryRef(), myMemoryLayer->GetGeometryType(),
                                        m_DisplayCtrl->GetCoordinate());
        OGRFeature::DestroyFeature(myFeature);
        m_ViewerLayerManager->Reload();
    } else {
        wxLogMessage(_T("No selected geometries!"));
    }
}


void vroomLoaderFrame::OnToolModifyUpdate(wxCommandEvent &event)
{
    vrDisplayToolMessage *myMsg = (vrDisplayToolMessage *) event.GetClientData();
    wxASSERT(myMsg);

    vrRenderer *myMemoryRenderer = _GetMemoryRenderer();
    wxASSERT(myMemoryRenderer);
    vrLayerVectorOGR *myMemoryLayer = (vrLayerVectorOGR *) myMemoryRenderer->GetLayer();
    wxASSERT(myMemoryLayer);

    wxPoint2DDouble myRealPt;
    m_DisplayCtrl->GetCoordinate()->ConvertFromPixels(myMsg->m_position, myRealPt);
    int myVertexIndex = myMsg->m_longData;
    wxDELETE(myMsg);

    wxArrayLong *mySelectedIDs = myMemoryLayer->GetSelectedIDs();
    wxASSERT(mySelectedIDs);
    wxASSERT(mySelectedIDs->GetCount() > 0);

    // update geometry and send new geometry to the modify tool
    OGRFeature *myFeature = myMemoryLayer->GetFeature(mySelectedIDs->Item(0));
    switch (myMemoryLayer->GetGeometryType()) {
        case wkbLineString: // line
        {
            OGRLineString *myLine = (OGRLineString *) myFeature->GetGeometryRef();
            myLine->setPoint(myVertexIndex, myRealPt.m_x, myRealPt.m_y);
        }
            break;


        case wkbPolygon: {
            OGRPolygon *myPolygon = (OGRPolygon *) myFeature->GetGeometryRef();
            OGRLineString *myLine = (OGRLineString *) myPolygon->getExteriorRing();
            myLine->setPoint(myVertexIndex, myRealPt.m_x, myRealPt.m_y);
        }
            break;


        case wkbPoint: {
            OGRPoint *myPt = (OGRPoint *) myFeature->GetGeometryRef();
            myPt->setX(myRealPt.m_x);
            myPt->setY(myRealPt.m_y);
        }
            break;

        default:
            wxLogError(_T("Modification of geometry type: %d isn't supported!"), myMemoryLayer->GetGeometryType());
            break;
    }
    myMemoryLayer->SetFeature(myFeature);
    vrDisplayToolModify *myModifyTool = (vrDisplayToolModify *) m_DisplayCtrl->GetTool();
    myModifyTool->SetActiveGeometry(myFeature->GetGeometryRef(), myMemoryLayer->GetGeometryType(),
                                    m_DisplayCtrl->GetCoordinate());
    OGRFeature::DestroyFeature(myFeature);
    m_ViewerLayerManager->Reload();
}


void vroomLoaderFrame::OnToolDraw(wxCommandEvent &event)
{
    wxLogMessage("setting edit tool");
    switch (m_editTypeCtrl->GetSelection()) {
        case 1:
        case 2:
            m_DisplayCtrl->SetTool(new vrDisplayToolEditLine(m_DisplayCtrl));
            break;

        default:
            m_DisplayCtrl->SetTool(new vrDisplayToolEdit(m_DisplayCtrl));
            break;
    }

    if (m_editor) {
        // Editor exists, assume view has changed
        m_editor->ViewChanged();
        m_editor->DrawShapeEdit(_GetMemoryRenderer()->GetRender());
    }
}


void vroomLoaderFrame::OnToolDrawAction(wxCommandEvent &event)
{
    // create editor if not exists
    if (m_editor == NULL) {
        switch (m_editTypeCtrl->GetSelection()) {
            case 1:
                m_editor = new vrShapeEditorLine(m_DisplayCtrl);
                break;

            case 2:
                m_editor = new vrShapeEditorPolygon(m_DisplayCtrl);
                break;

            default:
                m_editor = new vrShapeEditorPoint(m_DisplayCtrl);
                break;
        }
    }

    vrDisplayToolMessage *myMsg = (vrDisplayToolMessage *) event.GetClientData();
    wxASSERT(myMsg);

    wxPoint2DDouble myRealPt(0, 0);
    m_DisplayCtrl->GetCoordinate()->ConvertFromPixels(myMsg->m_position, myRealPt);
    wxASSERT(m_editor);
    m_editor->AddVertex(myRealPt);

    vrRenderer *myMemoryRenderer = _GetMemoryRenderer();
    wxASSERT(myMemoryRenderer);

    if (myMsg->m_evtType == vrEVT_TOOL_EDIT) {
        wxLogMessage("Clicked: %d, %d", myMsg->m_position.x, myMsg->m_position.y);
        m_editor->DrawShapeEdit(myMemoryRenderer->GetRender());
    } else if (myMsg->m_evtType == vrEVT_TOOL_EDIT_FINISHED) {
        wxLogMessage("Finished: %d, %d", myMsg->m_position.x, myMsg->m_position.y);
        vrLayerVectorOGR *myMemoryLayer = (vrLayerVectorOGR *) myMemoryRenderer->GetLayer();
        long myAddedId = myMemoryLayer->AddFeature(m_editor->GetGeometryRef());
        myMemoryLayer->SetSelectedID(myAddedId);
        wxDELETE(m_editor);
        m_ViewerLayerManager->Reload();
    }
    wxDELETE(myMsg);
}

