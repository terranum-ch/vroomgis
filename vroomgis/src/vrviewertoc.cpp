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
#include <wx/colordlg.h>	// colour dialog
#include <wx/numdlg.h>		// number entry dialog
#include <wx/imaglist.h>    // images list
#include <wx/renderer.h>    // wxRenderNative


#include "vrviewertoc.h"
#include "vrevent.h"
#include "vrviewerlayermanager.h"
#include "vrlayer.h"
#include "vrrender.h"
#include "vrrendervectorc2p.h"
#include "vroomgis_bmp.h"


void vrViewerTOC::ShowMenuContextual(vrRenderer * renderer) {
    wxASSERT(renderer);    
	wxMenu myPopMenu;
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
	GetControl()->PopupMenu(&myPopMenu);
}



void vrViewerTOC::ReloadData() {
    // ask for reloading data
	wxASSERT(m_ViewerManager);
	m_ViewerManager->Reload();
}



vrViewerTOC::vrViewerTOC() {
    m_FreezeStatus = false;
	m_ViewerManager = NULL;
}


vrViewerTOC::~vrViewerTOC() {
}



void vrViewerTOC::FreezeBegin() {
    wxASSERT(IsFreezed() == false);
    m_FreezeStatus = true;
    GetControl()->Freeze();
}



void vrViewerTOC::FreezeEnd() {
    wxASSERT(IsFreezed() == true);
    m_FreezeStatus = false;
    GetControl()->Thaw();
}



void vrViewerTOC::SetViewerLayerManager(vrViewerLayerManager * value) {
    wxASSERT(value);
    m_ViewerManager = value;
}






/************************************ vrViewerTOCList **********************************************/
void vrViewerTOCList::OnMouseRightDown(wxMouseEvent & event) {
    if (m_CheckList->GetCount() == 0) {
		return;
	}
    
	// hit test
	wxPoint myPos = event.GetPosition();
	int myItemID = m_CheckList->HitTest(myPos);
	wxLogMessage("Item seleced id is %d @ position %d - %d", myItemID, myPos.x, myPos.y);
    
	if (myItemID == wxNOT_FOUND) {
		return;
	}
    
	m_CheckList->SetSelection(myItemID);
	wxASSERT(GetViewerLayerManager());
	vrRenderer * myRenderer = GetViewerLayerManager()->GetRenderer(myItemID);
	wxASSERT(myRenderer);
    
	ShowMenuContextual(myRenderer);
}



void vrViewerTOCList::OnMouseWheel(wxMouseEvent & event) {
    if (m_CheckList->GetCount() == 0) {
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
		if (myItemSelected == (signed) m_CheckList->GetCount() -1) {
			return;
		}
		myNewPosition++;
	}else {
		// myRotation == 0 MBP trackpad...
		return;
	}
    
	wxASSERT(GetViewerLayerManager());
	wxLogMessage("Moving item : %d to %d", myItemSelected, myNewPosition);
	GetViewerLayerManager()->Move(myItemSelected, myNewPosition);
}



void vrViewerTOCList::OnSetColorPen(wxCommandEvent & event) {
    int mySelItem = GetSelection();
    wxASSERT(mySelItem != wxNOT_FOUND);
    wxASSERT(GetViewerLayerManager());
    
    vrRenderVector * myRenderVector = (vrRenderVector*) GetViewerLayerManager()->GetRenderer(mySelItem)->GetRender();
    wxASSERT(myRenderVector);
    wxColourData myActualCol;
    myActualCol.SetColour(myRenderVector->GetColorPen());
    wxColourDialog myColDlg (GetControl(), &myActualCol);
    if (myColDlg.ShowModal() == wxID_OK) {
        myRenderVector->SetColorPen(myColDlg.GetColourData().GetColour());
        ReloadData();
    }
}



