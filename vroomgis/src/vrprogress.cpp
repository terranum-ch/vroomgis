/***************************************************************************
				vrprogress.cpp
                    
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

#include "vrprogress.h"

vrProgress::vrProgress() {
}

vrProgress::~vrProgress() {
}




wxProgressDialog * vrProgressSimple::m_ProgressWnd = NULL;
wxWindow * vrProgressSimple::m_Parent = NULL;
wxString vrProgressSimple::m_Title = _("Progress");
wxString vrProgressSimple::m_Message = wxEmptyString;
tmPercent * vrProgressSimple::m_Percent = NULL;
bool vrProgressSimple::m_Continue = true;

void vrProgressSimple::Init(wxWindow * parent, const wxString & title, const wxString & message)
{
	m_ProgressWnd = NULL;
	m_Parent = parent;
	m_Title = title;
	m_Message = message;
	m_Percent = new tmPercent(100);
	m_Continue = true;
}




int vrProgressSimple::GDALUpdateSimple(double dfComplete, const char * pszMessage, void * pProgressArg){
	wxASSERT(m_Percent);
	m_Percent->SetValue(dfComplete*100.0);
	if (m_ProgressWnd == NULL) {
		m_ProgressWnd = new wxProgressDialog(m_Title, m_Message, 100, m_Parent,
										   wxPD_AUTO_HIDE|wxPD_APP_MODAL|wxPD_CAN_ABORT);
	}
	
	if (m_Percent->IsNewStep()) {
		wxLogMessage("percent : %d", m_Percent->GetPercent());
		m_Continue = m_ProgressWnd->Update(m_Percent->GetPercent());
	}
	
	if (m_Continue == false) {
		wxDELETE(m_ProgressWnd);
		return FALSE;
	}
	
	return TRUE;
}



void vrProgressSimple::End()
{
	wxDELETE(m_Percent);
	wxDELETE(m_ProgressWnd);
}