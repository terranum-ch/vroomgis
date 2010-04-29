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




BEGIN_EVENT_TABLE(vroomTwinFrame, wxFrame)
    EVT_MENU(wxID_EXIT,  vroomTwinFrame::OnQuit)
    EVT_MENU(wxID_ABOUT, vroomTwinFrame::OnAbout)
	EVT_MENU(wxID_OPEN, vroomTwinFrame::OnOpenLayer)
	EVT_MENU(wxID_REMOVE, vroomTwinFrame::OnCloseLayer)
	EVT_MENU (wxID_INFO, vroomTwinFrame::OnShowLog)
	/*EVT_MENU (wxID_DEFAULT, vroomTwinFrame::OnToolSelect)
	EVT_MENU (wxID_ZOOM_IN, vroomTwinFrame::OnToolZoom)
	EVT_MENU (wxID_ZOOM_FIT, vroomTwinFrame::OnToolZoomToFit)
	EVT_MENU (wxID_ZOOM_100, vroomTwinFrame::OnToolZoomToFit)
	EVT_MENU (wxID_MOVE_FRAME, vroomTwinFrame::OnToolPan)
	
	EVT_COMMAND(wxID_ANY, vrEVT_TOOL_ZOOM, vroomTwinFrame::OnToolAction)
	EVT_COMMAND(wxID_ANY, vrEVT_TOOL_SELECT, vroomTwinFrame::OnToolAction)
	EVT_COMMAND(wxID_ANY, vrEVT_TOOL_PAN, vroomTwinFrame::OnToolAction)*/
END_EVENT_TABLE()
IMPLEMENT_APP(vroomTwin)



bool vroomTwin::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;
	
    vroomTwinFrame *frame = new vroomTwinFrame("vroomTwin");
	frame->SetSize(50, 50, 800, 500);
	frame->Show(true);
	return true;
}

