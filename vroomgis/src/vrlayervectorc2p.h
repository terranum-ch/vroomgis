/***************************************************************************
 vrlayervectorsc2p.h

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

#ifndef _VRLAYERVECTORSC2P_H_
#define _VRLAYERVECTORSC2P_H_

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
// Include wxWidgets' headers
#ifndef WX_PRECOMP

#include <wx/wx.h>

#endif

#include <wx/regex.h>

#include "vrlayervector.h"

class vrRender;

class vrLabel;

enum CT_LAYER_TYPE {
    CT_DIP = 0,
    CT_POLYGON = 1
};

// For dealing with GIS data stored into C2P projects.
class vrLayerVectorC2P : public vrLayerVectorOGR {
  private:
    int m_activeLayerIndex;

  protected:
    virtual void _DrawPoint(wxDC* dc, OGRFeature* feature, OGRGeometry* geometry, const wxRect2DDouble& coord,
                            const vrRender* render, vrLabel* label, double pxsize);

    virtual void _DrawPolygon(wxDC* gdc, OGRFeature* feature, OGRGeometry* geometry, const wxRect2DDouble& coord,
                              const vrRender* render, vrLabel* label, double pxsize);

  public:
    vrLayerVectorC2P();

    virtual ~vrLayerVectorC2P();

    virtual bool Open(const wxFileName& filename, bool readwrite = false);

    virtual wxFileName GetDisplayName();

    virtual long AddFeature(OGRGeometry* geometry, void* data = nullptr);

    virtual bool DeleteFeature(long fid);

    virtual bool GetExtent(vrRealRect& rect);

    CT_LAYER_TYPE GetActiveLayerType();

    double GetNoDataValue();
};

#endif
