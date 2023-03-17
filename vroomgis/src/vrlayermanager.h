/***************************************************************************
 vrlayermanager.h
 Manage the layers. Keep a list of all opened layers
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

#ifndef _VR_LAYERMANAGER_H_
#define _VR_LAYERMANAGER_H_

#include "wx/wxprec.h"

#ifndef WX_PRECOMP

#include <wx/filename.h>
#include <wx/wx.h>

#endif

#include "vrlayer.h"
#include "vrviewerlayermanager.h"

class vrLayerManager {
  protected:
    vrArrayViewerLayerManager m_viewerManagers;
    vrArrayLayer m_layers;

  public:
    vrLayerManager();

    virtual ~vrLayerManager();

    bool Open(const wxFileName& filename, bool readwrite = false);

    bool Add(vrLayer* layer);

    bool Close(vrLayer* layer);

    bool Erase(const wxFileName& filename);

    int GetCount();

    vrLayer* GetLayer(const wxFileName& filename);

    bool AddViewerLayerManager(vrViewerLayerManager* manager);

    //, vrViewerTOC * toc);
    bool RemoveViewerLayerManager(vrViewerLayerManager* manager);
};

#endif
