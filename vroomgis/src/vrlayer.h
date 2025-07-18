/***************************************************************************
 vrlayer.h
                    Abstact base class for layers
 -------------------
 copyright            : (C) 2009 CREALP Lucien Schreiber
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

#ifndef _VRLAYER_H
#define _VRLAYER_H

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
// Include wxWidgets' headers
#ifndef WX_PRECOMP

#include <wx/wx.h>

#endif

#include <wx/filename.h>

#include "gdal_priv.h"    // gdal include
#include "ogrsf_frmts.h"  // ogr include
#include "vrdriver.h"     // Forward declaration not possible
#include "vrrealrect.h"

class vrRender;

class vrLabel;

// Abstact base class for layers
class vrLayer {
  protected:
    vrDRIVERS_TYPE m_driverType;
    wxFileName m_fileName;

  public:
    vrLayer();

    virtual ~vrLayer();

    virtual bool Create(const wxFileName& filename) {
        return false;
    }

    virtual bool Open(const wxFileName& filename, bool readwrite = false) {
        return false;
    }

    wxFileName GetFileName();

    virtual wxFileName GetDisplayName();

    virtual vrDRIVERS_TYPE GetType();

    virtual bool GetExtent(vrRealRect& rect) {
        return false;
    }

    virtual bool GetDataThread(wxImage* bmp, const vrRealRect& coord, double pxsize, const vrRender* render = nullptr,
                               vrLabel* label = nullptr) {
        return false;
    }

    virtual bool GetData(wxBitmap* bmp, const vrRealRect& coord, double pxsize, const vrRender* render = nullptr,
                         vrLabel* label = nullptr) {
        return false;
    }

    virtual bool IsOK() {
        return false;
    }

    virtual bool HasData() {
        return false;
    }
};

WX_DECLARE_OBJARRAY(vrLayer*, vrArrayLayer);

#endif
