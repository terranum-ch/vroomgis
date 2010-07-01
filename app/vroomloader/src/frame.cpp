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

#include <wx/mstream.h>




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
	
	wxImage::AddHandler(new wxPNGHandler);
	
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
}




void vroomLoaderFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	Close(true);
}



void vroomLoaderFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	lsVersionDlg * myDlg = new lsVersionDlg (this, wxID_ANY, wxEmptyString);
	wxMemoryInputStream sm("\211PNG\r\n\032\n\000\000\000\rIHDR\000\000\000@\000\000\000@\b\006\000\000\000\252iq\336\000\000\002\356iCCPICC Profile\000\000x\001\205T\317k\023A\024\3766n\251\320\"\bZk\016\262x\220\"IY\253hE\3246\375\021bk\014\333\037\266E\220d3I\326n6\353\356&\265\245\210\344\342\321*\336E\355\241\a\377\200\036z\360d/J\205ZE(\336\253(b\241\027-\361\315nL\266\245\352\300\316~\363\3367\357}ov\337\000\rr\3224\365\200\004\344\r\307R\242\021il|Bj\374\210\000\216\242\011A4%U\333\354N$\006A\203s\371{\347\330z\017\201[V\303{\373w\262w\255\232\322\266\232\a\204\375@\340G\232\331*\260\357\027q\nY\022\002\210<\337\241)\307t\b\337\343\330\362\354\2179Nyx\301\265\017+=\304Y\"|@5-\316\177M\270S\315%\323@\203H8\224\365qR>\234\327\213\224\327\035\ainf\306\310\020\275O\220\246\273\314\356\253b\241\234N\366\016\220\275\235\364~N\263\336>\302!\302\013\031\255?F\270\215\236\365\214\325?\342a\341\244\346\304\206=\034\0305\364\370`\025\267\032\251\3705\302\024_\020M'\242Tq\331.\r\361\230\256\375V\362J\202p\2208\312da\200sZHO\327Ln\370\272\207\005}&\327\023\257\342wVQ\341y\217\023g\336\324\335\357E\332\002\2570\016\035\014\032\014\232\rHP\020E\004a\230\260P@\206<\03214\262r?#\253\006\033\223{2u$j\273tbD\261A{6\334=\267Q\244\335<\376(\"q\224C\265\b\222\374A\376*\257\311O\345y\371\313\\\260\330V\367\224\255\233\232\272\362\340;\305\345\231\271\032\036\327\323\310\343sM^|\225\324v\223WG\226\254yz\274\232\354\032?\354W\2271\346\2025\217\304s\260\373\361-_\225\314\227)\214\024\305\343U\363\352K\204uZ17\337\237l\177;=\342.\317.\265\326s\255\211\2137V\233\227g\375jH\373\356\252\274\223\346U\371O^\365\361\374g\315\304c\342)1&v\212\347!\211\227\305.\361\222\330K\253\013\342`m\307\b\235\225\206)\322m\221\372$\325``\232\274\365/]?[x\275F\177\014\365Q\224\031\314\322T\211\367\177\302*d4\235\271\016o\372\217\333\307\374\027\344\367\212\347\237(\002/l\340\310\231\272mSq\357\020\241e\245ns\256\002\027\277\321}\360\266nk\243~8\374\004X<\253\026\255R5\237 \274\001\034v\207z\036\350)\230\323\226\226\3159R\207,\237\223\272\351\312bR\314P\333CRR\327%\327eK\026\263\231Ub\351v\330\231\323\035n\034\2419B\367\304\247Je\223\372\257\361\021\260\375\260R\371\371\254R\331~N\032\326\201\227\372o\300\274\375E\300x\213\211\000\000\000\011pHYs\000\000\013\023\000\000\013\023\001\000\232\234\030\000\000\nsIDATx\001\355\232m\214\335E\025\207\227\355\366Mk\335R\302RdM\213/\240\022\205\252\210\024\354V\261\305\230\022ED\243\021A@b0\276E\022\305T%&(_l?H\305H\325j\352\013\025\251\255R\253\205\326\265%`\264\221\006\225\326`\033V\312V\205\226\n\225\245\354\256\3173w\316\345\277\333{\367\336\275{kHvO\362\353\2319s\346\314\2313\347\314\177\356\246\307\r\016\016\266\214gj\035\317\233w\357\023\001\230\310\200q\036\201\211\022\030\347\0110q\011N\224\300D\011\214\363\b\2645k\377\307A\315\2625\222\035\236\356M}\2737%\000n\276\331\216U\013B\263\327jJ\000\334\374\362\345\313\247\353tOOO5\337\307,\357\354\354la\251\377\216\331P\301\200'W\350\216\256\311iLb~\377\314\2313\227\266\266\266\336\314\354'\301\244\321Y\251[\273\237\365\332\373\373\373\227\035:thU\254]\367\354j\212\006\240Q`\323\000h\372\247\300\306\377\003\367\263\316\224\274n:\300F\375w^\303\031\300\011\264b`\000>\017\354\000\3558\326\017\216U\006`\272\345\031\326t\363\357\006\353X3e\240\003\215\322X\356\000\037Q\003\340\"\234i\an\336\014X\001\036\003\332N\351\001o\224\342\313\362,\006.\a/\313\206\256\201\257\003c\265\217\205\006K\200\305#{\272\263#:\323\335\250\275Z\363\260\375\251\274\316\021\270\301>\3079P*\303\342|d\006\316\003\252\215\342\304z\333\032\316\213\237M\333\323\321)\275\371x\226O\243m\0064\003\223\263\315N\354\375\013\230u\256\365\355,?*\000\312\353E\243%\020\351\377\036\034\261\346u\352q\260\036H}8\220\216\207:U\267!\302\204v[r\255\367\300\357\240{5\320\366%\364oDg\217k\204.r\365\217\207\315\000fJ\224\021\315\n\244\237\243\001&\"\365_D\373A\2403b\215v\240\362\211\320\036\323-\035\363\303&|\001p\221g2\377R^\263m\030_\306\270\2337c*\301\303:\000z\033\311\000OT\343]\3404`\011h\347'`\b\341\324 \247q2\302\251 Eg\210B\345\216z\256\341\215\377\210*p\337\000\236\362v\370o\021-T\016]F\177\005\362'\340\305\223\3767c\3328A\245\021\350@#\001\b{\227\346\2066v\201\273s\337OcJI\370\251\310~\a^\014\014Z\275\244\363O1\277\213\315=\250=\372\302\222X\r\014\200Y\240}\313\360;\300R\214 \367\320\226<\234U\300@N\0061\376\021\332\247\200})\375i\304\002\351rc\321\212e\201^\244\377Kh\377\023\350\220F\277\352\034hx*^\227\307CO^\017\342R]\026v\231\027k[\337{\201\013\212{\200\376\233\001\251\374\340\363A\224I\372Z\024\367\304\330_\201s\177\005\312\257\252\0217\037\216d\356wX\003F\330\205\346gy\262A_g\314\214\337\203\320\223\217\026\0173\3077\006\254d3\267o\312\266\3722_\234\345S3\367\200\254}'\276/\313\246e>\017\231\357\024\307V\351\344\027I\261\363\340\317\302\037\202\373)\363\004*\221\247'\275\267\304R\332u\243\357K\320\315\233\376\236\2025\273\000\2357\002\203\344:?\006>\231\275<\303\016\315\252\244\203\246\265\251\033\024\363\326 \370\004\2101\017\344\327\300\303\220\334\374?\200w\300\013\200\024s\315\240\220\355u\340\035\300\305\002K\331\200\362\362m\236\373q\272\257c\354i\340\306T\274&\217G\372G\032\256\314\343\352yR\257Ro\254\300N\330\377y\301\276\277\020\357\004\276\016\327g\376(\\\377>\223\375\213\337\017\027\"3\030\216}8\356\000kA\201\270+OH\033\016\207\221\307\006o\310z\032\361\03689\353\233\366Q\247F\336\023\b\233\033\303\011df\303h0\344S\312\334\b\300\273\262}/\327X\247\310c\223\276\025Pi\211\362\270,\353;\276\304\313C\372V\211\245\027\335[I\337%LJ\351\254\234\276NX\"\376\346\2778\353\272\341;\221\357C\256S\256b\312J\357\004\326a\244\344\017\025B\003\332\021\264\213\316\216\324\236\204\375\3603\331\360\037\310\224\337\001\0343#\315\202\341@\324\322\356?\005\232\223\333\316y\324\223\220~\001\376\000\336`\a\362\335\275)\265J\377\270\210\221>\037\234\001\242\256\207\177\373\215\252\364\376\022k\231\002\177\030\230\236\2226\022\021\204r;d#q\202\340!$\312\355\303p\263\240\003\350\217\aR$\203\352\376|\023H\261^\004\340\011d\373\3330bj\037\201\337\202\340V\340\005x!\375E\310\267\300=\3250\036\227\237\206\377\014\266\002\251\370\3557@\013\201\013:\367\016\354<\226\327\321Q3\312\200.\006\263\200za\237\346\020r\023\006q\a6\376\302\274\341A\350aL\324C\332\222N*\261\226^\370A\333\032\225{;\357\002v\304\272,\217\332?\021\331\276\302\370\r\303\306C\357\313Y\307@\272\271\370\325\346\246c-7\265\023\304Z\265\370\365\305\265lG\037\033~\011FB\370\025\227\370\272\274\356=\316S\350sU\245\377 X\005$\235_\212\374\\\344\351\324\350\373\2650}\334\224\365s;\220<}7\026w\304%%qJ\277\355\310\357\315\343Q\036y\270|_D\1778w\235\276,\324\237!\024kb\377H\r\204\377\036\200\344\033@\362\242\236\032w\200\213I\337\a\237\006\221&\237\244m\244\244KK,9\356\227b'N\030\300\270,\265\321\005^\003t\330S\371\021\220,\005/?OZ\322\251\353\201\337d\333Q\002r\003e\206|\005\204\0371\216\250D\332b}\357,\313\250h#k$\346<}\271\017D |#H\256=\253\374]\246\023\251\3625\332:\352D'\271\241N\340\r\353&\035\2732\357%\346\304\247iu\036Wg?\230\223\365R\352\333\256\a\314{30\020\256\247\017f\"\254\364d\317\355y\364=M\a\374\332\230-\362b\333\261G\300\361\261.m/x\345\356\347\254\262Ct\242F^A\333\03326\373u\332\276\272\234$|`td'\214p\324\365)\264\215\256\216\253wk\326I\301\011\a\2023nV\230\201E\304\267\332\257K\254\2679\333\011\377\"\350_\310:n<t+\361\a\030\237\n\342\220>\220\365\365sI\224\200\201H\251\014\377\033\251u\033\203W\001\225.\a\207A\320/\321\331\217\216\216X\367\332\360\224\226\002\353jx\372#\"J\020\314MW#7\330\207\332\251(,\006n\3069\226\245\244\211\270k^H\377\203IZ*\227m\264\275T\247e\231w\324y\340\265\340q\340ajK2k\334W+8\241\234\001,N\277\234\005\257\247m*\305i:\350&\345o\317\272\021\321\340>1\0357\000\362+B\217v\335%\200\356uy\2766\376\016fe;)\200\271\355\347\330q7\366\0248]y\021\310V\002u\326\003\003\037?\206\336D\333\0009\366\331r\006\320)f\301\037\011\366\006D\027\0037\036\247w?\355n \031\034\347\370\303\307\361M\340^p\016\220\256\004\337\005n\336\013k!m\323O\207\215\376p\322\206\301\363\367}\320mL=\300\334\3106\235\226\256(\261dg\023:\376\315\300\2133NV;q\201\3662n\240\204\264\013<\011,\213\316!\001@P$#h\000$\203`\024o\307\230i\232\034r \223Y`9\374\200\276\001P\377\\\372\013\220o\317:F\375\243\271]\2139_\254\311\212\305\207\326|d\027\0007\244O\337\003\222}\003 7\300\355@:\r?>\006\237\001\014L\374\022\244\311g\035\a\217\002\003q\341\334M\333\250\013o\315W\253\017\245\361\230K?\2458\274\003xI\306\234of\375\370\313\256Y\022c\265\370\206<7J,.\277\025\005\033;iO\317zfP\370\341\351z\371\325Zc[L@\3679\"b\221\266\257Dz&0\252\aY\350\256\347\264\206\266\230\022i\372\rF\256\315\243~\n\317d^\257}t\316\202\235\r\314\206Je\2008\235\342tx7\363|\376\252G3\225\321\211\264\377\004\346\000\351s\210o*\254\035~{\372;\300KA\0370\020\006(\270\355\311\340\241\212\001` ]\271.j\273\036\322Q\324MU/\031\323\336\271\226\330\265\310W\"7\013L\301\272\2119\261\241\b\256\251li\232\352\207\200\177\211\332\203ZZ\233~\"\327\242\361r`\211T\333\203A\350?*\3751X\226\241`\3645\222P\034\253\324V_9\264\031\330\020\333\200\213i\313\200xY\351\240\274\022b\3145c^\370\341%\033vW\347\265R\211T\362\247\036Yy\263\365(\327\322\301\271\250\327\017eG=q\035>\277\326\334Z\343\330\270`\230\315\2679\a\252\030\000\344\006\255&<\221f\222\251)m \r\367\300\347e'\257\242\357\2055\023x\237\214\206\314\004\177\250]\355$\376\037\202\345p\037\330\202MK\244\242=\344\341\213\323\252\222\006\252\01662\200OQ\25772\377\3638\354\347\314\2238\b\\\314\264\256w\321\320\225\267c{\200\377 \341\241\371\207\333\233c-\372\215\223\001h&\360$\335\003\035\035\035g\264\265\265\035\246o\000\242\024\334x\243\360e:8c\306\214\336\271s\347\236\224\017.\035\340X\374ov\011\030\314\201\265k\327N\352\355\355}\200 \374\206\023\273\b\307\233A\351-1{\366\354\237\355\336\275\273w\321\242E)\323\306j\270\351%\240C\244f\372,\341\344\351\264\337\202\350i\002c\0327L\330\3619=e\312\300\224\315\033\267l\334K[\337\353-\245\252\353\036\223\000\270Z\263\034\254\344y3m\037\263\000\344 \264vuuy\0016\215\266n\335\332\337\214\223\017\207\216i\000b\221\3473o\352\351<\2377Z\315\267\211\000T\213\314x\221Od\300x9\351j\373\234\310\200j\221\031/\362\211\014\030/']m\237\343>\003\376\a\3672h_\265ko\343\000\000\000\000IEND\256B`\202", 3515);
	myDlg->SetBitmapLogo(wxBitmap(wxImage(sm)));
	myDlg->CentreOnParent(wxBOTH);
	myDlg->ShowModal();

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
		/*for (unsigned int i = 0; i< myPathsFileName.GetCount(); i++) {
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
		m_ViewerLayerManager->FreezeEnd();*/
		
		
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


