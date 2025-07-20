#ifndef VROOMDRAWER_TOOLBMP_H
#define VROOMDRAWER_TOOLBMP_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "vrbitmaps.h"

class vroomDrawerBitmaps {
  public:
    enum ID_TOOLBAR {
        ADD_MEMORY = 0,
    };

    static wxString SvgToolbar[];
    static wxBitmap GetToolbarBitmap(vroomDrawerBitmaps::ID_TOOLBAR id, const wxSize& size = wxSize(32, 32));
};

#endif /* VROOMDRAWER_TOOLBMP_H */
