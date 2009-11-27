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
	EVT_MENU(wxID_OPEN, vroomLoaderFrame::OnAddLayer)
END_EVENT_TABLE()
IMPLEMENT_APP(vroomLoader)



bool vroomLoader::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    vroomLoaderFrame *frame = new vroomLoaderFrame("vroomLoader");
    frame->Show(true);

    return true;
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
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_Scrolled = new wxScrolledWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_Scrolled->SetScrollRate( 5, 5 );
	m_Scrolled->SetBackgroundColour( wxColour( 255, 255, 255 ) );
	bSizer1->Add( m_Scrolled, 1, wxEXPAND | wxALL, 0 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	
	// Connect Events
	m_Scrolled->Connect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( vroomLoaderFrame::OnRightClick ), NULL, this );
}



vroomLoaderFrame::~vroomLoaderFrame()
{
	// Disconnect Events
	m_Scrolled->Disconnect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( vroomLoaderFrame::OnRightClick ), NULL, this );
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




void vroomLoaderFrame::OnAddLayer(wxCommandEvent & event)
{
	vrDrivers myDrivers;
	wxLogMessage("Adding gis layer : %s", myDrivers.GetWildcards());
	event.Skip();
}


