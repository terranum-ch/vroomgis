/***************************************************************************
 vrviewerlayermanager.h
 List of supported drivers and misc function for them
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
#ifndef _VRVIEWERLAYERMANAGER_H
#define _VRVIEWERLAYERMANAGER_H

#include "wx/wxprec.h"

#ifndef WX_PRECOMP

#include <wx/wx.h>

#endif

#include <wx/filename.h>
#include "vrrenderer.h"


class vrLayerManager;

class vrLayer;

class vrViewerDisplay;

class vrViewerTOC;

class vrViewerDisplay;



// Array of images (bitmaps arn't thread safe)
WX_DECLARE_OBJARRAY(wxImage*, wxArrayImage);


class vrViewerLayerManager
        : public wxEvtHandler
{
private:
    vrLayerManager *m_layerManager;
    vrArrayRenderer m_renderers;
    vrViewerDisplay *m_display;
    vrViewerTOC *m_toc;
    wxWindow *m_windowParent;
    wxFileName m_perfMonitorFile;
    bool m_reloadThread;


    // using array of wxImages instead of array of wxBitmap because bitmap are
    // limited ressources on some systems
    wxArrayImage m_images;

    bool m_freezeStatus;
    bool m_computeExtentStatus;
    bool m_userDefinedExtent;


    // Member function for loading data
    bool _BitmapArrayInit();

    void _BitmapArrayDelete();

    bool _GetLayersData(long &vectorcount);

    bool _GetLayersExtent(bool onlyvisible);

    wxBitmap *_MergeBitmapData();

    int _ReloadThread(long &vectorcount);

    int _Reload(long &vectorcount, long &drawnvertex, long &skippedvertex);


    // event function
    void OnReload(wxCommandEvent &event);

DECLARE_EVENT_TABLE();

public:
    //if wxWindow is null we cannot link to event handling
    vrViewerLayerManager(vrLayerManager *parent, wxWindow *window, vrViewerDisplay *viewer, vrViewerTOC *toc = NULL);

    virtual ~vrViewerLayerManager();

    void InitializeExtent(const vrRealRect &extent);

    bool Add(long pos, vrLayer *layer, vrRender *render = NULL, vrLabel *label = NULL, bool visible = true);

    bool Move(long oldpos, long newpos);

    bool Remove(vrRenderer *renderer);

    bool Zoom(const vrRealRect &extent);

    bool ZoomOut(const vrRealRect &extent);

    void ZoomToFit(bool onlyvisible = false);

    long Select(const wxRect &extent, vrRenderer *selectlayer = NULL);

    void ClearSelection();

    int GetSelectionCount(int *sellayers = NULL);


    vrRenderer *GetRenderer(const unsigned int &index);

    vrRenderer *GetRenderer(const wxString &layername);

    inline vrViewerDisplay *GetDisplay() const;

    int GetCount();

    void FreezeBegin();

    void FreezeEnd();

    void Reload();

    bool StartEdition(vrRenderer *renderer);

    void StopEdition();

    vrRenderer *GetEditionRenderer();

    void StartPerfMonitor(const wxFileName &filename);

    void StopPerfMonitor();

    void SetEngineThreaded(bool enable);
};

inline vrViewerDisplay *vrViewerLayerManager::GetDisplay() const
{
    return m_display;
}


WX_DECLARE_OBJARRAY(vrViewerLayerManager*, vrArrayViewerLayerManager);


#endif