void vrViewerTOCList::OnSetColorBrush(wxCommandEvent & event) {
    int mySelItem = GetSelection();
	wxASSERT(mySelItem != wxNOT_FOUND);
	wxASSERT(GetViewerLayerManager());
    
	vrRenderVector * myRenderVector = (vrRenderVector*) GetViewerLayerManager()->GetRenderer(mySelItem)->GetRender();
	wxASSERT(myRenderVector);
    
	// displaying colour dialog
	wxColourData myActualCol;
	myActualCol.SetColour(myRenderVector->GetColorBrush());
	wxColourDialog myColDlg (GetControl(), &myActualCol);
	if (myColDlg.ShowModal() == wxID_OK) {
		myRenderVector->SetColorBrush(myColDlg.GetColourData().GetColour());
		ReloadData();
	}
}



void vrViewerTOCList::OnSetTransparency(wxCommandEvent & event) {
    int mySelItem = GetSelection();
	wxASSERT(mySelItem != wxNOT_FOUND);
	wxASSERT(GetViewerLayerManager());
    
	vrRender * myRender = GetViewerLayerManager()->GetRenderer(mySelItem)->GetRender();
	wxASSERT(myRender);
    
	wxNumberEntryDialog myNumDlg(GetControl(),
								 "Adjust the transparency percent\n0 is fully opaque, 100 is fully transparent",
								 "Transparency percent:",
								 "Adjust Opacity",
								 myRender->GetTransparency(),
								 0,
								 100);
	if (myNumDlg.ShowModal()==wxID_OK) {
		myRender->SetTransparency(myNumDlg.GetValue());
		ReloadData();
	}
}



void vrViewerTOCList::OnSetWidth(wxCommandEvent & event) {
    int mySelItem = GetSelection();
	wxASSERT(mySelItem != wxNOT_FOUND);
	wxASSERT(GetViewerLayerManager());
	vrRender * myRender = GetViewerLayerManager()->GetRenderer(mySelItem)->GetRender();
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
	wxNumberEntryDialog myNumDlg(GetControl(),
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
	ReloadData();
}



void vrViewerTOCList::OnSetBrushStyle(wxCommandEvent & event){
	int mySelItem = GetSelection();
	wxASSERT(mySelItem != wxNOT_FOUND);
	wxASSERT(GetViewerLayerManager());
    
	vrRenderVector * myRenderVector = (vrRenderVector*) GetViewerLayerManager()->GetRenderer(mySelItem)->GetRender();
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
		ReloadData();
	}
}



void vrViewerTOCList::OnVisibleStatusChanged(wxCommandEvent & event) {
    wxASSERT(GetViewerLayerManager());
	vrRenderer * myItemRenderer = GetViewerLayerManager()->GetRenderer(event.GetInt());
	wxASSERT(myItemRenderer);
	myItemRenderer->SetVisible(m_CheckList->IsChecked(event.GetInt()));
	ReloadData();
}



vrViewerTOCList::vrViewerTOCList(wxWindow * parent, wxWindowID id, 
                                 const wxPoint & pos, const wxSize & size,
                                 int  n , const wxString choices[],
                                 long  style) {
    m_CheckList = new wxCheckListBox(parent, id, pos, size,n, choices, style);
    
    // connect event
    m_CheckList->Bind(wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, &vrViewerTOCList::OnVisibleStatusChanged, this);
    m_CheckList->Bind(wxEVT_RIGHT_DOWN, &vrViewerTOCList::OnMouseRightDown, this);
    m_CheckList->Bind(wxEVT_MOUSEWHEEL, &vrViewerTOCList::OnMouseWheel, this);
    
    m_CheckList->Bind(wxEVT_COMMAND_MENU_SELECTED, &vrViewerTOCList::OnSetColorPen, this, vrID_POPUP_PEN_COLOR);
    m_CheckList->Bind(wxEVT_COMMAND_MENU_SELECTED, &vrViewerTOCList::OnSetColorBrush, this, vrID_POPUP_BRUSH_COLOR);
    m_CheckList->Bind(wxEVT_COMMAND_MENU_SELECTED, &vrViewerTOCList::OnSetTransparency, this, vrID_POPUP_TRANSPARENCY);
    m_CheckList->Bind(wxEVT_COMMAND_MENU_SELECTED, &vrViewerTOCList::OnSetWidth, this, vrID_POPUP_DRAWING_WIDTH);
    m_CheckList->Bind(wxEVT_COMMAND_MENU_SELECTED, &vrViewerTOCList::OnSetBrushStyle, this, vrID_POPUP_BRUSH_SOLID, vrID_POPUP_BRUSH_BDIAGONAL);
}