void  vroomTwinFrame::_CreateControls()
{
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxSplitterWindow* m_splitter2;
	m_splitter2 = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_BORDER|wxSP_LIVE_UPDATE );

	wxPanel* m_panel1;
	m_panel1 = new wxPanel( m_splitter2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticText * m_staticText2 = new wxStaticText( m_panel1, wxID_ANY, "Display 1", wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	m_staticText2->SetFont( *wxSMALL_FONT );
	bSizer4->Add( m_staticText2, 0, wxALL, 5 );
	
	m_TocCtrl1 = new vrViewerTOC( m_panel1, wxID_ANY);
	bSizer4->Add( m_TocCtrl1, 1, wxEXPAND, 5 );
	
	wxStaticText * m_staticText1 = new wxStaticText( m_panel1, wxID_ANY, "Display 2", wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	m_staticText1->SetFont( *wxSMALL_FONT );
	
	bSizer4->Add( m_staticText1, 0, wxALL, 5 );
	
	m_TocCtrl2 = new vrViewerTOC( m_panel1, wxID_ANY);
	bSizer4->Add( m_TocCtrl2, 1, wxEXPAND, 5 );
	
	m_panel1->SetSizer( bSizer4 );
	m_panel1->Layout();
	bSizer4->Fit( m_panel1 );
	wxPanel* m_panel2;
	m_panel2 = new wxPanel( m_splitter2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	m_DisplayCtrl1 = new vrViewerDisplay( m_panel2, wxNewId(), *wxBLACK);
	bSizer5->Add( m_DisplayCtrl1, 1, wxEXPAND|wxRIGHT, 1 );
	
	m_DisplayCtrl2 = new vrViewerDisplay( m_panel2, wxNewId(), *wxWHITE);
	bSizer5->Add( m_DisplayCtrl2, 1, wxEXPAND|wxLEFT, 1 );
	
	m_panel2->SetSizer( bSizer5 );
	m_panel2->Layout();
	bSizer5->Fit( m_panel2 );
	m_splitter2->SplitVertically( m_panel1, m_panel2, 200 );
	bSizer1->Add( m_splitter2, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer1 );	
}


vroomTwinFrame::vroomTwinFrame(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title)
{
    SetIcon(wxICON(sample));

	// MENU
    wxMenu *fileMenu = new wxMenu;
	wxMenu *helpMenu = new wxMenu;
	//wxMenu *toolMenu = new wxMenu;
    
	helpMenu->Append(wxID_ABOUT, "&About...\tF1", "Show about dialog");
	helpMenu->Append(wxID_INFO, "Show Log Window", "Show log window");
    fileMenu->Append(wxID_OPEN, "&Open\tCtrl+O","Open GIS layer(s)");
	fileMenu->Append(wxID_REMOVE, "&Close\tCtrl+W", "Close GIS layer(s)");
	fileMenu->Append(wxID_EXIT, "E&xit\tAlt-X", "Quit this program");

	/*toolMenu->Append(wxID_DEFAULT, "Select\tCtrl+S", "Select the selection tool");
	toolMenu->Append(wxID_ZOOM_IN, "Zoom\tCtrl+Z", "Select the zoom tool");
	toolMenu->Append(wxID_MOVE_FRAME, "Pan\tCtrl+P", "Select the pan tool");
	toolMenu->AppendSeparator();
	toolMenu->Append(wxID_ZOOM_FIT, "Zoom to All layers", "Zoom view to the full extent of all layers");
	toolMenu->Append(wxID_ZOOM_100, "Zoom to visible layers", "Zoom view to the full extent of all visible layers");*/
	
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
	//menuBar->Append(toolMenu, "&Tools");
	menuBar->Append(helpMenu, "&Help");
	
    SetMenuBar(menuBar);
 
	
	// STATUS BAR
	CreateStatusBar(2);
    SetStatusText("Welcome to vroomTwin");
	
	
	// CONTROLS
	_CreateControls();
	
	wxLog * myDlgLog = new tmLogGuiSeverity(wxLOG_Warning);
	delete wxLog::SetActiveTarget(myDlgLog);
	m_LogWnd = new wxLogWindow(this, "vroomTwin Log", true, true);
	
	
	// VROOMGIS
	m_LayerManager = new vrLayerManager();
	m_ViewerLayerManager1 = new vrViewerLayerManager(m_LayerManager, this, m_DisplayCtrl1 , m_TocCtrl1);
	m_ViewerLayerManager2 = new vrViewerLayerManager(m_LayerManager, this, m_DisplayCtrl2 , m_TocCtrl2);

	
}



vroomTwinFrame::~vroomTwinFrame()
{	
	
	// don't delete m_ViewerLayerManager, will be deleted by the manager
	wxDELETE(m_LayerManager);
	
	delete wxLog::SetActiveTarget (NULL);
}




void vroomTwinFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	Close(true);
}



void vroomTwinFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(wxString::Format
                 (
				  "This is vroomTwin, a minimal Twin view vroomGIS sample\n"
				  "running under %s.\n"
				  "wxWidgets version is : %s",
				  wxGetOsDescription(),
				  wxVERSION_STRING
                 ),
                 "About vroomTwin",
                 wxOK | wxICON_INFORMATION,
                 this);
}




void vroomTwinFrame::OnOpenLayer(wxCommandEvent & event)
{
	
	/*
	// TODO: This is temp code for autoloading file
	wxFileName myTestFile("/Users/lucien/DATA/PRJ/COLTOP-GIS/test_data/gwn_combioula.shp");
	m_LayerManager->Open(myTestFile);
	vrLayer * myTestLayer = m_LayerManager->GetLayer(myTestFile);
	wxASSERT(myTestLayer);
	m_ViewerLayerManager->Add(-1, myTestLayer);
	return;
	 */
	
	/*
	
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
	//event.Skip();*/
}



void vroomTwinFrame::OnCloseLayer(wxCommandEvent & event){
	
/*	wxArrayString myLayersName;
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
				*/					 
									 
	
	
}



void vroomTwinFrame::OnShowLog (wxCommandEvent & event)
{
	m_LogWnd->Show(true);	
}



/*
void vroomTwinFrame::OnToolSelect (wxCommandEvent & event){
	m_DisplayCtrl->SetToolDefault();
}



void vroomTwinFrame::OnToolZoom (wxCommandEvent & event){
	m_DisplayCtrl->SetToolZoom();
}



void vroomTwinFrame::OnToolPan (wxCommandEvent & event){
	m_DisplayCtrl->SetToolPan();
}



void vroomTwinFrame::OnToolZoomToFit (wxCommandEvent & event)
{
	if (event.GetId() == wxID_ZOOM_100) {
		m_ViewerLayerManager->ZoomToFit(true);
	}
	else {
		m_ViewerLayerManager->ZoomToFit(false);
	}
	
	m_ViewerLayerManager->Reload();
}



void vroomTwinFrame::OnToolAction (wxCommandEvent & event){
	vrDisplayToolMessage * myMsg = (vrDisplayToolMessage*)event.GetClientData();
	wxASSERT(myMsg);
	
	if(myMsg->m_EvtType == vrEVT_TOOL_ZOOM){
		// getting rectangle
		vrCoordinate * myCoord = m_ViewerLayerManager->GetDispaly()->GetCoordinate();
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
	else if (myMsg->m_EvtType == vrEVT_TOOL_PAN) {
		vrCoordinate * myCoord = m_ViewerLayerManager->GetDispaly()->GetCoordinate();
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
}*/


