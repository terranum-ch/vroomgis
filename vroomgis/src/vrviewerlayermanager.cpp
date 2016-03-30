/***************************************************************************
				vrviewerlayermanager.cpp

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


#include "vrviewerlayermanager.h"
#include "vrevent.h"
#include "vrlayermanager.h"
#include "vrlayer.h"
#include "vrlayervector.h"
#include "vrviewertoc.h"
#include "vrviewerdisplay.h"
#include "vrcoordinate.h"
#include "vrperformance.h"


wxDEFINE_EVENT(vrEVT_VLM_RELOAD, wxCommandEvent);
wxDEFINE_EVENT(vrEVT_TOOL_ZOOM, wxCommandEvent);
wxDEFINE_EVENT(vrEVT_TOOL_ZOOMOUT, wxCommandEvent);
wxDEFINE_EVENT(vrEVT_TOOL_SELECT, wxCommandEvent);
wxDEFINE_EVENT(vrEVT_TOOL_PAN, wxCommandEvent);
wxDEFINE_EVENT(vrEVT_TOOL_SIGHT, wxCommandEvent);
wxDEFINE_EVENT(vrEVT_TOOL_EDIT, wxCommandEvent);
wxDEFINE_EVENT(vrEVT_TOOL_EDIT_FINISHED, wxCommandEvent);
wxDEFINE_EVENT(vrEVT_TOOL_MODIFY, wxCommandEvent);
wxDEFINE_EVENT(vrEVT_TOOL_MODIFY_FINISHED, wxCommandEvent);

BEGIN_EVENT_TABLE(vrViewerLayerManager, wxEvtHandler)
                EVT_COMMAND(wxID_ANY, vrEVT_VLM_RELOAD, vrViewerLayerManager::OnReload) END_EVENT_TABLE()


vrViewerLayerManager::vrViewerLayerManager(vrLayerManager *parent, wxWindow *window, vrViewerDisplay *viewer,
                                           vrViewerTOC *toc)
{

    //wxASSERT(viewer);
    wxASSERT(parent);
    m_windowParent = NULL;
    m_display = viewer;
    m_toc = toc;
    m_layerManager = parent;
    m_freezeStatus = false;
    m_userDefinedExtent = false;
    m_computeExtentStatus = false;
    m_perfMonitorFile.Clear();
    m_reloadThread = false;
    if (window) {
        m_windowParent = window;
        m_windowParent->PushEventHandler(this);
    }

    if (m_display == NULL) {
        wxLogError("No display attached");
    } else {
        m_display->SetViewerLayerManager(this);
    }


    if (m_toc == NULL) {
        //wxLogError("No toc defined");
    } else {
        m_toc->SetViewerLayerManager(this);
    }

    // add this viewer layer manager to the vrLayerManager
    m_layerManager->AddViewerLayerManager(this);
}


vrViewerLayerManager::~vrViewerLayerManager()
{
    //wxLogMessage("ViewerLayerManager Dtor called");

    if (m_windowParent) {
        m_windowParent->RemoveEventHandler(this);
    }


    wxDELETE(m_display);
    wxDELETE(m_toc);


    // delete renderers
    unsigned int iCount = m_renderers.GetCount();
    for (unsigned int i = 0; i < iCount; i++) {
        vrRenderer *myRenderer = m_renderers.Item(0);
        wxASSERT(myRenderer);
        wxDELETE(myRenderer);
        m_renderers.RemoveAt(0);
    }

}

void vrViewerLayerManager::InitializeExtent(const vrRealRect &extent)
{
    // Check input
    vrRealRect myExtent(extent);
    if (myExtent.m_height > 0) {
        myExtent.m_height = -myExtent.m_height;
    }

    m_userDefinedExtent = true;
    m_computeExtentStatus = false;

    vrCoordinate *coord = m_display->GetCoordinate();
    wxASSERT(coord);

    coord->ClearLayersExtent();
    coord->ClearPixelSize();
    coord->AddLayersExtent(myExtent);

    if (coord->ComputeFullExtent() == false) {
        wxLogError(_("Couldn't apply the desired extent."));
        m_userDefinedExtent = false;
        m_computeExtentStatus = true;
        return;
    }
}


bool vrViewerLayerManager::Add(long pos, vrLayer *layer, vrRender *render, vrLabel *label, bool visible)
{
    wxASSERT(layer);
    vrRenderer *myRenderer = new vrRenderer(layer, render, label);
    myRenderer->SetVisible(visible);


    // compute coordinate if layer added is visible and
    // there is no other visible layers
    if (visible == true && m_computeExtentStatus == false && m_userDefinedExtent == false) {
        m_computeExtentStatus = true;
        for (unsigned int i = 0; i < m_renderers.GetCount(); i++) {
            if (m_renderers.Item(i)->GetVisible() == true) {
                m_computeExtentStatus = false;
                break;
            }
        }
    }

    if (m_computeExtentStatus == true) {
        //wxLogMessage("Computing extent required");
    }


    if (pos >= (signed) m_renderers.GetCount()) {
        pos = m_renderers.GetCount();
    }

    if (pos == -1) {
        m_renderers.Insert(myRenderer, 0);
    } else {
        m_renderers.Insert(myRenderer, pos);
    }

    if (m_toc) {
        m_toc->Add(pos, myRenderer);
    }

    // if not freezed, refresh imediatelly.
    if (m_freezeStatus == false) {
        if (m_windowParent) {
            if (m_computeExtentStatus == true) {
                _GetLayersExtent(true);
                m_computeExtentStatus = false;
            }

            Reload();
        }
    }


    return true;
}


bool vrViewerLayerManager::Move(long oldpos, long newpos)
{
    wxASSERT(oldpos >= 0 && newpos >= 0);
    wxASSERT(oldpos < (signed) m_renderers.GetCount() && newpos < (signed) m_renderers.GetCount());
    wxASSERT(oldpos != newpos);

    int myNewPos = newpos;
    if (newpos > oldpos + 1) {
        myNewPos = myNewPos - 1;
    }

    vrRenderer *myRenderer = m_renderers.Item(oldpos);
    wxASSERT(myRenderer);
    m_renderers.RemoveAt(oldpos);
    m_renderers.Insert(myRenderer, myNewPos);


    if (m_toc && m_toc->Move(oldpos, newpos) == false) {
        wxLogError("Moving layer '%s' from position %ld to %ld failed",
                   myRenderer->GetLayer()->GetDisplayName().GetFullName(), oldpos, newpos);
        return false;
    }

    if (m_freezeStatus == false && m_windowParent) {
        Reload();
    }

    return true;
}


bool vrViewerLayerManager::Remove(vrRenderer *renderer)
{
    wxASSERT(renderer);

    int myRemoveIndex = wxNOT_FOUND;
    for (int i = 0; i < GetCount(); i++) {
        vrRenderer *myRenderer = GetRenderer(i);
        wxASSERT(myRenderer);
        if (myRenderer == renderer) {
            myRemoveIndex = i;
            break;
        }
    }

    if (myRemoveIndex == wxNOT_FOUND) {
        wxLogError("Unable to remove renderer, maybe allready removed");
        return false;
    }

    // remove from TOC
    if (m_toc) {
        m_toc->Remove(myRemoveIndex);
    }

    // remove from Renderer array
    vrRenderer *myRenderer = GetRenderer(myRemoveIndex);
    wxASSERT(myRenderer);
    wxDELETE(myRenderer);
    m_renderers.RemoveAt(myRemoveIndex);

    // if not freezed, refresh imediatelly.
    if (m_freezeStatus == false) {
        if (m_windowParent) {
            Reload();
        }
    }
    return true;
}


bool vrViewerLayerManager::ZoomOut(const vrRealRect &extent)
{
    if (extent.IsOk() == false) {
        wxLogError("Specified extent (%f, %f, %f, %f) isn't supported", extent.GetLeft(), extent.GetTop(),
                   extent.GetRight(), extent.GetBottom());
        return false;
    }

    vrRealRect myFullExtent = GetDisplay()->GetCoordinate()->GetExtent();
    double DivRatio = myFullExtent.m_width / extent.m_width;
    if (DivRatio < 1.0) {
        DivRatio = 1.0;
    }
    if (DivRatio > 10.0) { // For convenience purposes
        DivRatio = 10.0;
    }
    wxPoint2DDouble myCenter = extent.GetCentre();
    myFullExtent.Scale(DivRatio);
    myFullExtent.SetCentre(myCenter);
    return Zoom(myFullExtent);
}


bool vrViewerLayerManager::Zoom(const vrRealRect &extent)
{
    if (extent.IsOk() == false) {
        wxLogError("Specified extent (%f, %f, %f, %f) isn't supported", extent.GetLeft(), extent.GetTop(),
                   extent.GetRight(), extent.GetBottom());
        return false;
    }

    wxASSERT(m_display);

    // create fitted rectangle
    vrCoordinate *myCoord = m_display->GetCoordinate();
    wxASSERT(myCoord);
    vrRealRect myFittedRect = myCoord->GetRectFitted(extent);
    if (myFittedRect.IsOk() == false) {
        wxLogError("Wasn't able to zoom");
        return false;
    }

    myCoord->SetExtent(myFittedRect);

    // update drawing
    if (m_windowParent) {
        Reload();
    }
    return true;
}


void vrViewerLayerManager::ZoomToFit(bool onlyvisible)
{
    _GetLayersExtent(onlyvisible);
}


long vrViewerLayerManager::Select(const wxRect &extent, vrRenderer *selectlayer)
{
    // make selection on this layer if not null, otherwise use selection from TOC
    vrRenderer *mySelectLayer = selectlayer;
    if (selectlayer == NULL) {
        int myIndex = m_toc->GetSelection();
        if (myIndex == wxNOT_FOUND) {
            return wxNOT_FOUND;
        }
        mySelectLayer = m_renderers.Item(myIndex);
    }
    wxASSERT(mySelectLayer);

    if (mySelectLayer->GetLayer()->GetType() < vrDRIVER_VECTOR_SHP ||
        mySelectLayer->GetLayer()->GetType() > vrDRIVER_VECTOR_MEMORY) {
        wxLogMessage(_("Selection on raster isn't valid"));
        return wxNOT_FOUND;
    }

    // convert to real coordinates
    vrRealRect myRealCoord;
    wxRect myPxExtent = extent;
    if (myPxExtent.GetWidth() == 1 && myPxExtent.GetHeight() == 1) {
        myPxExtent.x = myPxExtent.x - 1;
        myPxExtent.y = myPxExtent.y - 1;
        myPxExtent.width = 3;
        myPxExtent.height = 3;
    }
    m_display->GetCoordinate()->ConvertFromPixels(myPxExtent, myRealCoord);

    vrLayerVector *myLayer = static_cast<vrLayerVector *>(mySelectLayer->GetLayer());
    wxASSERT(myLayer);
    return myLayer->Select(myRealCoord);
}


void vrViewerLayerManager::ClearSelection()
{
    for (int i = 0; i < GetCount(); i++) {
        if (m_renderers.Item(i)->GetLayer()->GetType() >= vrDRIVER_VECTOR_SHP &&
            m_renderers.Item(i)->GetLayer()->GetType() <= vrDRIVER_VECTOR_MEMORY) {
            vrLayerVector *myLayer = (vrLayerVector *) m_renderers.Item(i)->GetLayer();
            wxASSERT(myLayer);
            myLayer->GetSelectedIDs()->Clear();
        }
    }
}


int vrViewerLayerManager::GetSelectionCount(int *sellayers)
{
    int iTotal = 0;
    for (int i = 0; i < GetCount(); i++) {
        if (m_renderers.Item(i)->GetLayer()->GetType() >= vrDRIVER_VECTOR_SHP &&
            m_renderers.Item(i)->GetLayer()->GetType() <= vrDRIVER_VECTOR_MEMORY) {
            vrLayerVector *myLayer = (vrLayerVector *) m_renderers.Item(i)->GetLayer();
            wxASSERT(myLayer);
            int mySelCount = myLayer->GetSelectedIDs()->GetCount();
            if (mySelCount > 0) {
                iTotal += mySelCount;
                if (sellayers != NULL) {
                    *sellayers = *sellayers + 1;
                }
            }
        }
    }
    return iTotal;
}


vrRenderer *vrViewerLayerManager::GetRenderer(const unsigned int &index)
{
    // check index into limits
    if (index > m_renderers.GetCount() - 1) {
        wxLogError("Trying to Get Renderer out of bounds");
        return NULL;
    }

    vrRenderer *renderer = m_renderers.Item(index);
    wxASSERT(renderer);
    return renderer;
}


vrRenderer *vrViewerLayerManager::GetRenderer(const wxString &layername)
{
    for (unsigned int i = 0; i < m_renderers.GetCount(); i++) {
        // check on the full path (with file name)
        if (m_renderers.Item(i)->GetLayer()->GetFileName().GetFullPath() == layername) {
            return m_renderers.Item(i);
        }
        // check on the path without the file name (for ESRI raster data)
        if (m_renderers.Item(i)->GetLayer()->GetFileName().GetPath() == layername) {
            return m_renderers.Item(i);
        }
    }
    return NULL;
}


// number of renderers
int vrViewerLayerManager::GetCount()
{
    return m_renderers.GetCount();
}


void vrViewerLayerManager::FreezeBegin()
{
    wxASSERT(m_toc);
    wxASSERT(m_freezeStatus == false);
    m_freezeStatus = true;
    m_toc->FreezeBegin();
}

void vrViewerLayerManager::FreezeEnd()
{
    wxASSERT(m_toc);
    wxASSERT(m_freezeStatus == true);
    m_freezeStatus = false;
    m_toc->FreezeEnd();

    // compute layers extent
    if (m_computeExtentStatus == true) {
        _GetLayersExtent(true);
        m_computeExtentStatus = false;
    }


    // reloading data
    Reload();

}


void vrViewerLayerManager::Reload()
{
    vrPerformance *myPerf = NULL;
    if (m_perfMonitorFile.IsOk() == true) {
        myPerf = new vrPerformance(m_perfMonitorFile.GetFullPath(),
                                   "Number of layers\tTotal Vector Objects\tDrawn Vertex\tSkipped Vertex\tWindow Resolution(x)\tWindow Resolution(y)\t");
    }

    int myCountLayer = 0;
    long myVectorCount = 0;
    long myVertexDrawn = 0;
    long myVertexSkipped = 0;
    if (m_reloadThread == true) {
        myCountLayer = _ReloadThread(myVectorCount);
    } else {
        myCountLayer = _Reload(myVectorCount, myVertexDrawn, myVertexSkipped);
    }

    if (myVectorCount > 0) {
        wxLogDebug(_("Total Vector drawn: %ld"), myVectorCount);
    }

    if (myCountLayer == 0) {
        wxDELETE(myPerf);
        return;
    }

    if (myPerf != NULL) {
        myPerf->StopWork(wxString::Format("%d\t%ld\t%ld\t%ld\t%d\t%d;", myCountLayer, myVectorCount, myVertexDrawn,
                                          myVertexSkipped, m_display->GetSize().GetWidth(),
                                          m_display->GetSize().GetHeight()));
    }
}


bool vrViewerLayerManager::StartEdition(vrRenderer *renderer)
{
    bool bEditionStarted = false;
    for (unsigned int i = 0; i < m_renderers.GetCount(); i++) {
        vrRenderer *myRenderer = m_renderers.Item(i);
        wxASSERT(myRenderer);

        if (myRenderer == renderer) {
            myRenderer->SetInEdition(true);
            bEditionStarted = true;

            if (m_toc) {
                m_toc->SetEditStyle(i);
            }
        }
    }
    return bEditionStarted;
}


void vrViewerLayerManager::StopEdition()
{
    for (unsigned int i = 0; i < m_renderers.GetCount(); i++) {
        vrRenderer *myRenderer = m_renderers.Item(i);
        wxASSERT(myRenderer);
        myRenderer->SetInEdition(false);

        if (m_toc) {
            m_toc->SetNormalStyle(i);
        }
    }
}


vrRenderer *vrViewerLayerManager::GetEditionRenderer()
{
    for (unsigned int i = 0; i < m_renderers.GetCount(); i++) {
        vrRenderer *myRenderer = m_renderers.Item(i);
        wxASSERT(myRenderer);
        if (myRenderer->IsInEdition() == true) {
            return myRenderer;
        }
    }
    return NULL;
}


void vrViewerLayerManager::StartPerfMonitor(const wxFileName &filename)
{
    m_perfMonitorFile = filename;
}


void vrViewerLayerManager::StopPerfMonitor()
{
    m_perfMonitorFile.Clear();
}


void vrViewerLayerManager::SetEngineThreaded(bool enable)
{
#ifdef __LINUX__
    if (enable == true) {
        if (wxMessageBox(_("Engine not fully compatible with Linux, vector layers will not be rendered! Continue ?"),
                         _("Linux Problem"), wxYES_NO) == wxNO) {
            return;
        }
    }
#endif
    m_reloadThread = enable;
}


bool vrViewerLayerManager::_BitmapArrayInit()
{
    wxASSERT(m_images.GetCount() == 0);
    if (m_renderers.GetCount() == 0) {
        //wxLogError("Unable to create images, no layer opened");
        return false;
    }

    wxSize myDisplaySize = m_display->GetSize();

    for (unsigned int i = 0; i < m_renderers.GetCount(); i++) {

        // create image only for visible layers
        if (m_renderers.Item(i)->GetVisible()) {
            wxImage *myImage = new wxImage(myDisplaySize);
            /*if (myImage->HasAlpha() == false) {
                myImage->InitAlpha();
            }*/

            //wxLogMessage("Creation des images : %p", myImage);

            m_images.Add(myImage, 1);

        }
    }
    //wxLogMessage("Created %ld images", m_images.GetCount());
    return true;
}


