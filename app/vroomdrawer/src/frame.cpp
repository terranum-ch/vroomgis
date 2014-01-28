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
#include <wx/filepicker.h>

#include "tmlog.h"	// for double logging process
#include <wxhgversion_dlg.h>
#include "vrrendervectorc2p.h"
#include "vrlayervector.h"
#include "vrlayervectorstar.h"
#include "../../../vroomgis/art/vroomgis_bmp.cpp"
#include <lscrashreport.h>


IMPLEMENT_APP(vroomDrawer);
vroomDrawer::vroomDrawer(){
    wxHandleFatalExceptions();
}


bool vroomDrawer::OnInit()
{
#ifdef __WXDEBUG__
    // debugging string for OSX
    // this is needed for viewing string content with Xcode !!
    wxString myTest = _T("Test debugging");
    myTest.ToUTF8().data();
#endif
    
    if ( !wxApp::OnInit() ){
        return false;
    }

	wxInitAllImageHandlers();
	vroomgis_initialize_images();

    vroomDrawerFrame *frame = new vroomDrawerFrame("vroomDrawer");
	frame->SetSize(50, 50, 800, 500);
	frame->Show(true);
    return true;
}


void vroomDrawer::OnFatalException(){
    lsCrashReport myCrashReport (_T("vroomDrawer"));
    if(myCrashReport.PrepareReport(wxDebugReport::Context_Exception)==false){
        return;
    }
    if (myCrashReport.SendReportWeb(_T("http://www.crealp.ch/crashreport/upload_file.php"))==false){
        wxString myDocPath = wxStandardPaths::Get().GetDocumentsDir();
        if(myCrashReport.SaveReportFile(myDocPath)==false){
            wxLogError(_("Unable to save the crashreport!"));
            return;
        }
        wxLogWarning(_("Connection problem! crashreport wasn't sent. crashreport was saved into '%s'\nplease send it manually to lucien.schreiber@crealp.vs.ch"), myDocPath);
    }
}


BEGIN_EVENT_TABLE(vroomDrawerFrame, wxFrame)
    EVT_MENU(wxID_EXIT,  vroomDrawerFrame::OnQuit)
    EVT_MENU(wxID_ABOUT, vroomDrawerFrame::OnAbout)

	EVT_MENU( wxID_ADD, vroomDrawerFrame::OnLayerAdd )
	EVT_MENU( wxID_REMOVE, vroomDrawerFrame::OnLayerRemove )
	EVT_MENU( wxID_ZOOM_IN, vroomDrawerFrame::OnToolZoom )
	EVT_MENU( ID_MENU_SELECT, vroomDrawerFrame::OnToolSelect )
	EVT_MENU( ID_MENU_PAN, vroomDrawerFrame::OnToolPan )
	EVT_MENU( wxID_ZOOM_FIT, vroomDrawerFrame::OnZoomToFit )
	EVT_MENU (wxID_INFO, vroomDrawerFrame::OnShowLog)
    EVT_MENU(wxID_DELETE, vroomDrawerFrame::OnTestCrashSoftware)
	EVT_BUTTON(ID_MENU_ADDMEMORYLAYER, vroomDrawerFrame::OnStarLayerAdd)

	EVT_COMMAND(wxID_ANY, vrEVT_TOOL_ZOOM, vroomDrawerFrame::OnToolActionZoom)
	EVT_COMMAND(wxID_ANY, vrEVT_TOOL_ZOOMOUT, vroomDrawerFrame::OnToolActionZoom)
	EVT_COMMAND(wxID_ANY, vrEVT_TOOL_SELECT, vroomDrawerFrame::OnToolActionSelect)
	EVT_COMMAND(wxID_ANY, vrEVT_TOOL_PAN, vroomDrawerFrame::OnToolActionPan)
END_EVENT_TABLE()





