#ifndef BOOKON_FRAMEABOUT_H
#define BOOKON_FRAMEABOUT_H

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

class FrameAbout : public wxDialog {
public:
    FrameAbout(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString&title = _("About"),
               const wxPoint&pos = wxDefaultPosition, const wxSize&size = wxDefaultSize,
               long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

    ~FrameAbout();

private:
    wxStaticBitmap* m_ctrl_bmp;
    wxTextCtrl* m_ctrl_txt;
    wxStdDialogButtonSizer* m_ctrl_btn;
    wxButton* m_ctrl_btnOK;

    void _create_controls();
};

#endif
