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
		m_WindowParent->PopEventHandler(false);
		//m_WindowParent->SetEventHandler(m_WindowParent);
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



bool vrViewerLayerManager::Add(long pos, vrLayer * layer, vrRender * render, vrLabel * label) {
	wxASSERT(layer);
	vrRenderer * myRenderer = new vrRenderer(layer, render, label);
	
	// need to compute coordinate ?
	if (m_Renderers.GetCount() == 0) {
		m_ComputeExtentStatus = true;
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
				_GetLayersExtent();
				m_ComputeExtentStatus = false;
			}
			
			wxCommandEvent myEvt(vrEVT_VLM_RELOAD);
			m_WindowParent->ProcessWindowEvent(myEvt);
		}
	}
	
	
	return true;
}




bool vrViewerLayerManager::Remove(const wxFileName & filename) {
	return false;
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
		_GetLayersExtent();
		m_ComputeExtentStatus = false;
	}
	
	
	// reloading data
	wxCommandEvent myEvt(vrEVT_VLM_RELOAD);
	m_WindowParent->ProcessWindowEvent(myEvt);

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
	wxLogMessage("Created %d images", m_Images->GetCount());
	return true;
}



void vrViewerLayerManager::_BitmapArrayDelete() {
	if (m_Images) {
		wxLogMessage("Deleting %d images", m_Images->GetCount());
		
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
												   myCoordinate->GetExtent())==false) {
				wxLogError("Getting data for %s failed !",
						   m_Renderers.Item(i)->GetLayer()->GetName().GetFullName());
				bReturn = false;
			}
			iImageIndex++;
		}
	}
	
	return bReturn;
}


bool vrViewerLayerManager::_GetLayersExtent() {
		
	vrCoordinate * myCoordinate = m_Display->GetCoordinate();
	wxASSERT(myCoordinate);
	
	// clear actual layer extent
	myCoordinate->ClearLayersExtent();
	
	wxRect2DDouble myLayerExtent;
	for (unsigned int i = 0; i< m_Renderers.GetCount(); i++) {
		if (m_Renderers.Item(i)->GetLayer()->GetExtent(myLayerExtent)==true) {
			myCoordinate->AddLayersExtent(myLayerExtent);
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
	
	// paint to white the background
	myDC.SetBrush(*wxGREY_BRUSH);
	myDC.SetPen(*wxGREY_PEN);
	myDC.Clear();
	myDC.DrawRectangle(0,0,mySize.GetWidth(), mySize.GetHeight());
	

	
	/*wxImage myImgOne ("/Users/lucien/Documents/PRJ/COLTOPGIS/test_data/one.png", wxBITMAP_TYPE_PNG);
	wxImage myImgTwo ("/Users/lucien/Documents/PRJ/COLTOPGIS/test_data/two.png", wxBITMAP_TYPE_PNG);
	
	myImgOne.SetMaskColour(0, 0, 0);

	//myImgTwo.SetMaskColour(0, 0, 0);
	
	
	
	bool bVal = myImgTwo.HasAlpha();
	if (bVal == false) {
		myImgTwo.InitAlpha();
		bVal = true;
	}
	if (bVal) {
		// FIXME: Add transparency should go somewhere else
		unsigned char * alphachar = NULL;
		 int translucencypercent = 100;
		 unsigned int myimglen = mySize.GetWidth() * mySize.GetHeight();
		 alphachar= (unsigned char*)CPLMalloc(myimglen);
		 if (alphachar == NULL)
		 {
		 wxLogError(_T("Error creating translucency"));
		 return false;
		 }
		 
		 // convert percent to 0-255 and invert 
		 int myTransValue = translucencypercent * 255 / 100;
		 myTransValue = 255 - myTransValue;
		 
		 
		 for (unsigned int i = 0; i < myimglen;i ++)
		 {
		 *(alphachar + i) =  (char) myTransValue;
		 }
		 myImgTwo.SetAlpha(alphachar);
	}
	
	
	for (int x = 0; x<50; x++) {
		for (int y = 0; y < 50; y++) {
			myImgTwo.SetAlpha(x, y, 120);
		}
	}
	
	
	wxBitmap myOne (myImgOne);
	wxBitmap myTwo (myImgTwo);

	myDC.DrawBitmap(myOne,0,0,true);
	myDC.DrawBitmap(myTwo,0,0,true);*/


	for (int i = 0 ; i < m_Images->GetCount(); i++ ) {
		wxBitmap myBmp (*m_Images->Item(i));
		myDC.DrawBitmap(myBmp,0,0,true);
	}

	myDC.SelectObject(wxNullBitmap);
	return myAggregatedBmp;	
	
}



void vrViewerLayerManager::OnReload(wxCommandEvent & event) {
	
	_BitmapArrayInit();
	
	_GetLayersData();
	wxBitmap * myFinalBmp = _MergeBitmapData();
	_BitmapArrayDelete();
	
	// pass bitmap to dispaly
	wxASSERT(m_Display);
	m_Display->SetBitmap(myFinalBmp);
	
	wxDELETE(myFinalBmp);
	event.Skip();
}


#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(wxArrayImage);

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(vrArrayViewerLayerManager);






