/***************************************************************************
				vrviewertoc.cpp

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


#include "vrviewertoc.h"
#include "vrevent.h"
#include "vrviewerlayermanager.h"
#include "vrlayer.h"
#include "vrrender.h"
#include "vrrendervectorc2p.h"

#include <wx/colordlg.h>	// colour dialog
#include <wx/numdlg.h>		// number entry dialog




BEGIN_EVENT_TABLE(vrViewerTOC, wxCheckListBox)
	EVT_CHECKLISTBOX(wxID_ANY, vrViewerTOC::OnVisibleStatusChanged)
	EVT_RIGHT_DOWN(vrViewerTOC::OnMouseRightDown)
	EVT_MOUSEWHEEL(vrViewerTOC::OnMouseWheel)
	// POPUP EVENT
	EVT_MENU(vrID_POPUP_PEN_COLOR, vrViewerTOC::OnSetColorPen)
	EVT_MENU(vrID_POPUP_BRUSH_COLOR, vrViewerTOC::OnSetColorBrush)
	EVT_MENU(vrID_POPUP_TRANSPARENCY, vrViewerTOC::OnSetTransparency)
	EVT_MENU(vrID_POPUP_DRAWING_WIDTH, vrViewerTOC::OnSetWidth)
	EVT_MENU_RANGE(vrID_POPUP_BRUSH_SOLID, vrID_POPUP_BRUSH_BDIAGONAL, vrViewerTOC::OnSetBrushStyle)

END_EVENT_TABLE()



void vrViewerTOC::OnVisibleStatusChanged(wxCommandEvent & event) {

	//wxLogMessage("Value changed for item : %d value is %d", event.GetInt(), IsChecked(event.GetInt()));

	// change visibility status for layer
	wxASSERT(m_ViewerManager);
	vrRenderer * myItemRenderer = m_ViewerManager->GetRenderer(event.GetInt());
	wxASSERT(myItemRenderer);
	myItemRenderer->SetVisible(IsChecked(event.GetInt()));

	_ReloadData();
}



void vrViewerTOC::OnSetColorPen(wxCommandEvent & event) {
	int mySelItem = GetSelection();
	wxASSERT(mySelItem != wxNOT_FOUND);
	wxASSERT(m_ViewerManager);

	vrRenderVector * myRenderVector = (vrRenderVector*) m_ViewerManager->GetRenderer(mySelItem)->GetRender();
	wxASSERT(myRenderVector);

	// displaying colour dialog
	wxColourData myActualCol;
	myActualCol.SetColour(myRenderVector->GetColorPen());
	wxColourDialog myColDlg (this, &myActualCol);
	if (myColDlg.ShowModal() == wxID_OK) {
		myRenderVector->SetColorPen(myColDlg.GetColourData().GetColour());
		_ReloadData();
	}

}



void vrViewerTOC::OnSetColorBrush(wxCommandEvent & event) {
	int mySelItem = GetSelection();
	wxASSERT(mySelItem != wxNOT_FOUND);
	wxASSERT(m_ViewerManager);

	vrRenderVector * myRenderVector = (vrRenderVector*) m_ViewerManager->GetRenderer(mySelItem)->GetRender();
	wxASSERT(myRenderVector);

	// displaying colour dialog
	wxColourData myActualCol;
	myActualCol.SetColour(myRenderVector->GetColorBrush());
	wxColourDialog myColDlg (this, &myActualCol);
	if (myColDlg.ShowModal() == wxID_OK) {
		myRenderVector->SetColorBrush(myColDlg.GetColourData().GetColour());
		_ReloadData();
	}


}



void vrViewerTOC::OnSetTransparency(wxCommandEvent & event) {
	int mySelItem = GetSelection();
	wxASSERT(mySelItem != wxNOT_FOUND);
	wxASSERT(m_ViewerManager);

	vrRender * myRender = m_ViewerManager->GetRenderer(mySelItem)->GetRender();
	wxASSERT(myRender);

	wxNumberEntryDialog myNumDlg(this,
								 "Adjust the transparency percent\n0 is fully opaque, 100 is fully transparent",
								 "Transparency percent:",
								 "Adjust Opacity",
								 myRender->GetTransparency(),
								 0,
								 100);
	if (myNumDlg.ShowModal()==wxID_OK) {
		myRender->SetTransparency(myNumDlg.GetValue());
		_ReloadData();
	}
}



void vrViewerTOC::OnSetWidth(wxCommandEvent & event) {
	int mySelItem = GetSelection();
	wxASSERT(mySelItem != wxNOT_FOUND);
	wxASSERT(m_ViewerManager);
	vrRender * myRender = m_ViewerManager->GetRenderer(mySelItem)->GetRender();
	wxASSERT(myRender);
	int mySize = 1;
	if (myRender->GetType() == vrRENDER_VECTOR) {
		vrRenderVector * myRenderVector = (vrRenderVector*) myRender;
		mySize = myRenderVector->GetSize();
	}else if (myRender->GetType() == vrRENDER_VECTOR_C2P_DIPS) {
		vrRenderVectorC2PDips * myRenderDips = (vrRenderVectorC2PDips*) myRender;
		mySize = myRenderDips->GetSize();
	}
	else {
		wxFAIL;
		return;
	}


	// get width value
	wxNumberEntryDialog myNumDlg(this,
								 "Adjust the pen's width\nAllowed widths are between 0 and 50 pixels",
								 "Width:",
								 "Adjust pen's width",
								 mySize,
								 1,
								 50);
	if (myNumDlg.ShowModal()!=wxID_OK) {
		return;
	}
	if (myRender->GetType() == vrRENDER_VECTOR) {
		vrRenderVector * myRenderVector = (vrRenderVector*) myRender;
		myRenderVector->SetSize(myNumDlg.GetValue());
	}else if (myRender->GetType() == vrRENDER_VECTOR_C2P_DIPS) {
		vrRenderVectorC2PDips * myRenderDips = (vrRenderVectorC2PDips*) myRender;
		myRenderDips->SetSize(myNumDlg.GetValue());
	}
	else {
		wxFAIL;
		return;
	}
	_ReloadData();
}



void vrViewerTOC::OnSetBrushStyle(wxCommandEvent & event){
	int mySelItem = GetSelection();
	wxASSERT(mySelItem != wxNOT_FOUND);
	wxASSERT(m_ViewerManager);

	vrRenderVector * myRenderVector = (vrRenderVector*) m_ViewerManager->GetRenderer(mySelItem)->GetRender();
	wxASSERT(myRenderVector);

	// displaying colour dialog
	wxBrushStyle myOldStyle = myRenderVector->GetBrushStyle();
	wxBrushStyle myStyle = wxBRUSHSTYLE_SOLID;
	switch (event.GetId()) {
		case vrID_POPUP_BRUSH_SOLID:
			myStyle = wxBRUSHSTYLE_SOLID;
			break;

		case vrID_POPUP_BRUSH_TRANSPARENT:
			myStyle = wxBRUSHSTYLE_TRANSPARENT;
			break;

		case vrID_POPUP_BRUSH_BDIAGONAL:
			myStyle = wxBRUSHSTYLE_BDIAGONAL_HATCH;
			break;

		default:
			wxLogError(_("Brush style not supported: %d"), event.GetId());
			break;
	}
	myRenderVector->SetBrushStyle(myStyle);
	if (myOldStyle != myStyle) {
		_ReloadData();
	}
}



void vrViewerTOC::OnMouseRightDown(wxMouseEvent & event) {
	if (GetCount() == 0) {
		return;
	}

	// hit test
	wxPoint myPos = event.GetPosition();
	int myItemID = HitTest(myPos);
	wxLogMessage("Item seleced id is %d @ position %d - %d", myItemID, myPos.x, myPos.y);

	if (myItemID == wxNOT_FOUND) {
		return;
	}

	SetSelection(myItemID);
	wxASSERT(m_ViewerManager);
	vrRenderer * myRenderer = m_ViewerManager->GetRenderer(myItemID);
	wxASSERT(myRenderer);

	_ShowMenuContextual(myItemID, myRenderer);

}


void vrViewerTOC::OnMouseWheel(wxMouseEvent & event) {
	if (GetCount() == 0) {
		return;
	}

	int myItemSelected = GetSelection();
	if (myItemSelected == wxNOT_FOUND) {
		wxLogWarning("No item selected, select an item (%d)", myItemSelected);
		return;
	}

	// check if not the y axis
	if (event.GetWheelAxis() == 1) {
		return;
	}

	// Not used for now on...
	/*
	int myDelta = event.GetWheelDelta();
	int myRotation = event.GetWheelRotation();
	double myStep = myRotation / myDelta;
	wxLogMessage("Wheel rotation is %d with delta : %d, result is :%f",
				 myRotation,
				 myDelta,
				 myStep);
	*/

	int myRotation = event.GetWheelRotation();
	int myNewPosition = myItemSelected;
	// going up
	if (myRotation > 0) {
		if (myItemSelected == 0) {
			return;
		}
		myNewPosition--;
	}
	// going down
	else if (myRotation < 0) {
		if (myItemSelected == (signed) GetCount() -1) {
			return;
		}
		myNewPosition++;
	}else {
		// myRotation == 0 MBP trackpad...
		return;
	}

	wxASSERT(m_ViewerManager);
	wxLogMessage("Moving item : %d to %d", myItemSelected, myNewPosition);
	m_ViewerManager->Move(myItemSelected, myNewPosition);
}



