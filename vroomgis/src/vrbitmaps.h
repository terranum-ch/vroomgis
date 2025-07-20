//
// Created by Lucien Schreiber on 20.07.2025.
//

#ifndef _VR_BITMAPS_H_
#define _VR_BITMAPS_H_

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class vrBitmaps {
  public:
    static wxString SvgLogo;
    static wxString SvgToolbar[];

    static wxString GetColor();

    static wxBitmap GetLogoBitmap(const wxSize& size = wxSize(32, 32), const wxColour & colour = *wxBLACK);


};

#endif  //_VR_BITMAPS_H_
