#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "vroomdrawer_toolbmp.h"

wxString vroomDrawerBitmaps::SvgToolbar[] = {
    R"(<svg width="24" height="24" fill="none" stroke-linecap="round" stroke-linejoin="round" stroke-width="2" version="1.1" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg"> <path d="M0 0h24v24H0z" fill="none"/> <g stroke="%s"> <path d="M15 8h.01"/> <path d="m12.5 21h-6.5a3 3 0 0 1-3-3v-12a3 3 0 0 1 3-3h12a3 3 0 0 1 3 3v6.5"/> <path d="m3 16 5-5c0.928-0.893 2.072-0.893 3 0l4 4"/> <path d="m14 14 1-1c0.67-0.644 1.45-0.824 2.182-0.54"/> <path d="m16 19h6"/> <path d="m19 16v6"/> </g> </svg>)",
};

wxBitmap vroomDrawerBitmaps::GetToolbarBitmap(vroomDrawerBitmaps::ID_TOOLBAR id, const wxSize& size) {
    wxString my_bmp = wxString::Format(SvgToolbar[id], vrBitmaps::GetColor().ToStdString());
    return wxBitmapBundle::FromSVG(my_bmp, size).GetBitmap(size);
}