vrViewerTOCList::~vrViewerTOCList() {
    // disconnect event
    m_CheckList->Unbind(wxEVT_RIGHT_DOWN, &vrViewerTOCList::OnMouseRightDown, this);
    m_CheckList->Unbind(wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, &vrViewerTOCList::OnVisibleStatusChanged, this);
    m_CheckList->Unbind(wxEVT_MOUSEWHEEL, &vrViewerTOCList::OnMouseWheel, this);

    m_CheckList->Unbind(wxEVT_COMMAND_MENU_SELECTED, &vrViewerTOCList::OnSetColorPen, this, vrID_POPUP_PEN_COLOR);
    m_CheckList->Unbind(wxEVT_COMMAND_MENU_SELECTED, &vrViewerTOCList::OnSetColorBrush, this, vrID_POPUP_BRUSH_COLOR);
    m_CheckList->Unbind(wxEVT_COMMAND_MENU_SELECTED, &vrViewerTOCList::OnSetTransparency, this, vrID_POPUP_TRANSPARENCY);
    m_CheckList->Unbind(wxEVT_COMMAND_MENU_SELECTED, &vrViewerTOCList::OnSetWidth, this, vrID_POPUP_DRAWING_WIDTH);
    m_CheckList->Unbind(wxEVT_COMMAND_MENU_SELECTED, &vrViewerTOCList::OnSetBrushStyle, this, vrID_POPUP_BRUSH_SOLID, vrID_POPUP_BRUSH_BDIAGONAL);
    
    wxDELETE(m_CheckList);
}



bool vrViewerTOCList::Add(int index, vrRenderer * renderer) {
	if (index >= (signed) m_CheckList->GetCount()) {
		int myPos = m_CheckList->Append(renderer->GetLayer()->GetDisplayName().GetFullName());
		m_CheckList->Check(myPos, renderer->GetVisible());
		return true;
	}
    
	// if index is -1, insert at the begining.
	if (index == -1) {
		index = 0;
	}
    
	int myPos = m_CheckList->Insert(renderer->GetLayer()->GetDisplayName().GetFullName(), index);
	m_CheckList->Check(myPos, renderer->GetVisible());
	return true;
}



bool vrViewerTOCList::Move(long oldpos, long newpos) {
    wxString myOldText = m_CheckList->GetString(oldpos);
	bool myOldChecked = m_CheckList->IsChecked(oldpos);
	bool myOldSelected = m_CheckList->IsSelected(oldpos);
    
	// switching two values
	if (abs(oldpos - newpos) == 1) {
		m_CheckList->SetString (oldpos, m_CheckList->GetString(newpos));
		m_CheckList->Check(oldpos, m_CheckList->IsChecked(newpos));
		if (m_CheckList->IsSelected(newpos)) {
			m_CheckList->Select(oldpos);
		}
        
		m_CheckList->SetString(newpos, myOldText);
		m_CheckList->Check(newpos, myOldChecked);
		if (myOldSelected) {
			m_CheckList->Select(newpos);
		}
	}
	else {
		int myNewPos = newpos;
		if (newpos > oldpos) {
			myNewPos = myNewPos -1;
		}
        
		Remove(oldpos);
		m_CheckList->Insert(myOldText, myNewPos);
		m_CheckList->Check(myNewPos, myOldChecked);
		if (myOldSelected) {
			m_CheckList->Select(myNewPos);
		}
	}
	return true;
}



bool vrViewerTOCList::Remove(int index) {
    wxASSERT(index != wxNOT_FOUND);
	m_CheckList->Delete(index);
    return true;
}



int vrViewerTOCList::GetSelection() {
    return m_CheckList->GetSelection();
}


wxControl * vrViewerTOCList::GetControl(){
    return m_CheckList;
}