void vrViewerTOC::_ShowMenuContextual(int id, vrRenderer * renderer) {
	wxASSERT(renderer);
	wxASSERT(id != wxNOT_FOUND);

	wxMenu myPopMenu;
	//myPopMenu.Append(vrID_POPUP_REMOVE, "Remove Layer (not implemented)");
	//myPopMenu.Enable(vrID_POPUP_REMOVE, false);
	//myPopMenu.AppendSeparator();
	myPopMenu.Append(vrID_POPUP_TRANSPARENCY, _("Set Transparency..."));

	switch (renderer->GetRender()->GetType()) {
		case vrRENDER_VECTOR:
			myPopMenu.AppendSeparator();
			myPopMenu.Append(vrID_POPUP_PEN_COLOR, _("Set Pen color..."));
			myPopMenu.Append(vrID_POPUP_DRAWING_WIDTH, _("Set Pen width..."));
			myPopMenu.AppendSeparator();
		{
			wxMenu * myBrushMenu = new wxMenu();
			myBrushMenu->Append(vrID_POPUP_BRUSH_COLOR, _("Set Brush color..."));
			myBrushMenu->AppendSeparator();
			myBrushMenu->AppendRadioItem(vrID_POPUP_BRUSH_SOLID, _("Solid Brush"));
			myBrushMenu->AppendRadioItem(vrID_POPUP_BRUSH_TRANSPARENT, _("Transparent Brush"));
			myBrushMenu->AppendRadioItem(vrID_POPUP_BRUSH_BDIAGONAL, _("Diagonal Brush"));
			myPopMenu.AppendSubMenu(myBrushMenu, _T("Brush"));
		}
			break;

		case vrRENDER_VECTOR_C2P_DIPS:
			myPopMenu.AppendSeparator();
			myPopMenu.Append(vrID_POPUP_DRAWING_WIDTH, _("Set Pen width..."));
			break;


		default:
			break;
	}

	PopupMenu(&myPopMenu);
}


