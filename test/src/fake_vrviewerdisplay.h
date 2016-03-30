/***************************************************************************
 fake_vrviewerdisplay.h

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
#ifndef _FAKE_VRVIEWERDISPLAY_H
#define _FAKE_VRVIEWERDISPLAY_H

#include "wx/wxprec.h"

#ifndef WX_PRECOMP

#include <wx/wx.h>

#endif


#include "vrviewerdisplay.h"

class FakevrViewerDisplay
        : public vrViewerDisplay
{
    wxSize m_WndSize;

public:
    FakevrViewerDisplay(const wxSize &size = wxSize(600, 400));

    virtual ~FakevrViewerDisplay();

    void SetFakeSize(const wxSize &size)
    {
        m_WndSize = size;
    }

    virtual void DoGetSize(int *w, int *h) const;

    virtual wxSize GetSize()
    {
        int w, h;
        DoGetSize(&w, &h);
        return wxSize(w, h);
    }

};

#endif