/************************************ vrViewerTOCTreeData ******************************************/
vrViewerTOCTreeData::vrViewerTOCTreeData() {
    m_ItemType = vrTREEDATA_TYPE_INVALID;
}



vrViewerTOCTreeData::~vrViewerTOCTreeData() {
}



/************************************ vrViewerTOCTree **********************************************/
wxTreeItemId vrViewerTOCTree::_IndexToTree( wxTreeItemId root, const wxString& searchtext, vrVIEWERTOC_TREEDATA_TYPES searchtype)
{
	wxTreeItemIdValue cookie;
	wxTreeItemId search;
	wxTreeItemId item = m_Tree->GetFirstChild( root, cookie );
	wxTreeItemId child;
    
	while( item.IsOk() ){
		wxString sData = m_Tree->GetItemText(item);
        vrViewerTOCTreeData * myData = (vrViewerTOCTreeData*) m_Tree->GetItemData(item);
		if( searchtext.CompareTo(sData) == 0 && myData->m_ItemType == searchtype){
			return item;
		}
		if( m_Tree->ItemHasChildren( item ) ){
			wxTreeItemId search = _IndexToTree( item, searchtext, searchtype);
			if( search.IsOk() ){
				return search;
			}
		}
		item = m_Tree->GetNextChild( root, cookie);
	}
    
	/* Not found */
	wxTreeItemId dummy;
	return dummy;
}



int vrViewerTOCTree::_TreeToIndex(wxTreeItemId treeitem, vrVIEWERTOC_TREEDATA_TYPES searchtype) {
    wxASSERT(m_Tree);
    wxASSERT(GetViewerLayerManager());
    
    wxString myItemText =  m_Tree->GetItemText(treeitem);
    vrViewerTOCTreeData * myData = (vrViewerTOCTreeData*) m_Tree->GetItemData(treeitem);
    
    for (int i = 0; i< GetViewerLayerManager()->GetCount(); i++) {
        wxString myTmpText = GetViewerLayerManager()->GetRenderer(i)->GetLayer()->GetDisplayName().GetFullName();
        if (myTmpText.CompareTo(myItemText) == 0 && myData->m_ItemType == searchtype) {
            return i;
        }
    }
    return wxNOT_FOUND;
}



void vrViewerTOCTree::_InitBitmapList() {
    wxImageList *images = new wxImageList(16, 16, true);
    wxBitmap myTempBmp (16,16);
    wxBitmap myTempBmp2 (16,16);
    { 
        // unchecked
        wxMemoryDC myDC;
        myDC.SelectObject(myTempBmp);
        myDC.SetBackground(*wxTheBrushList->FindOrCreateBrush(m_Tree->GetBackgroundColour(), wxSOLID));
        myDC.Clear();
        wxRendererNative::Get().DrawCheckBox(GetControl(), myDC, wxRect(0, 0, 16, 16), 0);
        
        // checked
        myDC.SelectObject(myTempBmp2);
        myDC.SetBackground(*wxTheBrushList->FindOrCreateBrush(m_Tree->GetBackgroundColour(), wxSOLID));
        myDC.Clear();
        wxRendererNative::Get().DrawCheckBox(GetControl(), myDC, wxRect(0, 0, 16, 16), wxCONTROL_CHECKED);
        myDC.SelectObject(wxNullBitmap);
    }
    
    wxBitmap myGroupBmp (*_img_tree_folder_on);
    wxBitmap myGroupBmpOff (*_img_tree_folder_off);
    
    images->Add(myTempBmp);
    images->Add(myTempBmp2);
    images->Add(myGroupBmp);
    images->Add(myGroupBmpOff);
    m_Tree->AssignImageList(images);
}


void vrViewerTOCTree::_SetItemImageUnique(wxTreeItemId item, vrVIEWERTOC_IMAGES_TYPES image) {
    m_Tree->SetItemImage(item, image, wxTreeItemIcon_Normal);
    m_Tree->SetItemImage(item, image, wxTreeItemIcon_Selected);
}