void vrViewerTOC::_ReloadData() {
	// ask for reloading data
	wxASSERT(m_ViewerManager);
	m_ViewerManager->Reload();
}



vrViewerTOC::vrViewerTOC(wxWindow * parent, wxWindowID id, const wxPoint & pos,
						 const wxSize & size, int  n, const wxString choices[],
                         long  style):
wxCheckListBox(parent, id, pos, size, n, choices, style){

	m_FreezeStatus = false;
	m_ViewerManager = NULL;
}



vrViewerTOC::~vrViewerTOC() {
}



bool vrViewerTOC::Add(int index, vrRenderer * renderer, int control) {
	if (index >= (signed) GetCount()) {
		int myPos = Append(renderer->GetLayer()->GetDisplayName().GetFullName());
		Check(myPos, renderer->GetVisible());
		return true;
	}

	// if index is -1, insert at the begining.
	if (index == -1) {
		index = 0;
	}

	int myPos = Insert(renderer->GetLayer()->GetDisplayName().GetFullName(), index);
	Check(myPos, renderer->GetVisible());
	return true;
}


bool vrViewerTOC::Move(long oldpos, long newpos) {
	wxString myOldText = GetString(oldpos);
	bool myOldChecked = IsChecked(oldpos);
	bool myOldSelected = IsSelected(oldpos);

	// switching two values
	if (abs(oldpos - newpos) == 1) {
		SetString (oldpos, GetString(newpos));
		Check(oldpos, IsChecked(newpos));
		if (IsSelected(newpos)) {
			Select(oldpos);
		}

		SetString(newpos, myOldText);
		Check(newpos, myOldChecked);
		if (myOldSelected) {
			Select(newpos);
		}
	}
	else {
		int myNewPos = newpos;
		if (newpos > oldpos) {
			myNewPos = myNewPos -1;
		}

		Remove(oldpos);
		Insert(myOldText, myNewPos);
		Check(myNewPos, myOldChecked);
		if (myOldSelected) {
			Select(myNewPos);
		}
	}
	return true;
}


