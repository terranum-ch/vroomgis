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



BEGIN_EVENT_TABLE(vroomLoaderFrame, wxFrame)
    EVT_MENU(wxID_EXIT,  vroomLoaderFrame::OnQuit)
    EVT_MENU(wxID_ABOUT, vroomLoaderFrame::OnAbout)
	EVT_MENU(wxID_OPEN, vroomLoaderFrame::OnOpenLayer)
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
	//m_splitter2->Connect( wxEVT_IDLE, wxIdleEventHandler( MyPanel1::m_splitter2OnIdle ), NULL, this );
	wxPanel* m_panel1;
	m_panel1 = new wxPanel( m_splitter2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	m_TocCtrl = new vrViewerTOC( m_panel1, wxID_ANY);
	bSizer4->Add( m_TocCtrl, 2, wxEXPAND, 5 );
	
	m_LogCtrl = new wxTextCtrl( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
	bSizer4->Add( m_LogCtrl, 1, wxEXPAND, 5 );

	
	m_panel1->SetSizer( bSizer4 );
	m_panel1->Layout();
	bSizer4->Fit( m_panel1 );
	wxPanel* m_panel2;
	m_panel2 = new wxPanel( m_splitter2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	m_DisplayCtrl = new wxScrolledWindow( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_DisplayCtrl->SetScrollRate( 5, 5 );
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
	wxMenu *layermenu = new wxMenu;
    
	helpMenu->Append(wxID_ABOUT, "&About...\tF1", "Show about dialog");
    fileMenu->Append(wxID_OPEN, "&Open\tCtrl+O","Open a GIS layer");
	fileMenu->Append(wxID_EXIT, "E&xit\tAlt-X", "Quit this program");
	
     wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
	menuBar->Append(layermenu, "&Layers");
	menuBar->Append(helpMenu, "&Help");
	
    SetMenuBar(menuBar);
 
	
	// STATUS BAR
	CreateStatusBar(2);
    SetStatusText("Welcome to vroomLoader");
	
	
	// CONTROLS
	_CreateControls();
	wxLog::SetActiveTarget(new wxLogTextCtrl (m_LogCtrl));
	
	// Connect Events
	m_DisplayCtrl->Connect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( vroomLoaderFrame::OnRightClick ), NULL, this );


	// VROOMGIS
	m_ViewerLayerManager = new vrViewerLayerManager(&m_LayerManager, NULL, m_TocCtrl);
	
}



vroomLoaderFrame::~vroomLoaderFrame()
{
	// Disconnect Events
	m_DisplayCtrl->Disconnect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( vroomLoaderFrame::OnRightClick ), NULL, this );


	
}




void vroomLoaderFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	wxLog::SetActiveTarget (NULL);
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
			bool myOpen = m_LayerManager.Open(wxFileName(myPathsFileName.Item(i)));
			wxASSERT(myOpen);
		}
		
		for (unsigned int j = 0; j< myPathsFileName.GetCount(); j++) {
			// get files
			vrLayer * myLayer = m_LayerManager.GetLayer( wxFileName(myPathsFileName.Item(j)));
			wxASSERT(myLayer);
			
			// add files to the viewer
			m_ViewerLayerManager->Add(-1, myLayer);
		}
		
		
	}
	
	
	
	event.Skip();
}