void vrViewerTOCTree::OnMouseDown(wxMouseEvent & event) {
	int flags = 0;
	wxTreeItemId clickedid = m_Tree->HitTest(event.GetPosition(), flags);
	if (flags & wxTREE_HITTEST_ONITEMICON)
	{
        vrViewerTOCTreeData * myData = (vrViewerTOCTreeData*) m_Tree->GetItemData(clickedid);
        switch (myData->m_ItemType) {
            case vrTREEDATA_TYPE_LAYER:
            {
                int myItemIndex = _TreeToIndex(clickedid, myData->m_ItemType);
                if (myItemIndex == wxNOT_FOUND){
                    return;
                }
                
                if(m_Tree->GetItemImage(clickedid) == vrVIEWERTOC_IMAGE_CHECKED){
                    _SetItemImageUnique(clickedid, vrVIEWERTOC_IMAGE_UNCHECKED);
                }
                else{
                    _SetItemImageUnique(clickedid, vrVIEWERTOC_IMAGE_CHECKED);
                }
                
                bool IsVisible = GetViewerLayerManager()->GetRenderer(myItemIndex)->GetVisible();         
                GetViewerLayerManager()->GetRenderer(myItemIndex)->SetVisible(!IsVisible);
                ReloadData();
            }   
                break;
                
                
            case vrTREEDATA_TYPE_GROUP:
                if (m_Tree->GetItemImage(clickedid) == vrVIEWERTOC_IMAGE_GROUP_ON) {
                    _SetItemImageUnique(clickedid, vrVIEWERTOC_IMAGE_GROUP_OFF); 
                }else{
                    _SetItemImageUnique(clickedid, vrVIEWERTOC_IMAGE_GROUP_ON);
                }
                break;
                
                
            default:
                break;
        }
        return;
	}
	event.Skip();
}


void vrViewerTOCTree::OnItemRightDown(wxTreeEvent & event) {
	wxMenu myPopMenu;
	myPopMenu.Append(vrID_POPUP_TRANSPARENCY, _("Set Transparency..."));
    myPopMenu.Append(vrID_POPUP_GROUP_ADD, _("Add Group"));
    
	GetControl()->PopupMenu(&myPopMenu);
    event.Skip();
}

void vrViewerTOCTree::OnSetColorPen(wxCommandEvent & event) {
}

void vrViewerTOCTree::OnSetColorBrush(wxCommandEvent & event) {
}

void vrViewerTOCTree::OnSetTransparency(wxCommandEvent & event) {
}

void vrViewerTOCTree::OnSetWidth(wxCommandEvent & event) {
}

void vrViewerTOCTree::OnNewGroup(wxCommandEvent & event) {
    AddGroup(_("New Group"));
}


void vrViewerTOCTree::OnDragStart(wxTreeEvent & event){
    m_DragItemID = wxTreeItemId();
	wxASSERT(m_DragItemID.IsOk() == false);
	
    m_DragItemID = event.GetItem();
    if (m_DragItemID == m_RootNode) {
        wxLogWarning("This can't be dragged!");
        return;
    }
    
    vrViewerTOCTreeData * myData = (vrViewerTOCTreeData*) m_Tree->GetItemData(m_DragItemID);
    wxASSERT(myData);
    
    switch (myData->m_ItemType) {
        case vrTREEDATA_TYPE_LAYER:
        case vrTREEDATA_TYPE_GROUP:
            event.Allow();
            break;
            
        default:
            wxLogWarning("This can't be dragged!");
            break;
    }
}


void vrViewerTOCTree::OnDragStop(wxTreeEvent & event){
    if (m_DragItemID.IsOk() == false) {
        return;
    }
    
    wxTreeItemId myItemStart = m_DragItemID;
	wxTreeItemId myItemStop = event.GetItem();
    
    if (myItemStart == myItemStop){
        return;
    }
    
    bool m_AllreadyFreezed = IsFreezed();
    if (m_AllreadyFreezed == false) {
        FreezeBegin();
    }
    
    vrViewerTOCTreeData * myDataStop = (vrViewerTOCTreeData*) m_Tree->GetItemData(myItemStop);
    // drag into group
    if (myDataStop->m_ItemType == vrTREEDATA_TYPE_GROUP) {
        // copy treectrl to group
        vrViewerTOCTreeData * myDataCopied = (vrViewerTOCTreeData*) m_Tree->GetItemData(myItemStart);
        vrViewerTOCTreeData * myDataCopiedNew = new vrViewerTOCTreeData(*myDataCopied);
        
        m_Tree->AppendItem (myItemStop, m_Tree->GetItemText(myItemStart),
                            m_Tree->GetItemImage(myItemStart), -1, myDataCopiedNew);
        
        // delete item
        m_Tree->Delete(myItemStart);
    }
    
    
    if (m_AllreadyFreezed == false) {
        FreezeEnd();
    }
}