void vrViewerLayerManager::_BitmapArrayDelete()
{
    if (m_images.GetCount() > 0) {
        //wxLogMessage("Deleting %ld images", m_images.GetCount());

        unsigned int iCountImg = m_images.GetCount();
        for (unsigned j = 0; j < iCountImg; j++) {
            wxImage *myImage = m_images.Item(0);
            wxASSERT(myImage);
            wxDELETE(myImage);
            m_images.RemoveAt(0);
        }
        wxASSERT(m_images.GetCount() == 0);
    }
}


bool vrViewerLayerManager::_GetLayersData(long &vectorcount)
{

    // gettting display coordinates
    wxASSERT(m_display);
    vrCoordinate *myCoordinate = m_display->GetCoordinate();
    wxASSERT(myCoordinate);

    vectorcount = 0;
    // getting data from vrRenderer -> vrLayer
    bool bReturn = true;
    int iImageIndex = 0;
    if (m_images.GetCount() == 0) {
        return false;
    }

    wxArrayInt myInvalidRaster;
    int iTotLayers = m_renderers.GetCount();
    for (int i = iTotLayers - 1; i >= 0; i--) {
        if (m_renderers.Item(i)->GetVisible() == true) {
            long myVectorCount = 0;
            if (m_renderers.Item(i)->GetBitmapDataThread(m_images.Item(iImageIndex), myCoordinate->GetExtent(),
                                                         myCoordinate->GetPixelSize(), myVectorCount) == false) {
                wxLogMessage("No data to display for '%s' !",
                             m_renderers.Item(i)->GetLayer()->GetDisplayName().GetFullName());
                bReturn = false;
                myInvalidRaster.Add(iImageIndex);
            }
            vectorcount = vectorcount + myVectorCount;
            iImageIndex++;
        }
    }

    // remove invalid raster
    for (int i = myInvalidRaster.GetCount() - 1; i >= 0; i--) {
        wxImage *myImage = m_images.Item(myInvalidRaster.Item(i));
        wxASSERT(myImage);
        wxDELETE(myImage);
        m_images.RemoveAt(myInvalidRaster.Item(i));
        //wxLogMessage("Removing image at index : %d", i);
    }
    return bReturn;
}