void  vroomDrawerFrame::_CreateControls(){
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxSplitterWindow* m_splitter2;
	m_splitter2 = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_BORDER|wxSP_LIVE_UPDATE );
	
	wxPanel* m_panel1;
	m_panel1 = new wxPanel( m_splitter2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	wxPanel * m_panel3 = new wxPanel( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL); // wxBORDER_SIMPLE|wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer51;
	bSizer51 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticText* m_staticText1;
	m_staticText1 = new wxStaticText( m_panel3, wxID_ANY, _("Number of features:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer51->Add( m_staticText1, 0, wxALL|wxEXPAND, 5 );
	
	m_NbStarCtrl = new wxSlider( m_panel3, wxID_ANY, 500, 1, 2000, wxDefaultPosition, wxDefaultSize, wxSL_LABELS );
	bSizer51->Add( m_NbStarCtrl, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 5 );
	
	wxString m_FeatureTypeCtrlChoices[] = { _("Points"), _("Lines"), _("Stars") };
	int m_FeatureTypeCtrlNChoices = sizeof( m_FeatureTypeCtrlChoices ) / sizeof( wxString );
	m_FeatureTypeCtrl = new wxRadioBox( m_panel3, wxID_ANY, _("Feature's type"), wxDefaultPosition, wxDefaultSize, m_FeatureTypeCtrlNChoices, m_FeatureTypeCtrlChoices, 1, wxRA_SPECIFY_COLS );
	m_FeatureTypeCtrl->SetSelection( 0 );
	bSizer51->Add( m_FeatureTypeCtrl, 0, wxALL|wxEXPAND, 5 );
	
	wxButton * m_button1 = new wxButton( m_panel3, ID_MENU_ADDMEMORYLAYER, _("Add memory layer"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer51->Add( m_button1, 0, wxALL, 5 );
	
	m_panel3->SetSizer( bSizer51 );
	m_panel3->Layout();
	bSizer51->Fit( m_panel3 );
	bSizer4->Add( m_panel3, 0, wxEXPAND, 5 );
	
	m_TocCtrl = new vrViewerTOCTree( m_panel1, wxID_ANY);
	bSizer4->Add( m_TocCtrl->GetControl(), 1, wxEXPAND, 5 );
	
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
	m_splitter2->SplitVertically( m_panel1, m_panel2, 200 );
	bSizer1->Add( m_splitter2, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	wxMenuBar* m_menubar1;
	m_menubar1 = new wxMenuBar( 0 );
	wxMenu* myFileMenu;
	myFileMenu = new wxMenu();
	wxMenuItem* m_menuItem1;
	m_menuItem1 = new wxMenuItem( myFileMenu, wxID_ADD, wxString( _("Add Layer...") ) , wxEmptyString, wxITEM_NORMAL );
	myFileMenu->Append( m_menuItem1 );
	
	wxMenuItem* m_menuItem2;
	m_menuItem2 = new wxMenuItem( myFileMenu, wxID_REMOVE, wxString( _("Remove Layer...") ) , wxEmptyString, wxITEM_NORMAL );
	myFileMenu->Append( m_menuItem2 );
	myFileMenu->AppendSeparator();
	myFileMenu->Append(wxID_INFO, _("Show Log...\tCtrl+L"));
    myFileMenu->AppendSeparator();
    myFileMenu->Append(wxID_DELETE, _("Generate crash..."));
	myFileMenu->Append(wxID_ABOUT, _("About"));
	myFileMenu->Append(wxID_EXIT, _("Quit"));
	
	m_menubar1->Append( myFileMenu, _("File") ); 
	
	wxMenu* myToolMenu;
	myToolMenu = new wxMenu();
	wxMenuItem* m_menuItem3;
	m_menuItem3 = new wxMenuItem( myToolMenu, wxID_ZOOM_IN, wxString( _("Zoom") ) + wxT('\t') + wxT("Ctrl+Z"), wxEmptyString, wxITEM_NORMAL );
	myToolMenu->Append( m_menuItem3 );
	
	wxMenuItem* m_menuItem4;
	m_menuItem4 = new wxMenuItem( myToolMenu, ID_MENU_SELECT, wxString( _("Select") ) + wxT('\t') + wxT("Ctrl+S"), wxEmptyString, wxITEM_NORMAL );
	myToolMenu->Append( m_menuItem4 );
	
	wxMenuItem* m_menuItem5;
	m_menuItem5 = new wxMenuItem( myToolMenu, ID_MENU_PAN, wxString( _("Pan") ) + wxT('\t') + wxT("Ctrl+P"), wxEmptyString, wxITEM_NORMAL );
	myToolMenu->Append( m_menuItem5 );
	
	wxMenuItem* m_separator1;
	m_separator1 = myToolMenu->AppendSeparator();
	
	wxMenuItem* m_menuItem6;
	m_menuItem6 = new wxMenuItem( myToolMenu, wxID_ZOOM_FIT, wxString( _("Zoom to fit") ) + wxT('\t') + wxT("Ctrl+0"), wxEmptyString, wxITEM_NORMAL );
	myToolMenu->Append( m_menuItem6 );
	
	m_menubar1->Append( myToolMenu, _("Tools") ); 
	
	this->SetMenuBar( m_menubar1 );
	this->Centre( wxBOTH );
}



double vroomDrawerFrame::_GetRandomNumber(double min, double max){
	double multiply=((double)rand()/(double)RAND_MAX);
	double myNumber = (max - min) * multiply;
	return myNumber + min;
}

vrLayer * vroomDrawerFrame::_GetMemoryLayerPoints(const wxFileName & name, int number, 
												  const vrRealRect & extent){
	vrLayerVectorOGR * myLayer = new vrLayerVectorOGR();
	if(myLayer->Create(name, wkbPoint)==false){
		wxFAIL;
		return NULL;
	}
				
	// adding features to the layer
	for (int i = 0; i<number; i++) {
		OGRPoint myPt;
		myPt.setX(_GetRandomNumber(extent.GetLeft(), extent.GetRight()));
		myPt.setY(_GetRandomNumber(extent.GetBottom(), extent.GetTop()));
		myLayer->AddFeature(&myPt, NULL);
	}
	return myLayer;
}



vrLayer * vroomDrawerFrame::_GetMemoryLayerLine (const wxFileName & name, int number, 
												 const vrRealRect & extent){
	vrLayerVectorOGR * myLayer = new vrLayerVectorOGR();
	if(myLayer->Create(name, wkbLineString)==false){
		wxFAIL;
		return NULL;
	}
	
	// adding features to the layer
	OGRLineString * myLine = (OGRLineString*) OGRGeometryFactory::createGeometry(wkbLineString);
	for (int i = 0; i<number; i++) {
		myLine->addPoint(_GetRandomNumber(extent.GetLeft(), extent.GetRight()),
						_GetRandomNumber(extent.GetBottom(), extent.GetTop()));
		if (myLine->getNumPoints() >= 10) {
			myLayer->AddFeature(myLine, NULL);
			OGRGeometryFactory::destroyGeometry(myLine);
			myLine = (OGRLineString*) OGRGeometryFactory::createGeometry(wkbLineString);
		}
	}
	if (myLine->getNumPoints() > 2) {
		myLayer->AddFeature(myLine, NULL);
	}
	OGRGeometryFactory::destroyGeometry(myLine);
	return myLayer;
}



vrLayer * vroomDrawerFrame::_GetMemoryLayerStar (const wxFileName & name, int number, 
												 const vrRealRect & extent){
	vrLayerVectorStar * myLayer = new vrLayerVectorStar();
	if(myLayer->Create(name, wkbPoint)==false){
		wxFAIL;
		return NULL;
	}
	// adding size field
    OGRFieldDefn myFieldStarSize ("starsize", OFTInteger);
	myLayer->AddField(myFieldStarSize);
	
	
	// adding features to the layer
	for (int i = 0; i<number; i++) {
		int myStarSize = wxRound(_GetRandomNumber(3.0, 20.0));
		OGRPoint myPt;
		myPt.setX(_GetRandomNumber(extent.GetLeft(), extent.GetRight()));
		myPt.setY(_GetRandomNumber(extent.GetBottom(), extent.GetTop()));
		myLayer->AddFeature(&myPt, &myStarSize);
	}
	return myLayer;
}




vroomDrawerFrame::vroomDrawerFrame(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title)
{
	wxIcon myVroomGISIcon;
	myVroomGISIcon.CopyFromBitmap(*_img_vroomgis_sml);
	SetIcon(myVroomGISIcon);

	// STATUS BAR
	CreateStatusBar(2);
    SetStatusText("Welcome to vroomDrawer");

	// CONTROLS
	_CreateControls();

	wxLog * myDlgLog = new tmLogGuiSeverity(wxLOG_Warning);
	delete wxLog::SetActiveTarget(myDlgLog);
	m_LogWnd = new wxLogWindow(this, "vroomDrawer Log", true, true);

	// Connect Events
	m_DisplayCtrl->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(vroomDrawerFrame::OnKeyDown),NULL, this);
	m_DisplayCtrl->Connect(wxEVT_KEY_UP, wxKeyEventHandler(vroomDrawerFrame::OnKeyUp),NULL, this);

	// VROOMGIS
	m_LayerManager = new vrLayerManager();
	m_ViewerLayerManager = new vrViewerLayerManager(m_LayerManager, this, m_DisplayCtrl , m_TocCtrl);
	m_DisplayCtrl->SetStatusCoordinates(GetStatusBar(), 1, _("Coordinates not available"));
}



vroomDrawerFrame::~vroomDrawerFrame(){
	// Disconnect Events
	m_DisplayCtrl->Disconnect(wxEVT_KEY_DOWN, wxKeyEventHandler(vroomDrawerFrame::OnKeyDown),NULL, this);
	m_DisplayCtrl->Disconnect(wxEVT_KEY_UP, wxKeyEventHandler(vroomDrawerFrame::OnKeyUp),NULL, this);

	// don't delete m_ViewerLayerManager, will be deleted by the manager
	wxDELETE(m_LayerManager);
	delete wxLog::SetActiveTarget (NULL);
	vroomgis_clear_images();
}



void vroomDrawerFrame::OnQuit(wxCommandEvent& event){
	Close();
}



void vroomDrawerFrame::OnAbout(wxCommandEvent& WXUNUSED(event)){
	wxHgVersionDlg myDlg (this, wxID_ANY, "About");
	myDlg.SetBitmapLogo(*_img_vroomgis);
	myDlg.ShowModal();
}


bool vroomDrawerFrame::OpenLayers (const wxArrayString & names){
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

		// if type is DIPS
		vrRenderVectorC2PDips * myRender = NULL;
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


void vroomDrawerFrame::OnLayerAdd(wxCommandEvent & event)
{
/*
    wxArrayString myTestFile;
    myTestFile.Add("/home/lucien/programmation/ColtopGIS/test_data/mntmo.c2d");
    OpenLayers(myTestFile);
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



void vroomDrawerFrame::OnLayerRemove(wxCommandEvent & event){

	wxArrayString myLayersName;
	for (int i = 0; i<m_ViewerLayerManager->GetCount(); i++) {
		vrRenderer * myRenderer = m_ViewerLayerManager->GetRenderer(i);
		wxASSERT(myRenderer);
		myLayersName.Add(myRenderer->GetLayer()->GetDisplayName().GetFullName());
	}

	if (myLayersName.IsEmpty()) {
		wxLogError("No layer opened, nothing to close");
		return;
	}


	wxMultiChoiceDialog  myChoiceDlg (this, "Select Layer(s) to close",
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



void vroomDrawerFrame::OnShowLog (wxCommandEvent & event)
{
	m_LogWnd->Show(true);
}



void vroomDrawerFrame::OnKeyDown(wxKeyEvent & event){
	m_KeyBoardState = wxKeyboardState(event.ControlDown(),
									  event.ShiftDown(),
									  event.AltDown(),
									  event.MetaDown());
	if (m_KeyBoardState.GetModifiers() != wxMOD_CMD) {
		event.Skip();
		return;
	}
	
	const vrDisplayTool * myTool = m_DisplayCtrl->GetTool();
	if (myTool == NULL) {
		event.Skip();
		return;
	}
		
	if (myTool->GetID() == wxID_ZOOM_IN) {
		m_DisplayCtrl->SetToolZoomOut();
	}
	event.Skip();
}



void vroomDrawerFrame::OnKeyUp(wxKeyEvent & event){
	if (m_KeyBoardState.GetModifiers() != wxMOD_CMD) {
		event.Skip();
		return;
	}
	
	const vrDisplayTool * myTool = m_DisplayCtrl->GetTool();
	if (myTool == NULL) {
		event.Skip();
		return;
	}
	
	if (myTool->GetID() == wxID_ZOOM_OUT || myTool->GetID() == wxID_ZOOM_IN) {
		m_DisplayCtrl->SetToolZoom();
	}
	event.Skip();
}



void vroomDrawerFrame::OnToolSelect (wxCommandEvent & event){
	m_DisplayCtrl->SetToolDefault();
}



void vroomDrawerFrame::OnToolZoom (wxCommandEvent & event){
	m_DisplayCtrl->SetToolZoom();
}



void vroomDrawerFrame::OnToolPan (wxCommandEvent & event){
	m_DisplayCtrl->SetToolPan();
}



void vroomDrawerFrame::OnZoomToFit (wxCommandEvent & event)
{
    m_ViewerLayerManager->ZoomToFit(true);
	m_ViewerLayerManager->Reload();
}


// crash function for better stack trace
static void bar(const char *p){
    char *pc = 0;
    *pc = *p;
    printf("bar: %s\n", p);
}


void foo(int n){
    bar("even");
}


void vroomDrawerFrame::OnTestCrashSoftware (wxCommandEvent & event){
    foo(31);
}


void vroomDrawerFrame::OnToolActionZoom (wxCommandEvent & event){
	vrDisplayToolMessage * myMsg = (vrDisplayToolMessage*)event.GetClientData();
	wxASSERT(myMsg);
	
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
	if (myMsg->m_EvtType == vrEVT_TOOL_ZOOM) {
		m_ViewerLayerManager->Zoom(myFittedRect);
	}
	else {
		m_ViewerLayerManager->ZoomOut(myFittedRect);
	}
	wxDELETE(myMsg);
}



void vroomDrawerFrame::OnToolActionSelect (wxCommandEvent & event){
	vrDisplayToolMessage * myMsg = (vrDisplayToolMessage*)event.GetClientData();
	wxASSERT(myMsg);
	
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
	
	wxDELETE(myMsg);
}



void vroomDrawerFrame::OnToolActionPan (wxCommandEvent & event){
	vrDisplayToolMessage * myMsg = (vrDisplayToolMessage*)event.GetClientData();
	wxASSERT(myMsg);
	
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
	
	wxDELETE(myMsg);
}



void vroomDrawerFrame::OnStarLayerAdd (wxCommandEvent & event){
	wxFileName myMemoryLayerName ("", _("Memory Features"), "memory");
	wxASSERT(myMemoryLayerName.GetExt() == "memory");
	
	// check if memory layer allready added
	m_ViewerLayerManager->FreezeBegin();
	for (int i = 0; i < m_ViewerLayerManager->GetCount(); i++) {
		if (m_ViewerLayerManager->GetRenderer(i)->GetLayer()->GetFileName() == myMemoryLayerName) {
			vrRenderer * myRenderer = m_ViewerLayerManager->GetRenderer(i);
			vrLayer * myLayer = myRenderer->GetLayer();
			wxASSERT(myRenderer);
			m_ViewerLayerManager->Remove(myRenderer);
			
			// close layer (not used anymore);
			m_LayerManager->Close(myLayer);
		}
	}
	
	// if extent not specified, init extent with window size
	vrRealRect myExtent = m_DisplayCtrl->GetCoordinate()->GetExtent();
	if (myExtent.IsOk() == false) {
		m_ViewerLayerManager->InitializeExtent(vrRealRect(0,
														  m_DisplayCtrl->GetSize().GetHeight(),
														  m_DisplayCtrl->GetSize().GetWidth(),
														  m_DisplayCtrl->GetSize().GetHeight() * -1.0));
		myExtent = m_DisplayCtrl->GetCoordinate()->GetExtent();
	}

	// init random engine
	srand((unsigned)time(NULL));
	
	vrLayer * myLayer = NULL;	
	vrRenderVector * myRender = new vrRenderVector();

	switch (m_FeatureTypeCtrl->GetSelection()) {
		case 1: // points
			myLayer = _GetMemoryLayerLine(myMemoryLayerName, m_NbStarCtrl->GetValue(), myExtent);
			break;
			
		case 2: // stars
			myLayer = _GetMemoryLayerStar(myMemoryLayerName, m_NbStarCtrl->GetValue(), myExtent);
			myRender->SetSize(1);
			myRender->SetColorPen(*wxBLUE);
			break;
			
			
		default: // points
			myLayer = _GetMemoryLayerPoints(myMemoryLayerName, m_NbStarCtrl->GetValue(), myExtent);
			myRender->SetSize(4);
			myRender->SetColorPen(*wxGREEN);
			break;
	}
	wxASSERT(myLayer);
	m_LayerManager->Add(myLayer);
	//vrLayer * myMemoryLayer =  m_LayerManager->GetLayer(myMemoryLayerName);	
	m_ViewerLayerManager->Add(-1, myLayer, myRender);
	m_ViewerLayerManager->FreezeEnd();
}