void vrViewerTOCTree::OnEditStart(wxTreeEvent & event) {
    vrViewerTOCTreeData * myData = (vrViewerTOCTreeData*) m_Tree->GetItemData(event.GetItem());
    wxASSERT(myData);
    
    if (myData->m_ItemType != vrTREEDATA_TYPE_GROUP) {
        event.Veto();
    }
    wxLogMessage("Starting edition");
    event.Allow();
    event.Skip();
}

/*
void tmTOCCtrl::OnDragStart(wxTreeEvent & event){
	m_DragItemID = wxTreeItemId();
	wxASSERT(m_DragItemID.IsOk() == false);
	
	if ( event.GetItem() != GetRootItem() ){
		event.Allow();
		m_DragItemID = event.GetItem();
	}
	else {
		wxLogWarning("This can't be dragged!");
	}
}



void tmTOCCtrl::OnDragStop(wxTreeEvent & event){
	wxLogMessage("Dragging stopped!");
	
	wxTreeItemId myItemStart = m_DragItemID;
	wxTreeItemId myItemStop = event.GetItem();
	
	m_DragItemID = wxTreeItemId();
	wxASSERT(m_DragItemID.IsOk() == false);
	
	if (myItemStop == myItemStart) {
		return;
	}
	
	if (myItemStop.IsOk()) {
		int myItemStartPos = wxNOT_FOUND;
		int myItemStopPos = wxNOT_FOUND;
		
		wxASSERT(m_root.IsOk());
		wxTreeItemIdValue myCookie;
		wxTreeItemId myFirstLayer = GetFirstChild(m_root, myCookie);
		wxASSERT(myFirstLayer.IsOk());
		if (myFirstLayer == myItemStart) {
			myItemStartPos = 0;
		}
		if (myFirstLayer == myItemStop) {
			myItemStopPos = 0;
		}
		
		int myIterPosition = 1;
		while (1) {
			wxTreeItemId myIterLayer = GetNextChild(m_root, myCookie);
			if (myIterLayer.IsOk() == false) {
				break;
			}
			if (myIterLayer == myItemStart) {
				myItemStartPos = myIterPosition;
			}
			if (myIterLayer == myItemStop) {
				myItemStopPos = myIterPosition;
			}
			myIterPosition++;
		}
		
		wxLogMessage("Item %d moved to %d", myItemStartPos, myItemStopPos);
		
		// move items
		if (abs(myItemStopPos - myItemStartPos) == 1) {
			SwapLayers(myItemStart, myItemStopPos);
		}
		else {
			if (myItemStopPos == 0) {
				MoveLayers(myItemStart, 0);
			}
			else {
				MoveLayers(myItemStart, myItemStopPos+1);
			}
		}
		
		// update display
		wxCommandEvent evt(tmEVT_LM_UPDATE, wxID_ANY);
		GetEventHandler()->AddPendingEvent(evt);
	}
}
*/




