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
	EVT_COMMAND(wxID_ANY, vrEVT_VLM_RELOAD, vrViewerLayerManager::OnReload)
END_EVENT_TABLE()


vrViewerLayerManager::vrViewerLayerManager(vrLayerManager * parent, wxWindow * window,
										   vrViewerDisplay * viewer,vrViewerTOC * toc) {

	//wxASSERT(viewer);
	wxASSERT(parent);
	m_WindowParent = NULL;
	m_Display = viewer;
	m_Toc = toc;
	m_LayerManager = parent;
	m_FreezeStatus = false;
	m_UserDefinedExtent = false;
	m_ComputeExtentStatus = false;
	m_PerfMonitorFile.Clear();
	m_ReloadThread = false;
	if (window) {
		m_WindowParent = window;
		m_WindowParent->PushEventHandler(this);
	}

	if (m_Display==NULL){
		wxLogError("No display attached");
	}
	else {
		m_Display->SetViewerLayerManager(this);
	}



	if (m_Toc == NULL){
		wxLogError("No toc defined");
	}
	else {
		m_Toc->SetViewerLayerManager(this);
	}

	// add this viewer layer manager to the vrLayerManager
	m_LayerManager->AddViewerLayerManager(this);
}



vrViewerLayerManager::~vrViewerLayerManager() {
	//wxLogMessage("ViewerLayerManager Dtor called");

	if (m_WindowParent) {
		m_WindowParent->RemoveEventHandler(this);
	}


	wxDELETE(m_Display);
	wxDELETE(m_Toc);


	// delete renderers
	unsigned int iCount = m_Renderers.GetCount();
	for (unsigned int i = 0; i<iCount; i++) {
		vrRenderer * myRenderer = m_Renderers.Item(0);
		wxASSERT(myRenderer);
		wxDELETE(myRenderer);
		m_Renderers.RemoveAt(0);
	}

}

void vrViewerLayerManager::InitializeExtent(const vrRealRect & extent)
{
    // Check input
    vrRealRect myExtent(extent);
    if (myExtent.m_height>0){
        myExtent.m_height = -myExtent.m_height;
    }

    m_UserDefinedExtent = true;
    m_ComputeExtentStatus = false;

    vrCoordinate * coord = m_Display->GetCoordinate();
	wxASSERT(coord);

	coord->ClearLayersExtent();
	coord->ClearPixelSize();
	coord->AddLayersExtent(myExtent);

	if ( coord->ComputeFullExtent()==false){
        wxLogError(_("Couldn't apply the desired extent."));
        m_UserDefinedExtent = false;
        m_ComputeExtentStatus = true;
		return;
	}
}



bool vrViewerLayerManager::Add(long pos, vrLayer * layer, vrRender * render, vrLabel * label, bool visible) {
	wxASSERT(layer);
	vrRenderer * myRenderer = new vrRenderer(layer, render, label);
	myRenderer->SetVisible(visible);


	// compute coordinate if layer added is visible and
	// there is no other visible layers
	if (visible == true && m_ComputeExtentStatus == false && m_UserDefinedExtent == false) {
		m_ComputeExtentStatus = true;
		for (unsigned int i = 0; i<m_Renderers.GetCount(); i++) {
			if (m_Renderers.Item(i)->GetVisible() == true) {
				m_ComputeExtentStatus = false;
				break;
			}
		}
	}

	if (m_ComputeExtentStatus == true){
		//wxLogMessage("Computing extent required");
	}


	if (pos >= (signed) m_Renderers.GetCount()) {
		pos = m_Renderers.GetCount();
	}

	if (pos == -1){
		m_Renderers.Insert(myRenderer, 0);
	}
	else {
		m_Renderers.Insert(myRenderer, pos);
	}

	if (m_Toc) {
		m_Toc->Add(pos, myRenderer);
	}

	// if not freezed, refresh imediatelly.
	if (m_FreezeStatus==false) {
		if(m_WindowParent){
			if (m_ComputeExtentStatus == true) {
				_GetLayersExtent(true);
				m_ComputeExtentStatus = false;
			}

			Reload();
		}
	}


	return true;
}



