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
#include "vrviewertoc.h"
#include "vrviewerdisplay.h"
#include "vrcoordinate.h"


wxDEFINE_EVENT(vrEVT_VLM_RELOAD, wxCommandEvent);
wxDEFINE_EVENT(vrEVT_TOOL_ZOOM, wxCommandEvent);
wxDEFINE_EVENT(vrEVT_TOOL_SELECT, wxCommandEvent);
wxDEFINE_EVENT(vrEVT_TOOL_PAN, wxCommandEvent);
wxDEFINE_EVENT(vrEVT_TOOL_SIGHT, wxCommandEvent);


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
	m_ComputeExtentStatus = false;
	m_Images = NULL;
	
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
	wxLogMessage("ViewerLayerManager Dtor called");
	
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
		m_Renderers.Detach(0);
		wxDELETE(myRenderer);
	}
	
}



bool vrViewerLayerManager::Add(long pos, vrLayer * layer, vrRender * render, vrLabel * label, bool visible) {
	wxASSERT(layer);
	vrRenderer * myRenderer = new vrRenderer(layer, render, label);
	myRenderer->SetVisible(visible);
	
	// need to compute coordinate ?
	if (m_Renderers.GetCount() == 0) {
		m_ComputeExtentStatus = true;
		wxLogMessage("Computing status is set to TRUE");
	}
	
	
	if (pos == -1){
		m_Renderers.Insert(myRenderer, 0);
	}
	else {
		m_Renderers.Insert(myRenderer, pos);
	}
	
	if (m_Toc) {
		m_Toc->Add(pos, myRenderer, m_Renderers.GetCount());
	}
	
	// if not freezed, refresh imediatelly.
	if (m_FreezeStatus==false) {
		if(m_WindowParent){
			if (m_ComputeExtentStatus == true) {
				_GetLayersExtent(false);
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
	m_Renderers.Detach(oldpos);
	m_Renderers.Insert(myRenderer, myNewPos);
	
	
	if(m_Toc && m_Toc->Move(oldpos, newpos)==false){
		wxLogError("Moving layer '%s' from position %d to %d failed",
				   myRenderer->GetLayer()->GetName().GetFullName(),
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
	
	
	// remove from Renderer array
	vrRenderer * myRenderer = GetRenderer(myRemoveIndex);
	wxASSERT(myRenderer);
	m_Renderers.Detach(myRemoveIndex);
	wxDELETE(myRenderer);
	
	// remove from TOC
	if (m_Toc) {
		m_Toc->Remove(myRemoveIndex);
	}
	
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
		_GetLayersExtent(false);
		m_ComputeExtentStatus = false;
	}
	
	
	// reloading data
	Reload();

}


void vrViewerLayerManager::Reload() {
	_BitmapArrayInit();
	
	_GetLayersData();
	wxBitmap * myFinalBmp = _MergeBitmapData();
	_BitmapArrayDelete();
	
	// pass bitmap to dispaly
	wxASSERT(m_Display);
	m_Display->SetBitmap(myFinalBmp);
	
	wxDELETE(myFinalBmp);
}


bool vrViewerLayerManager::_BitmapArrayInit() {
	wxASSERT(m_Images == NULL);
	if (m_Renderers.GetCount()== 0) {
		//wxLogError("Unable to create images, no layer opened");
		return false;
	}
	
	m_Images = new wxArrayImage();
	wxSize myDisplaySize = m_Display->GetSize();
	
	for (unsigned int i = 0; i<m_Renderers.GetCount(); i++) {
		
		// create image only for visible layers
		if (m_Renderers.Item(i)->GetVisible()) {
			wxImage * myImage = new wxImage(myDisplaySize);
			/*if (myImage->HasAlpha() == false) {
				myImage->InitAlpha();
			}*/
			
			wxLogMessage("Creation des images : %p", myImage);
			
			m_Images->Add(myImage);
			
		}
	}
	wxLogMessage("Created %ld images", m_Images->GetCount());
	return true;
}



void vrViewerLayerManager::_BitmapArrayDelete() {
	if (m_Images) {
		wxLogMessage("Deleting %ld images", m_Images->GetCount());
		
		unsigned int iCountImg = m_Images->GetCount();
		for (unsigned j = 0; j<iCountImg; j++){
			wxImage * myImage = m_Images->Item(0);
			wxASSERT(myImage);
			m_Images->Detach(0);
			wxDELETE(myImage);
		}
		wxASSERT(m_Images->GetCount()==0);
		wxDELETE(m_Images);
	}
}



bool vrViewerLayerManager::_GetLayersData() {
	
	// gettting display coordinates
	wxASSERT(m_Display);
	vrCoordinate * myCoordinate = m_Display->GetCoordinate();
	wxASSERT(myCoordinate);
	
	// getting data from vrRenderer -> vrLayer
	bool bReturn = true;
	int iImageIndex = 0;
	if(m_Images==NULL)
	{
		return false;
	}
	
	int iTotLayers = m_Renderers.GetCount();
	for (int i = iTotLayers-1; i>= 0; i--) {
		if (m_Renderers.Item(i)->GetVisible() == true) {
			if (m_Renderers.Item(i)->GetBitmapData( m_Images->Item(iImageIndex),
												   myCoordinate->GetExtent(),
												   myCoordinate->GetPixelSize())==false) {
				wxLogError("Getting data for %s failed !",
						   m_Renderers.Item(i)->GetLayer()->GetName().GetFullName());
				bReturn = false;
			}
			iImageIndex++;
		}
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
	
	// TODO: Remove Temp logging code
	myLayerExtent = myCoordinate->GetExtent();
	wxLogMessage("Windows extent is :\n left : %.3f\nright : %.3f\ntop : %.3f\nbottom : %.3f",
				 myLayerExtent.GetLeft(),
				 myLayerExtent.GetRight(),
				 myLayerExtent.GetTop(),
				 myLayerExtent.GetBottom());
	
	return true;
}



wxBitmap * vrViewerLayerManager::_MergeBitmapData() {
	
	if (m_Images == NULL){
		return NULL;
	}
	
	if (m_Images->GetCount() == 0) {
		return NULL;
	}
	
	wxSize mySize = m_Images->Item(0)->GetSize();
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
	for (unsigned int i = 0 ; i < m_Images->GetCount(); i++ ) {
		wxBitmap myBmp (*m_Images->Item(i));
		myDC.DrawBitmap(myBmp,0,0,true);
	}

	wxLogMessage("Merging bitmaps with mask took : %ldms", sw.Time());
	
	myDC.SelectObject(wxNullBitmap);
	return myAggregatedBmp;	
	
}



void vrViewerLayerManager::OnReload(wxCommandEvent & event) {
	Reload();
	event.Skip();
}


#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(wxArrayImage);

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(vrArrayViewerLayerManager);






