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

#include "frame.h"
#include <wx/filepicker.h>

#include "tmlog.h"	// for double logging process
#include "vrdisplayvalue.h"	// for displaying values
#include <lsversion_dlg.h>

#include "../../../vroomgis/art/vroomgis_bmp.cpp"




BEGIN_EVENT_TABLE(vroomLoaderFrame, wxFrame)
    EVT_MENU(wxID_EXIT,  vroomLoaderFrame::OnQuit)
    EVT_MENU(wxID_ABOUT, vroomLoaderFrame::OnAbout)
	EVT_MENU(wxID_OPEN, vroomLoaderFrame::OnOpenLayer)
	EVT_MENU(wxID_REMOVE, vroomLoaderFrame::OnCloseLayer)
	EVT_MENU (wxID_INFO, vroomLoaderFrame::OnShowLog)
	EVT_MENU (wxID_DEFAULT, vroomLoaderFrame::OnToolSelect)
	EVT_MENU (wxID_ZOOM_IN, vroomLoaderFrame::OnToolZoom)
	EVT_MENU (wxID_ZOOM_FIT, vroomLoaderFrame::OnToolZoomToFit)
	EVT_MENU (wxID_ZOOM_100, vroomLoaderFrame::OnToolZoomToFit)
	EVT_MENU (wxID_MOVE_FRAME, vroomLoaderFrame::OnToolPan)
	EVT_MENU (vlID_MOVE_LAYER, vroomLoaderFrame::OnMoveLayer)
	EVT_MENU (vlID_DISPLAY_VALUE, vroomLoaderFrame::OnToolDisplayValue)
	
	EVT_COMMAND(wxID_ANY, vrEVT_TOOL_ZOOM, vroomLoaderFrame::OnToolAction)
	EVT_COMMAND(wxID_ANY, vrEVT_TOOL_SELECT, vroomLoaderFrame::OnToolAction)
	EVT_COMMAND(wxID_ANY, vrEVT_TOOL_PAN, vroomLoaderFrame::OnToolAction)
END_EVENT_TABLE()
IMPLEMENT_APP(vroomLoader)



vroomDropFiles::vroomDropFiles(vroomLoaderFrame * parent){
	wxASSERT(parent);
	m_LoaderFrame = parent;
}


bool vroomDropFiles::OnDropFiles(wxCoord x, wxCoord y, 
								 const wxArrayString & filenames){
	if (filenames.GetCount() == 0) {
		return false;
	}
	
	m_LoaderFrame->OpenLayers(filenames);
	return true;
}



bool vroomLoader::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;
	
	wxInitAllImageHandlers();
	initialize_images();
	
    vroomLoaderFrame *frame = new vroomLoaderFrame("vroomLoader");
    //frame->CenterOnScreen(wxBOTH);
	frame->SetSize(50, 50, 800, 500);
	frame->Show(true);
	
    return true;
}