bool vrViewerLayerManager::_GetLayersExtent(bool onlyvisible)
{

    vrCoordinate *myCoordinate = m_display->GetCoordinate();
    wxASSERT(myCoordinate);

    // clear actual layer extent
    myCoordinate->ClearLayersExtent();
    myCoordinate->ClearPixelSize();

    // if there is a raster layer or a vector layer containing features visible
    // then we totally ignore coordinates from empty vector layers.
    bool bHasRealCoordinates = false;
    for (unsigned int i = 0; i < m_renderers.GetCount(); i++) {
        vrRenderer *myRenderer = m_renderers.Item(i);
        if (myRenderer == NULL) {
            continue;
        }
        if (myRenderer->GetVisible() == false) {
            continue;
        }

        if (myRenderer->GetLayer()->HasData() == true) {
            bHasRealCoordinates = true;
            break;
        }
    }

    // really compute extent
    vrRealRect myLayerExtent;
    for (unsigned int i = 0; i < m_renderers.GetCount(); i++) {
        vrRenderer *myRenderer = m_renderers.Item(i);
        if (myRenderer == NULL) {
            continue;
        }

        if (myRenderer->GetLayer()->GetExtent(myLayerExtent) == false) {
            continue;
        }

        if (myRenderer->GetVisible() == false && onlyvisible == true) {
            continue;
        }

        if (bHasRealCoordinates == true && myRenderer->GetLayer()->HasData() == false) {
            continue;
        }
        myCoordinate->AddLayersExtent(myLayerExtent);
    }

    if (myCoordinate->ComputeFullExtent() == false) {
        return false;
    }

    /*myLayerExtent = myCoordinate->GetExtent();
    wxLogMessage("Windows extent is :\n left : %.3f\nright : %.3f\ntop : %.3f\nbottom : %.3f",
                 myLayerExtent.GetLeft(),
                 myLayerExtent.GetRight(),
                 myLayerExtent.GetTop(),
                 myLayerExtent.GetBottom());*/

    return true;
}


