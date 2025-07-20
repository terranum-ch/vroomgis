#ifndef _VR_BITMAPS_H_
#define _VR_BITMAPS_H_

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class vrBitmaps {
  public:
    static wxString SvgLogo;
    enum ID_TOOLBAR {
        SELECT = 0,
        PAN,
        ZOOM,
        ZOOM_FIT,
        EDIT,
        MODIFY
    };
    enum ID_TOC {
        CHECK_ON = 0,
        CHECK_OFF,
        FOLDER
    };

    enum ID_CURSORS {
        ZOOM_IN = 0,
        ZOOM_OUT,
        HAND,
        VERTEX_EDIT,
        VERTEX_ADD,
        VERTEX_REMOVE
    };

    static wxString SvgToolbar[];
    static wxString SvgToc[];
    static wxString SvgCursors[];

    static wxString GetColor();

    static wxBitmap GetLogo(const wxSize& size = wxSize(32, 32), const wxColour& colour = *wxBLACK);
    static wxBitmap GetToolbarBitmap(vrBitmaps::ID_TOOLBAR id, const wxSize& size = wxSize(32, 32));
    static wxBitmap GetTocBitmap(vrBitmaps::ID_TOC id, const wxSize& size = wxSize(16, 16));
    static wxBitmap GetCursorBitmap(vrBitmaps::ID_CURSORS id, const wxSize& size = wxSize(24, 24));
};

#endif  //_VR_BITMAPS_H_