vrViewerTOCTree::vrViewerTOCTree(wxWindow * parent, wxWindowID id,
                                 const wxPoint & pos, const wxSize & size,
                                 long style) {
    m_Tree = new wxTreeCtrl(parent, id, pos, size, style);
    m_RootNode = m_Tree->AddRoot("Project");
    _InitBitmapList();
    
    // connect event
    m_Tree->Bind(wxEVT_LEFT_DOWN, &vrViewerTOCTree::OnMouseDown, this);
    m_Tree->Bind(wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, &vrViewerTOCTree::OnItemRightDown, this);
    m_Tree->Bind(wxEVT_COMMAND_TREE_BEGIN_DRAG, &vrViewerTOCTree::OnDragStart, this);
    m_Tree->Bind(wxEVT_COMMAND_TREE_END_DRAG, &vrViewerTOCTree::OnDragStop, this);
    m_Tree->Bind(wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, &vrViewerTOCTree::OnEditStart, this);
    
    m_Tree->Bind(wxEVT_COMMAND_MENU_SELECTED, &vrViewerTOCTree::OnNewGroup, this, vrID_POPUP_GROUP_ADD);
}



vrViewerTOCTree::~vrViewerTOCTree() {
    // disconnect event
    m_Tree->Unbind(wxEVT_LEFT_DOWN, &vrViewerTOCTree::OnMouseDown, this);
    m_Tree->Unbind(wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, &vrViewerTOCTree::OnItemRightDown, this);
    m_Tree->Unbind(wxEVT_COMMAND_TREE_BEGIN_DRAG, &vrViewerTOCTree::OnDragStart, this);
    m_Tree->Unbind(wxEVT_COMMAND_TREE_END_DRAG, &vrViewerTOCTree::OnDragStop, this);
    m_Tree->Unbind(wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, &vrViewerTOCTree::OnEditStart, this);

    m_Tree->Unbind(wxEVT_COMMAND_MENU_SELECTED, &vrViewerTOCTree::OnNewGroup, this, vrID_POPUP_GROUP_ADD);

    
    wxDELETE(m_Tree);
}



bool vrViewerTOCTree::Add(int index, vrRenderer * renderer) {
    vrViewerTOCTreeData * myData = new vrViewerTOCTreeData();
    myData->m_ItemType = vrTREEDATA_TYPE_LAYER;
    
    if (index >= (signed) m_Tree->GetCount()) {
		wxTreeItemId myAddedId = m_Tree->AppendItem(m_RootNode, renderer->GetLayer()->GetDisplayName().GetFullName(),1,1, myData);
        //m_Tree->SetItemState(myAddedId, wxTREE_ITEMSTATE_NONE);
		return true;
	}
    
	// if index is -1, insert at the begining.
	if (index == -1) {
		index = 0;
	}
    
    wxTreeItemId myAddedId = m_Tree->InsertItem(m_RootNode, index, renderer->GetLayer()->GetDisplayName().GetFullName(),1,1,myData);
    //m_Tree->SetItemState(myAddedId, wxTREE_ITEMSTATE_NONE);
    
    return true;
}



bool vrViewerTOCTree::Move(long oldpos, long newpos) {
    return false;
}



bool vrViewerTOCTree::Remove(int index) {
    // search for item text
    wxString myItemText = GetViewerLayerManager()->GetRenderer(index)->GetLayer()->GetDisplayName().GetFullName();
    wxTreeItemId myFound = _IndexToTree (m_RootNode, myItemText, vrTREEDATA_TYPE_LAYER);    
    if (myFound.IsOk() == false) {
        wxLogError(_("Error, item %d not found!!!"), index);
        return false;
    }
    else{
        wxLogMessage("Item : %s found (index = %d) ", myItemText, index);
    }
 
    m_Tree->Delete(myFound);
    return true;
}



int vrViewerTOCTree::GetSelection() {
    if (m_Tree->GetSelection().IsOk() == false) {
        return wxNOT_FOUND;
    }
    
    wxTreeItemId mySelTreeId = m_Tree->GetSelection();
    return _TreeToIndex(mySelTreeId, vrTREEDATA_TYPE_LAYER);
}



wxControl * vrViewerTOCTree::GetControl() {
    wxASSERT(m_Tree);
    return m_Tree;
}


bool vrViewerTOCTree::AddGroup(const wxString & name) {
    vrViewerTOCTreeData * myData = new vrViewerTOCTreeData();
    myData->m_ItemType = vrTREEDATA_TYPE_GROUP;
    m_Tree->AppendItem(m_RootNode, name, vrVIEWERTOC_IMAGE_GROUP_ON, -1, myData);
    return true;
}