void  vroomLoaderFrame::_CreateControls()
{
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxSplitterWindow* m_splitter2;
	m_splitter2 = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_BORDER|wxSP_LIVE_UPDATE );
	wxPanel* m_panel1;
	m_panel1 = new wxPanel( m_splitter2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	m_TocCtrl = new vrViewerTOC( m_panel1, wxID_ANY);
	bSizer4->Add( m_TocCtrl, 2, wxEXPAND, 5 );
	
	//wxFilePickerCtrl * myPicker = new wxFilePickerCtrl(m_panel1, wxID_ANY,wxEmptyString, 
	//												   wxFileSelectorPromptStr,
	//												   wxFileSelectorDefaultWildcardStr,
	//												   wxDefaultPosition,
	//												   wxDefaultSize,
	//												   wxFLP_SAVE | wxFLP_USE_TEXTCTRL);
													   
	//bSizer4->Add(myPicker, 0, wxEXPAND | wxALL, 5);
	
	
	m_panel1->SetSizer( bSizer4 );
	m_panel1->Layout();
	bSizer4->Fit( m_panel1 );
	wxPanel* m_panel2;
	m_panel2 = new wxPanel( m_splitter2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	m_DisplayCtrl = new vrViewerDisplay( m_panel2, wxID_ANY, wxColour(120,120,120));
	
	bSizer5->Add( m_DisplayCtrl, 1, wxEXPAND, 5 );
	
	m_panel2->SetSizer( bSizer5 );
	m_panel2->Layout();
	bSizer5->Fit( m_panel2 );
	m_splitter2->SplitVertically( m_panel1, m_panel2, 300 );
	bSizer1->Add( m_splitter2, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer1 );
	//this->Layout();
}


vroomLoaderFrame::vroomLoaderFrame(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title)
{
    SetIcon(wxICON(vroomgis));
	m_DisplayValueDlg = NULL;

	// MENU
    wxMenu *fileMenu = new wxMenu;
	wxMenu *helpMenu = new wxMenu;
	wxMenu *toolMenu = new wxMenu;
    
	helpMenu->Append(wxID_ABOUT, "&About...\tF1", "Show about dialog");
	helpMenu->Append(wxID_INFO, "Show Log Window", "Show log window");
    fileMenu->Append(wxID_OPEN, "&Open\tCtrl+O","Open GIS layer(s)");
	fileMenu->Append(wxID_REMOVE, "&Close\tCtrl+W", "Close GIS layer(s)");
	fileMenu->Append(wxID_EXIT, "E&xit\tAlt-X", "Quit this program");

	toolMenu->Append(wxID_DEFAULT, "Select\tCtrl+S", "Select the selection tool");
	toolMenu->Append(wxID_ZOOM_IN, "Zoom\tCtrl+Z", "Select the zoom tool");
	toolMenu->Append(wxID_MOVE_FRAME, "Pan\tCtrl+P", "Select the pan tool");
	toolMenu->AppendSeparator();
	toolMenu->Append(wxID_ZOOM_FIT, "Zoom to All layers", "Zoom view to the full extent of all layers");
	toolMenu->Append(wxID_ZOOM_100, "Zoom to visible layers", "Zoom view to the full extent of all visible layers");
	toolMenu->AppendSeparator();
	toolMenu->Append(vlID_MOVE_LAYER, "Move layer...\tCtrl+M", "Move the selected layer");
	toolMenu->AppendSeparator();
	toolMenu->Append(vlID_DISPLAY_VALUE, "Display raster value...\tCtrl+I", "Display pixel values for raster");
	
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
	
	wxLog * myDlgLog = new tmLogGuiSeverity(wxLOG_Warning);
	delete wxLog::SetActiveTarget(myDlgLog);
	m_LogWnd = new wxLogWindow(this, "vroomLoader Log", true, true);
	
	
	// Connect Events
	m_DisplayCtrl->Connect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( vroomLoaderFrame::OnRightClick ), NULL, this );

	// DND
	m_TocCtrl->SetDropTarget(new vroomDropFiles(this));

	// VROOMGIS
	m_LayerManager = new vrLayerManager();
	m_ViewerLayerManager = new vrViewerLayerManager(m_LayerManager, this, m_DisplayCtrl , m_TocCtrl);
	
}



vroomLoaderFrame::~vroomLoaderFrame()
{	
	// Disconnect Events
	m_DisplayCtrl->Disconnect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( vroomLoaderFrame::OnRightClick ), NULL, this );

	// don't delete m_ViewerLayerManager, will be deleted by the manager
	wxDELETE(m_LayerManager);
	
	delete wxLog::SetActiveTarget (NULL);
	clear_images();
}




void vroomLoaderFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	Close(true);
}



void vroomLoaderFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	lsVersionDlg * myDlg = new lsVersionDlg(this, wxID_ANY, wxEmptyString);
	myDlg->SetBitmapLogo(*_img_vroomgis);
	myDlg->ShowModal();
	wxDELETE(myDlg);
}


