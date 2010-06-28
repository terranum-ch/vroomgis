/***************************************************************************
				vrdisplayvalue.h

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

#ifndef _VRDISPLAYVALUE_H
#define _VRDISPLAYVALUE_H

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
// Include wxWidgets' headers
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif


#include "vrdisplaytool.h"

class vrViewerLayerManager;
class vrViewerDisplay;

class vrDisplayValueDlg : public wxDialog {
private:
    vrViewerLayerManager * m_LayerViewerManager;

	// controls
	wxChoice* m_LayerChoice;
	wxStaticText* m_ValuesText;

	void OnCloseDlg(wxCloseEvent & event);

	DECLARE_EVENT_TABLE();
public:
    vrDisplayValueDlg(wxWindow * parent,
					  vrViewerLayerManager * viewerlayermanager,
					  wxWindowID id = wxID_ANY,
					  const wxString & title = "Display Raster value",
					  const wxPoint& pos = wxDefaultPosition,
					  const wxSize& size = wxSize(300, -1),
					  long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER); //wxSTAY_ON_TOP );
    virtual ~vrDisplayValueDlg();

    void UpdateValues(const wxPoint & pos);

};



class vrDisplayValueTool : public vrDisplayTool {
  private:
    vrDisplayValueDlg * m_Dlg;

  public:
    vrDisplayValueTool(vrViewerDisplay * display, vrDisplayValueDlg * dialog);
    virtual ~vrDisplayValueTool();

    virtual bool MouseDown(const wxMouseEvent & event){ return true;}
    virtual bool MouseUp(const wxMouseEvent & event){ return true;}
    virtual bool MouseMove(const wxMouseEvent & event);

};
#endif
