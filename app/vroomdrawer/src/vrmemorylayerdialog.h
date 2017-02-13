/*********************************************//**
@file vrmemorylayerdialog
@brief Add memory layer dialog
@date 13 février 2017
@author Lucien SCHREIBER
 *************************************************/

#ifndef _VRMEMORY_LAYER_DIALOG_H__
#define _VRMEMORY_LAYER_DIALOG_H__

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#	include <wx/wx.h>
#endif


///////////////////////////////////////////////////////////////////////////////
/// Class vrMemoryLayerDialog
///////////////////////////////////////////////////////////////////////////////
class vrMemoryLayerDialog : public wxDialog {
private:
    wxSlider* m_NbStarCtrl;
    wxRadioBox* m_FeatureTypeCtrl;
    wxStaticText* m_staticText2;
    wxTextCtrl* m_LayerNameCtrl;
    wxStdDialogButtonSizer* m_sdbSizer1;
    wxButton* m_sdbSizer1OK;
    wxButton* m_sdbSizer1Cancel;

public:
    vrMemoryLayerDialog(wxWindow* parent,
                        wxWindowID id = wxID_ANY,
                        const wxString& title = _("Add memory layer"),
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = wxDEFAULT_DIALOG_STYLE);
    ~vrMemoryLayerDialog();

};
#endif //_VRMEMORY_LAYER_DIALOG_H__