bool vroomLoaderFrame::OpenLayers (const wxArrayString & names){
	for (unsigned int i = 0; i< names.GetCount(); i++) {
		// open files
		bool myOpen = m_LayerManager->Open(wxFileName(names.Item(i)));
		wxASSERT(myOpen);
	}
	
	m_ViewerLayerManager->FreezeBegin();
	for (unsigned int j = 0; j< names.GetCount(); j++) {
		// get files
		vrLayer * myLayer = m_LayerManager->GetLayer( wxFileName(names.Item(j)));
		wxASSERT(myLayer);
		
		// add files to the viewer
		m_ViewerLayerManager->Add(-1, myLayer);
	}
	m_ViewerLayerManager->FreezeEnd();
	return true;
	
}


void vroomLoaderFrame::OnOpenLayer(wxCommandEvent & event)
{
	
	
	// TODO: This is temp code for autoloading file
	/*
	wxFileName myTestFile("/Users/lucien/DATA/PRJ/COLTOP-GIS/test_data/gwn_combioula.shp");
	wxFileName myTestFile1("/Users/lucien/DATA/PRJ/COLTOP-GIS/test_data/MNT.tif");
	wxFileName myTestFile2("/Users/lucien/DATA/PRJ/COLTOP-GIS/test_data/ortophoto_clp.tif");

	m_LayerManager->Open(myTestFile);
	m_LayerManager->Open(myTestFile1);
	m_LayerManager->Open(myTestFile2);
	
	vrLayer * myTestLayer = m_LayerManager->GetLayer(myTestFile);
	vrLayer * myTestLayer1 = m_LayerManager->GetLayer(myTestFile1);
	vrLayer * myTestLayer2 = m_LayerManager->GetLayer(myTestFile2);
	
	wxASSERT(myTestLayer);
	wxASSERT(myTestLayer1);
	wxASSERT(myTestLayer2);

	m_ViewerLayerManager->Add(-1, myTestLayer1);
	m_ViewerLayerManager->Add(-1, myTestLayer2);
	m_ViewerLayerManager->Add(-1, myTestLayer);
	return;
	*/
	 
	
	
	
	vrDrivers myDrivers;
	wxFileDialog myFileDlg (this, "Select GIS Layers",
							wxEmptyString,
							wxEmptyString,
							myDrivers.GetWildcards(),
							 wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE | wxFD_CHANGE_DIR);
	
	wxArrayString myPathsFileName;
	
	if(myFileDlg.ShowModal()==wxID_OK){
		// try to open files

		myFileDlg.GetPaths(myPathsFileName);
		wxASSERT(myPathsFileName.GetCount() > 0);
		
		OpenLayers(myPathsFileName);
	}
}



void vroomLoaderFrame::OnCloseLayer(wxCommandEvent & event){
	
	wxArrayString myLayersName;
	for (int i = 0; i<m_ViewerLayerManager->GetCount(); i++) {
		vrRenderer * myRenderer = m_ViewerLayerManager->GetRenderer(i);
		wxASSERT(myRenderer);
		myLayersName.Add(myRenderer->GetLayer()->GetName().GetFullName());
	}
	
	if (myLayersName.IsEmpty()) {
		wxLogError("No layer opened, nothing to close");
		return;
	}
	

	wxMultiChoiceDialog myChoiceDlg (this, "Select Layer(s) to close",
									 "Close layer(s)",
									 myLayersName);
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
	for (int j = (signed) myLayerToRemoveIndex.GetCount() -1; j >= 0 ; j--) {
		
		// remove from viewer manager (TOC and Display)
		vrRenderer * myRenderer = m_ViewerLayerManager->GetRenderer(myLayerToRemoveIndex.Item(j));
		vrLayer * myLayer = myRenderer->GetLayer();
		wxASSERT(myRenderer);
		m_ViewerLayerManager->Remove(myRenderer);
		
		// close layer (not used anymore);
		m_LayerManager->Close(myLayer);
	}
	
	m_ViewerLayerManager->FreezeEnd();
									 
									 
	
	
}



