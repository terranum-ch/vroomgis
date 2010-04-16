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




BEGIN_EVENT_TABLE(vroomLoaderFrame, wxFrame)
    EVT_MENU(wxID_EXIT,  vroomLoaderFrame::OnQuit)
    EVT_MENU(wxID_ABOUT, vroomLoaderFrame::OnAbout)
	EVT_MENU(wxID_OPEN, vroomLoaderFrame::OnOpenLayer)
	EVT_MENU(wxID_REMOVE, vroomLoaderFrame::OnCloseLayer)
	EVT_MENU (wxID_INFO, vroomLoaderFrame::OnShowLog)
	EVT_MENU (wxID_DEFAULT, vroomLoaderFrame::OnToolSelect)
	EVT_MENU (wxID_ZOOM_IN, vroomLoaderFrame::OnToolZoom)
END_EVENT_TABLE()
IMPLEMENT_APP(vroomLoader)



bool vroomLoader::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;
	
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
	
	wxFilePickerCtrl * myPicker = new wxFilePickerCtrl(m_panel1, wxID_ANY,wxEmptyString, 
													   wxFileSelectorPromptStr,
													   wxFileSelectorDefaultWildcardStr,
													   wxDefaultPosition,
													   wxDefaultSize,
													   wxFLP_SAVE | wxFLP_USE_TEXTCTRL);
													   
	bSizer4->Add(myPicker, 0, wxEXPAND | wxALL, 5);
	
	
	m_panel1->SetSizer( bSizer4 );
	m_panel1->Layout();
	bSizer4->Fit( m_panel1 );
	wxPanel* m_panel2;
	m_panel2 = new wxPanel( m_splitter2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	m_DisplayCtrl = new vrViewerDisplay( m_panel2, wxID_ANY);
	m_DisplayCtrl->SetBackgroundColour( wxColour( 255, 255, 255 ) );
	
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
    SetIcon(wxICON(sample));

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
}




void vroomLoaderFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	Close(true);
}



void vroomLoaderFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(wxString::Format
                 (
				  "This is vroomLoader, a minimal vroomGIS sample\n"
				  "running under %s.\n"
				  "wxWidgets version is : %s",
				  wxGetOsDescription(),
				  wxVERSION_STRING
                 ),
                 "About vroomLoader",
                 wxOK | wxICON_INFORMATION,
                 this);
}




void vroomLoaderFrame::OnOpenLayer(wxCommandEvent & event)
{
	/*
	// TODO: This is temp code for autoloading file
	wxFileName myTestFile("/Users/lucien/Documents/PRJ/COLTOPGIS/test_data/gwn_combioula.shp");
	m_LayerManager->Open(myTestFile);
	vrLayer * myTestLayer = m_LayerManager->GetLayer(myTestFile);
	wxASSERT(myTestLayer);
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
		
		
		for (unsigned int i = 0; i< myPathsFileName.GetCount(); i++) {
			// open files
			bool myOpen = m_LayerManager->Open(wxFileName(myPathsFileName.Item(i)));
			wxASSERT(myOpen);
		}
		
		m_ViewerLayerManager->FreezeBegin();
		for (unsigned int j = 0; j< myPathsFileName.GetCount(); j++) {
			// get files
			vrLayer * myLayer = m_LayerManager->GetLayer( wxFileName(myPathsFileName.Item(j)));
			wxASSERT(myLayer);
			
			// add files to the viewer
			m_ViewerLayerManager->Add(-1, myLayer);
		}
		m_ViewerLayerManager->FreezeEnd();
		
		
	}
	//event.Skip();
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