bool vrViewerLayerManager::Move(long oldpos, long newpos) {
	wxASSERT(oldpos >= 0 && newpos >= 0);
	wxASSERT(oldpos < (signed) m_Renderers.GetCount() && newpos < (signed) m_Renderers.GetCount());
	wxASSERT(oldpos != newpos);

	int myNewPos = newpos;
	if (newpos > oldpos + 1) {
		myNewPos = myNewPos -1;
	}

	vrRenderer * myRenderer = m_Renderers.Item(oldpos);
	wxASSERT(myRenderer);
	m_Renderers.RemoveAt(oldpos);
	m_Renderers.Insert(myRenderer, myNewPos);


	if(m_Toc && m_Toc->Move(oldpos, newpos)==false){
		wxLogError("Moving layer '%s' from position %ld to %ld failed",
				   myRenderer->GetLayer()->GetDisplayName().GetFullName(),
				   oldpos, newpos);
		return false;
	}

	if (m_FreezeStatus == false && m_WindowParent) {
		Reload();
	}

	return true;
}



bool vrViewerLayerManager::Remove(vrRenderer * renderer) {
	wxASSERT(renderer);

	int myRemoveIndex = wxNOT_FOUND;
	for (int i = 0; i< GetCount(); i++) {
		vrRenderer * myRenderer = GetRenderer(i);
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
	if (m_Toc) {
		m_Toc->Remove(myRemoveIndex);
	}

	// remove from Renderer array
	vrRenderer * myRenderer = GetRenderer(myRemoveIndex);
	wxASSERT(myRenderer);
	wxDELETE(myRenderer);
	m_Renderers.RemoveAt(myRemoveIndex);

	// if not freezed, refresh imediatelly.
	if (m_FreezeStatus==false) {
		if(m_WindowParent){
			Reload();
		}
	}
	return true;
}



bool vrViewerLayerManager::ZoomOut(const vrRealRect & extent) {
	if (extent.IsOk() == false) {
		wxLogError("Specified extent (%f, %f, %f, %f) isn't supported",
				   extent.GetLeft(), extent.GetTop(),
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


bool vrViewerLayerManager::Zoom(const vrRealRect & extent) {
	if (extent.IsOk() == false) {
		wxLogError("Specified extent (%f, %f, %f, %f) isn't supported",
				   extent.GetLeft(), extent.GetTop(),
				   extent.GetRight(), extent.GetBottom());
		return false;
	}

	wxASSERT(m_Display);

	// create fitted rectangle
	vrCoordinate * myCoord = m_Display->GetCoordinate();
	wxASSERT(myCoord);
	vrRealRect myFittedRect = myCoord->GetRectFitted(extent);
	if (myFittedRect.IsOk() == false) {
		wxLogError("Wasn't able to zoom");
		return false;
	}

	myCoord->SetExtent(myFittedRect);

	// update drawing
	if(m_WindowParent){
		Reload();
	}
	return true;
}


void vrViewerLayerManager::ZoomToFit(bool onlyvisible) {
	_GetLayersExtent(onlyvisible);
}



long vrViewerLayerManager::Select(const wxRect & extent, vrRenderer * selectlayer) {
    // make selection on this layer if not null, otherwise use selection from TOC
    vrRenderer * mySelectLayer = selectlayer;
    if (selectlayer == NULL) {
        int myIndex = m_Toc->GetSelection();
        if (myIndex == wxNOT_FOUND) {
            return wxNOT_FOUND;
        }
        mySelectLayer = m_Renderers.Item(myIndex);
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
	m_Display->GetCoordinate()->ConvertFromPixels(myPxExtent, myRealCoord);

	vrLayerVector * myLayer = static_cast<vrLayerVector *>(mySelectLayer->GetLayer());
	wxASSERT(myLayer);
	return myLayer->Select(myRealCoord);
}



void vrViewerLayerManager::ClearSelection() {
	for (int i = 0; i< GetCount(); i++) {
		if(m_Renderers.Item(i)->GetLayer()->GetType() >= vrDRIVER_VECTOR_SHP &&
		   m_Renderers.Item(i)->GetLayer()->GetType() <= vrDRIVER_VECTOR_MEMORY){
			vrLayerVector * myLayer = (vrLayerVector*) m_Renderers.Item(i)->GetLayer();
			wxASSERT(myLayer);
			myLayer->GetSelectedIDs()->Clear();
		}
	}
}


int vrViewerLayerManager::GetSelectionCount(int * sellayers) {
	int iTotal = 0;
	for (int i = 0; i< GetCount(); i++) {
		if(m_Renderers.Item(i)->GetLayer()->GetType() >= vrDRIVER_VECTOR_SHP &&
		   m_Renderers.Item(i)->GetLayer()->GetType() <= vrDRIVER_VECTOR_MEMORY){
			vrLayerVector * myLayer = (vrLayerVector*) m_Renderers.Item(i)->GetLayer();
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



vrRenderer * vrViewerLayerManager::GetRenderer(const unsigned int & index) {
	// check index into limits
	if (index > m_Renderers.GetCount()-1) {
		wxLogError("Trying to Get Renderer out of bounds");
		return NULL;
	}

	vrRenderer * renderer = m_Renderers.Item(index);
	wxASSERT(renderer);
	return renderer;
}



vrRenderer * vrViewerLayerManager::GetRenderer(const wxString & layername) {
    for (unsigned int i = 0; i< m_Renderers.GetCount(); i++) {
        if (m_Renderers.Item(i)->GetLayer()->GetFileName().GetFullPath() == layername){
            return m_Renderers.Item(i);
        }
    }
    return NULL;
}


// number of renderers
int vrViewerLayerManager::GetCount() {
	return m_Renderers.GetCount();
}



void vrViewerLayerManager::FreezeBegin() {
	wxASSERT(m_Toc);
	wxASSERT(m_FreezeStatus==false);
	m_FreezeStatus = true;
	m_Toc->FreezeBegin();
}

void vrViewerLayerManager::FreezeEnd() {
	wxASSERT(m_Toc);
	wxASSERT(m_FreezeStatus==true);
	m_FreezeStatus = false;
	m_Toc->FreezeEnd();

	// compute layers extent
	if (m_ComputeExtentStatus == true) {
		_GetLayersExtent(true);
		m_ComputeExtentStatus = false;
	}


	// reloading data
	Reload();

}


void vrViewerLayerManager::Reload() {
	vrPerformance * myPerf = NULL;
	if (m_PerfMonitorFile.IsOk() == true) {
		myPerf = new vrPerformance(m_PerfMonitorFile.GetFullPath(),
								   "Number of layers\tTotal Vector Objects\tDrawn Vertex\tSkipped Vertex\tWindow Resolution(x)\tWindow Resolution(y)\t");
	}

	int myCountLayer = 0;
	long myVectorCount = 0;
    long myVertexDrawn = 0;
    long myVertexSkipped = 0;
	if (m_ReloadThread == true) {
		myCountLayer = _ReloadThread(myVectorCount);
	}
	else {
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
		myPerf->StopWork(wxString::Format("%d\t%ld\t%ld\t%ld\t%d\t%d;",
										  myCountLayer,
										  myVectorCount,
                                          myVertexDrawn,
                                          myVertexSkipped,
										  m_Display->GetSize().GetWidth(),
										  m_Display->GetSize().GetHeight())) ;
	}
}



bool vrViewerLayerManager::StartEdition(vrRenderer * renderer) {
    bool bEditionStarted = false;
    for (unsigned int i = 0; i< m_Renderers.GetCount(); i++) {
        vrRenderer * myRenderer = m_Renderers.Item(i);
        wxASSERT(myRenderer);
        
        if (myRenderer == renderer){
            myRenderer->SetInEdition(true);
            bEditionStarted = true;
            
            if (m_Toc) {
                m_Toc->SetEditStyle(i);
            }
        }
    }
    return bEditionStarted;
}



void vrViewerLayerManager::StopEdition() {
    for (unsigned int i = 0; i< m_Renderers.GetCount(); i++) {
        vrRenderer * myRenderer = m_Renderers.Item(i);
        wxASSERT(myRenderer);
        myRenderer->SetInEdition(false);
        
        if (m_Toc) {
            m_Toc->SetNormalStyle(i);
        }
    }
}



vrRenderer * vrViewerLayerManager::GetEditionRenderer() {
    for (unsigned int i = 0; i< m_Renderers.GetCount(); i++) {
        vrRenderer * myRenderer = m_Renderers.Item(i);
        wxASSERT(myRenderer);
        if (myRenderer->IsInEdition() == true) {
            return myRenderer;
        }
    }
    return NULL;
}



void vrViewerLayerManager::StartPerfMonitor(const wxFileName & filename) {
	m_PerfMonitorFile = filename;
}



void vrViewerLayerManager::StopPerfMonitor() {
	m_PerfMonitorFile.Clear();
}


void vrViewerLayerManager::SetEngineThreaded(bool enable) {
#ifdef __LINUX__
	if (enable == true) {
		if (wxMessageBox(_("Engine not fully compatible with Linux, vector layers will not be rendered! Continue ?"),
						   _("Linux Problem"), wxYES_NO) == wxNO) {
			return;
		}
	}
#endif
	m_ReloadThread = enable;
}


bool vrViewerLayerManager::_BitmapArrayInit() {
	wxASSERT(m_Images.GetCount() == 0);
	if (m_Renderers.GetCount()== 0) {
		//wxLogError("Unable to create images, no layer opened");
		return false;
	}

	wxSize myDisplaySize = m_Display->GetSize();

	for (unsigned int i = 0; i<m_Renderers.GetCount(); i++) {

		// create image only for visible layers
		if (m_Renderers.Item(i)->GetVisible()) {
			wxImage * myImage = new wxImage(myDisplaySize);
			/*if (myImage->HasAlpha() == false) {
				myImage->InitAlpha();
			}*/

			//wxLogMessage("Creation des images : %p", myImage);

			m_Images.Add(myImage, 1);

		}
	}
	//wxLogMessage("Created %ld images", m_Images.GetCount());
	return true;
}



void vrViewerLayerManager::_BitmapArrayDelete() {
	if (m_Images.GetCount() > 0) {
		//wxLogMessage("Deleting %ld images", m_Images.GetCount());

		unsigned int iCountImg = m_Images.GetCount();
		for (unsigned j = 0; j<iCountImg; j++){
			wxImage * myImage = m_Images.Item(0);
			wxASSERT(myImage);
			wxDELETE(myImage);
			m_Images.RemoveAt(0);
		}
		wxASSERT(m_Images.GetCount()==0);
	}
}



bool vrViewerLayerManager::_GetLayersData(long & vectorcount) {

	// gettting display coordinates
	wxASSERT(m_Display);
	vrCoordinate * myCoordinate = m_Display->GetCoordinate();
	wxASSERT(myCoordinate);

	vectorcount = 0;
	// getting data from vrRenderer -> vrLayer
	bool bReturn = true;
	int iImageIndex = 0;
	if(m_Images.GetCount() == 0)
	{
		return false;
	}

	wxArrayInt myInvalidRaster;
	int iTotLayers = m_Renderers.GetCount();
	for (int i = iTotLayers-1; i>= 0; i--) {
		if (m_Renderers.Item(i)->GetVisible() == true) {
			long myVectorCount = 0;
			if (m_Renderers.Item(i)->GetBitmapDataThread( m_Images.Item(iImageIndex),
														 myCoordinate->GetExtent(),
														 myCoordinate->GetPixelSize(),
														 myVectorCount)==false) {
				wxLogMessage("No data to display for '%s' !",
							 m_Renderers.Item(i)->GetLayer()->GetDisplayName().GetFullName());
				bReturn = false;
				myInvalidRaster.Add(iImageIndex);
			}
			vectorcount = vectorcount + myVectorCount;
			iImageIndex++;
		}
	}

	// remove invalid raster
	for (int i = myInvalidRaster.GetCount() -1; i >= 0;i--){
        wxImage * myImage = m_Images.Item(myInvalidRaster.Item(i));
        wxASSERT(myImage);
        wxDELETE(myImage);
        m_Images.RemoveAt(myInvalidRaster.Item(i));
        //wxLogMessage("Removing image at index : %d", i);
	}
	return bReturn;
}


bool vrViewerLayerManager::_GetLayersExtent(bool onlyvisible) {

	vrCoordinate * myCoordinate = m_Display->GetCoordinate();
	wxASSERT(myCoordinate);

	// clear actual layer extent
	myCoordinate->ClearLayersExtent();
	myCoordinate->ClearPixelSize();

	vrRealRect myLayerExtent;
	for (unsigned int i = 0; i< m_Renderers.GetCount(); i++) {
		if (m_Renderers.Item(i)->GetLayer()->GetExtent(myLayerExtent)==true) {

			if (onlyvisible == true && m_Renderers.Item(i)->GetVisible() == false){
			}
			else {
				myCoordinate->AddLayersExtent(myLayerExtent);
			}
		}
	}

	if ( myCoordinate->ComputeFullExtent()==false){
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



wxBitmap * vrViewerLayerManager::_MergeBitmapData() {
	if (m_Images.GetCount() == 0) {
		return NULL;
	}

	wxSize mySize = m_Images.Item(0)->GetSize();
	wxASSERT(mySize != wxDefaultSize);


	wxBitmap * myAggregatedBmp = new wxBitmap(mySize);
	wxMemoryDC myDC(*myAggregatedBmp);

	// paint the background
	wxASSERT(m_Display);
	myDC.SetBrush(m_Display->GetBackgroundColour());
	myDC.SetPen(m_Display->GetBackgroundColour());
	myDC.Clear();
	myDC.DrawRectangle(0,0,mySize.GetWidth(), mySize.GetHeight());

	wxStopWatch sw;
	for (unsigned int i = 0 ; i < m_Images.GetCount(); i++ ) {
		wxBitmap myBmp (*m_Images.Item(i));
		myDC.DrawBitmap(myBmp,0,0,true);
	}

	//wxLogMessage("Merging bitmaps with mask took : %ldms", sw.Time());

	myDC.SelectObject(wxNullBitmap);
	return myAggregatedBmp;

}



int vrViewerLayerManager::_ReloadThread(long & vectorcount) {
	_BitmapArrayInit();

	_GetLayersData(vectorcount);
	wxBitmap * myFinalBmp = _MergeBitmapData();

	int myLayersNumber = m_Images.GetCount();
	wxSize myBmpSize = wxDefaultSize;

	if(myLayersNumber > 0){
		myBmpSize = m_Images.Item(0)->GetSize();
	}

	_BitmapArrayDelete();

	// pass bitmap to dispaly
	wxASSERT(m_Display);
	m_Display->SetBitmap(myFinalBmp);

	wxDELETE(myFinalBmp);
	return myLayersNumber;
}



int vrViewerLayerManager::_Reload(long & vectorcount, long & drawnvertex, long & skippedvertex) {
	// gettting display coordinates
	wxASSERT(m_Display);
	vrCoordinate * myCoordinate = m_Display->GetCoordinate();
	wxASSERT(myCoordinate);

	wxBitmap * myBmp = new wxBitmap(m_Display->GetSize());
	wxMemoryDC dc (*myBmp);
	dc.SetBackground(wxBrush(m_Display->GetBackgroundColour()));
	dc.Clear();
	dc.SelectObject(wxNullBitmap);

	vectorcount = 0;
	// getting data from vrRenderer -> vrLayer
	bool bIsAtLeastOneVisible = false;
	int iShownLayer = 0;
	int iTotLayers = m_Renderers.GetCount();
	for (int i = iTotLayers-1; i>= 0; i--) {
		if (m_Renderers.Item(i)->GetVisible() == true) {
			long myVectorCount = 0;
            long myVertexDrawn = 0;
            long myVertexSkipped = 0;
			if (m_Renderers.Item(i)->GetBitmapData(myBmp,
												   myCoordinate->GetExtent(),
												   myCoordinate->GetPixelSize(),
												   myVectorCount, myVertexDrawn, myVertexSkipped)==false) {
				wxLogMessage("No data to display for '%s' !",
							 m_Renderers.Item(i)->GetLayer()->GetDisplayName().GetFullName());
			}
			else {
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
		m_Display->SetBitmap(NULL);
		wxDELETE(myBmp);
		return 0;
	}

	wxASSERT(m_Display);
	m_Display->SetBitmap(myBmp);
	wxDELETE(myBmp);
	return iShownLayer;
}





void vrViewerLayerManager::OnReload(wxCommandEvent & event) {
	Reload();
	event.Skip();
}


#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(wxArrayImage);

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(vrArrayViewerLayerManager);