void vroomLoaderFrame::OnShowLog (wxCommandEvent & event)
{
	m_LogWnd->Show(true);	
}




void vroomLoaderFrame::OnToolSelect (wxCommandEvent & event){
	m_DisplayCtrl->SetToolDefault();
}



void vroomLoaderFrame::OnToolZoom (wxCommandEvent & event){
	m_DisplayCtrl->SetToolZoom();
}



void vroomLoaderFrame::OnToolPan (wxCommandEvent & event){
	m_DisplayCtrl->SetToolPan();
}



void vroomLoaderFrame::OnToolZoomToFit (wxCommandEvent & event)
{
	if (event.GetId() == wxID_ZOOM_100) {
		m_ViewerLayerManager->ZoomToFit(true);
	}
	else {
		m_ViewerLayerManager->ZoomToFit(false);
	}
	
	m_ViewerLayerManager->Reload();
}


void vroomLoaderFrame::OnMoveLayer (wxCommandEvent & event){
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
	for (int i = 0; i<m_ViewerLayerManager->GetCount(); i++) {
		myPosMenu.Append(vlID_MENU_POPUP_LAYER + i, 
						 wxString::Format("%d - %s",i+1,
										  m_ViewerLayerManager->GetRenderer(i)->GetLayer()->GetName().GetFullName()));
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



void vroomLoaderFrame::OnToolDisplayValue (wxCommandEvent & event){
	
	m_DisplayValueDlg = (vrDisplayValueDlg*) wxWindow::FindWindowById(vlID_DISPLAY_VALUE_DLG);
	if(m_DisplayValueDlg != NULL) {
		m_DisplayValueDlg->Raise();
	}
	else{
		m_DisplayValueDlg = new vrDisplayValueDlg(this, m_ViewerLayerManager, vlID_DISPLAY_VALUE_DLG);
		m_DisplayValueDlg->Show();
	}
	
	vrDisplayValueTool * myDisplayTool = new vrDisplayValueTool(m_DisplayCtrl,
																m_DisplayValueDlg);
	m_DisplayCtrl->SetTool(myDisplayTool);
}



void vroomLoaderFrame::OnToolAction (wxCommandEvent & event){
	vrDisplayToolMessage * myMsg = (vrDisplayToolMessage*)event.GetClientData();
	wxASSERT(myMsg);
	
	if(myMsg->m_EvtType == vrEVT_TOOL_ZOOM){
		// getting rectangle
		vrCoordinate * myCoord = m_ViewerLayerManager->GetDisplay()->GetCoordinate();
		wxASSERT(myCoord);
		
		// get real rectangle
		vrRealRect myRealRect;
		bool bSuccess = myCoord->ConvertFromPixels(myMsg->m_Rect, myRealRect);
		wxASSERT(bSuccess == true);
		
		// get fitted rectangle
		vrRealRect myFittedRect =myCoord->GetRectFitted(myRealRect);
		wxASSERT(myFittedRect.IsOk());
				
		// moving view
		m_ViewerLayerManager->Zoom(myFittedRect);
		m_ViewerLayerManager->Reload();
		
		
	}else if (myMsg->m_EvtType == vrEVT_TOOL_SELECT) {
		vrCoordinate * myCoord = m_ViewerLayerManager->GetDisplay()->GetCoordinate();
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
	else if (myMsg->m_EvtType == vrEVT_TOOL_PAN) {
		vrCoordinate * myCoord = m_ViewerLayerManager->GetDisplay()->GetCoordinate();
		wxASSERT(myCoord);
		
		wxPoint myMovedPos = myMsg->m_Position;
		wxPoint2DDouble myMovedRealPt;
		if (myCoord->ConvertFromPixels(myMovedPos, myMovedRealPt)==false){
			wxLogError("Error converting point : %d, %d to real coordinate",
					   myMovedPos.x, myMovedPos.y);
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