wxBitmap *vrViewerLayerManager::_MergeBitmapData()
{
    if (m_images.GetCount() == 0) {
        return NULL;
    }

    wxSize mySize = m_images.Item(0)->GetSize();
    wxASSERT(mySize != wxDefaultSize);


    wxBitmap *myAggregatedBmp = new wxBitmap(mySize);
    wxMemoryDC myDC(*myAggregatedBmp);

    // paint the background
    wxASSERT(m_display);
    myDC.SetBrush(m_display->GetBackgroundColour());
    myDC.SetPen(m_display->GetBackgroundColour());
    myDC.Clear();
    myDC.DrawRectangle(0, 0, mySize.GetWidth(), mySize.GetHeight());

    wxStopWatch sw;
    for (unsigned int i = 0; i < m_images.GetCount(); i++) {
        wxBitmap myBmp(*m_images.Item(i));
        myDC.DrawBitmap(myBmp, 0, 0, true);
    }

    //wxLogMessage("Merging bitmaps with mask took : %ldms", sw.Time());

    myDC.SelectObject(wxNullBitmap);
    return myAggregatedBmp;

}


int vrViewerLayerManager::_ReloadThread(long &vectorcount)
{
    _BitmapArrayInit();

    _GetLayersData(vectorcount);
    wxBitmap *myFinalBmp = _MergeBitmapData();

    int myLayersNumber = m_images.GetCount();
    wxSize myBmpSize = wxDefaultSize;

    if (myLayersNumber > 0) {
        myBmpSize = m_images.Item(0)->GetSize();
    }

    _BitmapArrayDelete();

    // pass bitmap to dispaly
    wxASSERT(m_display);
    m_display->SetBitmap(myFinalBmp);

    wxDELETE(myFinalBmp);
    return myLayersNumber;
}