void vrViewerTOC::Remove(int index) {
	wxASSERT(index != wxNOT_FOUND);
	Delete(index);
}

void vrViewerTOC::FreezeBegin() {
	wxASSERT(m_FreezeStatus==false);
	m_FreezeStatus = true;
	Freeze();
}

void vrViewerTOC::FreezeEnd() {
	wxASSERT(m_FreezeStatus == true);
	m_FreezeStatus = false;
	Thaw();
}

void vrViewerTOC::SetViewerLayerManager(vrViewerLayerManager * value) {
	wxASSERT(value);
	m_ViewerManager = value;
}













void vrViewerTOCTree::OnMouseRightDown(wxMouseEvent & event) {
}



void vrViewerTOCTree::OnMouseWheel(wxMouseEvent & event) {
}



void vrViewerTOCTree::OnSetColorPen(wxCommandEvent & event) {
}



void vrViewerTOCTree::OnSetColorBrush(wxCommandEvent & event) {
}



void vrViewerTOCTree::OnSetTransparency(wxCommandEvent & event) {
}



void vrViewerTOCTree::OnSetWidth(wxCommandEvent & event) {
}



void vrViewerTOCTree::OnVisibleStatusChanged(wxCommandEvent & event) {
}



void vrViewerTOCTree::_ShowMenuContextual(int id, vrRenderer * renderer) {
}



void vrViewerTOCTree::_ReloadData() {
}



vrViewerTOCTree::vrViewerTOCTree(wxWindow * parent, wxWindowID id, const wxPoint & pos, const wxSize & size, long style) 
: wxTreeCtrl(parent, id, pos, size, style) {
    m_FreezeStatus = false;
    m_ViewerManager = NULL;
    m_RootItem = AddRoot("Root");
}



vrViewerTOCTree::~vrViewerTOCTree() {
}



bool vrViewerTOCTree::Add(int index, vrRenderer * renderer, int control) {
    AppendItem(m_RootItem, renderer->GetLayer()->GetDisplayName().GetFullName());
    // TODO: Check item
    return true;
}



bool vrViewerTOCTree::Move(long oldpos, long newpos) {
    return false;
}



bool vrViewerTOCTree::Remove(int index) {
    return false;
}



void vrViewerTOCTree::FreezeBegin() {
    wxASSERT(m_FreezeStatus==false);
	m_FreezeStatus = true;
	Freeze();
}



void vrViewerTOCTree::FreezeEnd() {
    wxASSERT(m_FreezeStatus == true);
	m_FreezeStatus = false;
	Thaw();
}



void vrViewerTOCTree::SetViewerLayerManager(vrViewerLayerManager * value) {
    wxASSERT(value);
    m_ViewerManager = value;
}








