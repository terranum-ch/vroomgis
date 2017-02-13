# include "vrmemorylayerdialog.h"

vrMemoryLayerDialog::vrMemoryLayerDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
    _CreateControls();
}


void vrMemoryLayerDialog::_CreateControls() {
    this->SetSizeHints( wxDefaultSize, wxDefaultSize );

    wxBoxSizer* bSizer6;
    bSizer6 = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer* bSizer8;
    bSizer8 = new wxBoxSizer( wxHORIZONTAL );

    m_staticText2 = new wxStaticText( this, wxID_ANY, _("Layer name:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText2->Wrap( -1 );
    bSizer8->Add( m_staticText2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

    m_LayerNameCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 250,-1 ), 0 );
    m_LayerNameCtrl->SetMaxLength( 255 );
    bSizer8->Add( m_LayerNameCtrl, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );


    bSizer6->Add( bSizer8, 0, wxEXPAND, 5 );

    wxStaticBoxSizer* sbSizer1;
    sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Number of features") ), wxVERTICAL );

    m_NbStarCtrl = new wxSlider( this, wxID_ANY, 500, 1, 2000, wxDefaultPosition, wxDefaultSize, wxSL_LABELS );
    sbSizer1->Add( m_NbStarCtrl, 0, wxALL|wxEXPAND, 5 );


    bSizer6->Add( sbSizer1, 1, wxEXPAND|wxALL, 5 );

    wxString m_FeatureTypeCtrlChoices[] = { _("Points"), _("Lines"), _("Stars") };
    int m_FeatureTypeCtrlNChoices = sizeof( m_FeatureTypeCtrlChoices ) / sizeof( wxString );
    m_FeatureTypeCtrl = new wxRadioBox( this, wxID_ANY, _("Feature's type"), wxDefaultPosition, wxDefaultSize, m_FeatureTypeCtrlNChoices, m_FeatureTypeCtrlChoices, 1, wxRA_SPECIFY_COLS );
    m_FeatureTypeCtrl->SetSelection( 0 );
    bSizer6->Add( m_FeatureTypeCtrl, 0, wxALL|wxEXPAND, 5 );

    m_sdbSizer1 = new wxStdDialogButtonSizer();
    m_sdbSizer1OK = new wxButton( this, wxID_OK );
    m_sdbSizer1->AddButton( m_sdbSizer1OK );
    m_sdbSizer1Cancel = new wxButton( this, wxID_CANCEL );
    m_sdbSizer1->AddButton( m_sdbSizer1Cancel );
    m_sdbSizer1->Realize();

    bSizer6->Add( m_sdbSizer1, 0, wxEXPAND|wxALL, 5 );


    this->SetSizer( bSizer6 );
    this->Layout();
    bSizer6->Fit( this );

    this->Centre( wxBOTH );
}


vrMemoryLayerDialog::~vrMemoryLayerDialog() {
}