int vrViewerLayerManager::_Reload(long &vectorcount, long &drawnvertex, long &skippedvertex)
{
    // gettting display coordinates
    wxASSERT(m_display);
    vrCoordinate *myCoordinate = m_display->GetCoordinate();
    wxASSERT(myCoordinate);

    wxBitmap *myBmp = new wxBitmap(m_display->GetSize());
    wxMemoryDC dc(*myBmp);
    dc.SetBackground(wxBrush(m_display->GetBackgroundColour()));
    dc.Clear();
    dc.SelectObject(wxNullBitmap);

    vectorcount = 0;
    // getting data from vrRenderer -> vrLayer
    bool bIsAtLeastOneVisible = false;
    int iShownLayer = 0;
    int iTotLayers = m_renderers.GetCount();
    for (int i = iTotLayers - 1; i >= 0; i--) {
        if (m_renderers.Item(i)->GetVisible() == true) {
            long myVectorCount = 0;
            long myVertexDrawn = 0;
            long myVertexSkipped = 0;
            if (m_renderers.Item(i)->GetBitmapData(myBmp, myCoordinate->GetExtent(), myCoordinate->GetPixelSize(),
                                                   myVectorCount, myVertexDrawn, myVertexSkipped) == false) {
                wxLogMessage("No data to display for '%s' !",
                             m_renderers.Item(i)->GetLayer()->GetDisplayName().GetFullName());
            } else {
                iShownLayer++;
            }
            vectorcount = vectorcount + myVectorCount;
            drawnvertex = drawnvertex + myVertexDrawn;
            skippedvertex = skippedvertex + myVertexSkipped;
            bIsAtLeastOneVisible = true;
        }
    }

    // no visible raster!
    if (bIsAtLeastOneVisible == false) {
        m_display->SetBitmap(NULL);
        wxDELETE(myBmp);
        return 0;
    }

    wxASSERT(m_display);
    m_display->SetBitmap(myBmp);
    wxDELETE(myBmp);
    return iShownLayer;
}


void vrViewerLayerManager::OnReload(wxCommandEvent &event)
{
    Reload();
    event.Skip();
}


#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(wxArrayImage);

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(vrArrayViewerLayerManager);






