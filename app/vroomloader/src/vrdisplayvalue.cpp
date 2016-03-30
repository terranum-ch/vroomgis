/***************************************************************************
				vrdisplayvalue.cpp

                             -------------------
    copyright            : (C) 2010 CREALP Lucien Schreiber
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

#include "vrdisplayvalue.h"
#include "vroomgis.h"
#include "vrlayerraster.h"

BEGIN_EVENT_TABLE(vrDisplayValueDlg, wxDialog)
    EVT_CLOSE(vrDisplayValueDlg::OnCloseDlg)
END_EVENT_TABLE();


vrDisplayValueDlg::vrDisplayValueDlg(wxWindow *parent, vrViewerLayerManager *viewerlayermanager, wxWindowID id,
                                     const wxString &title, const wxPoint &pos, const wxSize &size, long style)
        : wxDialog(parent, id, title, pos, size, style)
{
    wxASSERT(viewerlayermanager);
    m_LayerViewerManager = viewerlayermanager;


    this->SetSizeHints(wxDefaultSize, wxDefaultSize);

    wxBoxSizer *bSizer2;
    bSizer2 = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer *bSizer3;
    bSizer3 = new wxBoxSizer(wxHORIZONTAL);

    wxFlexGridSizer *fgSizer1;
    fgSizer1 = new wxFlexGridSizer(2, 2, 0, 0);
    fgSizer1->SetFlexibleDirection(wxBOTH);
    fgSizer1->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

    wxStaticText *m_staticText2;
    m_staticText2 = new wxStaticText(this, wxID_ANY, wxT("Layer:"), wxDefaultPosition, wxDefaultSize, 0);
    m_staticText2->Wrap(-1);
    fgSizer1->Add(m_staticText2, 0, wxALL, 5);

    wxArrayString m_LayerChoiceChoices;
    m_LayerChoice = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(200, -1), m_LayerChoiceChoices, 0);
    m_LayerChoice->SetSelection(0);
    fgSizer1->Add(m_LayerChoice, 0, wxALL, 5);

    wxStaticText *m_staticText6;
    m_staticText6 = new wxStaticText(this, wxID_ANY, wxT("Values:"), wxDefaultPosition, wxDefaultSize, 0);
    m_staticText6->Wrap(-1);
    fgSizer1->Add(m_staticText6, 0, wxALL, 5);

    m_ValuesText = new wxStaticText(this, wxID_ANY, wxT("\n\n"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
    m_ValuesText->Wrap(-1);
    fgSizer1->Add(m_ValuesText, 0, wxALL | wxEXPAND, 5);

    bSizer3->Add(fgSizer1, 1, wxEXPAND, 5);

    bSizer2->Add(bSizer3, 1, wxEXPAND | wxALL, 5);

    this->SetSizer(bSizer2);
    this->Layout();
    this->Fit();

    // list raster layers
    for (int i = 0; i < m_LayerViewerManager->GetCount(); i++) {
        wxString myLayerName = m_LayerViewerManager->GetRenderer(i)->GetLayer()->GetDisplayName().GetFullName();
        m_LayerChoice->Append(myLayerName);
    }
    m_LayerChoice->SetSelection(0);

}

vrDisplayValueDlg::~vrDisplayValueDlg()
{
}


void vrDisplayValueDlg::OnCloseDlg(wxCloseEvent &event)
{
    wxASSERT(m_LayerViewerManager);
    m_LayerViewerManager->GetDisplay()->SetToolDefault();
    Destroy();
}

void vrDisplayValueDlg::UpdateValues(const wxPoint &pos)
{
    wxASSERT(m_LayerViewerManager);
    if (m_LayerChoice->GetCount() == 0) {
        return;
    }

    int iIndex = m_LayerChoice->GetSelection();
    if (iIndex >= m_LayerViewerManager->GetCount()) {
        wxLogError("Layer selected didn't exist anymore, close the Informations dialog");
        return;
    }

    vrDRIVERS_TYPE myType = m_LayerViewerManager->GetRenderer(iIndex)->GetLayer()->GetType();

    if (myType < vrDRIVER_RASTER_TIFF) {
        wxLogWarning("This isn't a raster layer, please select a raster layer");
        return;
    }
    vrLayerRasterGDAL *myRasterLayer = (vrLayerRasterGDAL *) m_LayerViewerManager->GetRenderer(iIndex)->GetLayer();
    wxASSERT(myRasterLayer);

    wxPoint2DDouble myCoord;
    if (!m_LayerViewerManager->GetDisplay()->GetCoordinate()->ConvertFromPixels(pos, myCoord)) {
        wxLogError("Converting position %d, %d to real coordinates failed", pos.x, pos.y);
        return;
    }

    wxArrayDouble myValues;
    myRasterLayer->GetPixelValue(myCoord.m_x, myCoord.m_y, myValues);
    if (myValues.GetCount() == 0) {
        return;
    }

    wxString myTextValues = wxEmptyString;
    for (unsigned int i = 0; i < myValues.GetCount(); i++) {
        myTextValues.Append(wxString::Format(_T("%.4f\n"), myValues.Item(i)));
    }
    myTextValues.RemoveLast(1);
    m_ValuesText->SetLabel(myTextValues);
}


vrDisplayValueTool::vrDisplayValueTool(vrViewerDisplay *display, vrDisplayValueDlg *dialog)
{
    Create(display, wxID_DEFAULT, "Select", wxCursor(wxCURSOR_ARROW));
    wxASSERT(dialog);
    m_Dlg = dialog;
}

vrDisplayValueTool::~vrDisplayValueTool()
{
}


bool vrDisplayValueTool::MouseMove(const wxMouseEvent &event)
{
    if (m_Dlg == NULL) {
        return false;
    }

    if (!m_Dlg->IsShown()) {
        return false;
    }

    m_Dlg->UpdateValues(event.GetPosition());
    return true;
}

