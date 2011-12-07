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
#include "vrlayervector.h"
#include "vrrender.h"
#include "vrrendervectorc2p.h"
#include "vroomgis_bmp.h"


wxMenu * vrViewerTOC::CreateContextualMenu(vrRenderer * renderer, bool usegroup) {
    wxMenu * myPopMenu = new wxMenu();
    if (usegroup == true){
        myPopMenu->Append(vrID_POPUP_GROUP_ADD, _("Add Group"));
        myPopMenu->AppendSeparator();
    }
    
	myPopMenu->Append(vrID_POPUP_TRANSPARENCY, _("Set Transparency..."));
    myPopMenu->AppendSeparator();
    myPopMenu->Append(vrID_POPUP_PEN_COLOR, _("Set Pen color..."));
    myPopMenu->Append(vrID_POPUP_DRAWING_WIDTH, _("Set Pen width..."));
    myPopMenu->AppendSeparator();
    wxMenu * myBrushMenu = new wxMenu();
    myBrushMenu->Append(vrID_POPUP_BRUSH_COLOR, _("Set Brush color..."));
    myBrushMenu->AppendSeparator();
    myBrushMenu->AppendRadioItem(vrID_POPUP_BRUSH_SOLID, _("Solid Brush"));
    myBrushMenu->AppendRadioItem(vrID_POPUP_BRUSH_TRANSPARENT, _("Transparent Brush"));
    myBrushMenu->AppendRadioItem(vrID_POPUP_BRUSH_BDIAGONAL, _("Diagonal Brush"));
    myPopMenu->AppendSubMenu(myBrushMenu, _T("Brush"));
    
    // disable all
    myPopMenu->Enable(vrID_POPUP_PEN_COLOR, false);
    myPopMenu->Enable(vrID_POPUP_DRAWING_WIDTH, false);
    myPopMenu->Enable(vrID_POPUP_BRUSH_COLOR, false);
    myPopMenu->Enable(vrID_POPUP_BRUSH_SOLID, false);
    myPopMenu->Enable(vrID_POPUP_BRUSH_TRANSPARENT, false);
    myPopMenu->Enable(vrID_POPUP_BRUSH_BDIAGONAL, false);
      
    if (renderer != NULL) {
        switch (renderer->GetRender()->GetType()) {
                
            case vrRENDER_VECTOR:
                myPopMenu->Enable(vrID_POPUP_PEN_COLOR, true);
                myPopMenu->Enable(vrID_POPUP_DRAWING_WIDTH, true);
            {
                vrLayerVector * myVLayer = (vrLayerVector*) renderer->GetLayer();
                if (myVLayer->GetGeometryType() == wkbPolygon || 
                    myVLayer->GetGeometryType() == wkbPolygon25D || 
                    myVLayer->GetGeometryType() == wkbMultiPolygon || 
                    myVLayer->GetGeometryType() == wkbMultiPolygon25D) {
                    
                    myPopMenu->Enable(vrID_POPUP_BRUSH_COLOR, true);
                    myPopMenu->Enable(vrID_POPUP_BRUSH_SOLID, true);
                    myPopMenu->Enable(vrID_POPUP_BRUSH_TRANSPARENT, true);
                    myPopMenu->Enable(vrID_POPUP_BRUSH_BDIAGONAL, true);
                    
                    // check brush choice!
                    vrRenderVector * myRenderVector = (vrRenderVector*) renderer->GetRender();
                    wxBrushStyle myBrushStyle = myRenderVector->GetBrushStyle();
                    switch (myBrushStyle) {
                        case wxBRUSHSTYLE_SOLID:
                            myPopMenu->Check(vrID_POPUP_BRUSH_SOLID, true);
                            break;
                            
                        case wxBRUSHSTYLE_TRANSPARENT:
                            myPopMenu->Check(vrID_POPUP_BRUSH_TRANSPARENT, true);
                            break;
                            
                        case wxBRUSHSTYLE_BDIAGONAL_HATCH:
                            myPopMenu->Check(vrID_POPUP_BRUSH_BDIAGONAL, true);
                            break;
                            
                        default:
                            //wxFAIL;
                            break;
                    }
                }
            }
                break;
              
                
            case vrRENDER_VECTOR_C2P_DIPS:
                myPopMenu->Enable(vrID_POPUP_DRAWING_WIDTH, true);
                break;
                
            default:
                break;
                
        }
    }
    
    return myPopMenu;
}


void vrViewerTOC::SymbologyModified() {
    // do nothing but may be derived to implement behavior when operation was done on the control
}


void vrViewerTOC::ReloadData() {
    // ask for reloading data
	wxASSERT(m_ViewerManager);
	m_ViewerManager->Reload();
}



bool vrViewerTOC::SetColorPen(int itemindex) {
    if (itemindex == wxNOT_FOUND) {
        wxLogError(_("Error getting selected layer!"));
        return false;
    }
    
    wxASSERT(GetViewerLayerManager());
    vrRenderVector * myRenderVector = (vrRenderVector*) GetViewerLayerManager()->GetRenderer(itemindex)->GetRender();
    wxASSERT(myRenderVector);
    wxColourData myActualCol;
    myActualCol.SetColour(myRenderVector->GetColorPen());
    wxColourDialog myColDlg (GetControl(), &myActualCol);
    if (myColDlg.ShowModal() == wxID_OK) {
        myRenderVector->SetColorPen(myColDlg.GetColourData().GetColour());
        SymbologyModified();
        return true;
    }
    return false;
}



bool vrViewerTOC::SetWidth(int itemindex) {
    if (itemindex == wxNOT_FOUND) {
        wxLogError(_("Error getting selected layer!"));
        return false;
    }
    wxASSERT(GetViewerLayerManager());
    vrRender * myRender = GetViewerLayerManager()->GetRenderer(itemindex)->GetRender();
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
        return false;
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
        return false;
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
        return false;
    }
    SymbologyModified();
    return true;
}



bool vrViewerTOC::SetBrushStyle(int itemindex, int menuid) {
    if (itemindex == wxNOT_FOUND) {
        wxLogError(_("Error getting selected layer!"));
        return false;
    }
	wxASSERT(GetViewerLayerManager());
    
	vrRenderVector * myRenderVector = (vrRenderVector*) GetViewerLayerManager()->GetRenderer(itemindex)->GetRender();
	wxASSERT(myRenderVector);
    
	// displaying colour dialog
	wxBrushStyle myOldStyle = myRenderVector->GetBrushStyle();
	wxBrushStyle myStyle = wxBRUSHSTYLE_SOLID;
	switch (menuid) {
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
			wxLogError(_("Brush style not supported: %d"), menuid);
			break;
	}
	myRenderVector->SetBrushStyle(myStyle);
	if (myOldStyle != myStyle) {
        SymbologyModified();
		return true;
	}
    return false;
}



bool vrViewerTOC::SetColorBrush(int itemindex) {
    if (itemindex == wxNOT_FOUND) {
        wxLogError(_("Error getting selected layer!"));
        return false;
    }
	wxASSERT(GetViewerLayerManager());
	vrRenderVector * myRenderVector = (vrRenderVector*) GetViewerLayerManager()->GetRenderer(itemindex)->GetRender();
	wxASSERT(myRenderVector);
    
	// displaying colour dialog
	wxColourData myActualCol;
	myActualCol.SetColour(myRenderVector->GetColorBrush());
	wxColourDialog myColDlg (GetControl(), &myActualCol);
	if (myColDlg.ShowModal() == wxID_OK) {
		myRenderVector->SetColorBrush(myColDlg.GetColourData().GetColour());
        SymbologyModified();
        return true;
	}
    return false;
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
    
	wxMenu * myMenu = CreateContextualMenu(myRenderer, false);
    wxASSERT(myMenu);
    GetControl()->PopupMenu(myMenu);
    
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
    if (SetColorPen(mySelItem) == true) {
        ReloadData();
    }
}



void vrViewerTOCList::OnSetColorBrush(wxCommandEvent & event) {
    int mySelItem = GetSelection();
	if (SetColorBrush(mySelItem) == true) {
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
        SymbologyModified();
		ReloadData();
	}
}



void vrViewerTOCList::OnSetWidth(wxCommandEvent & event) {
    int mySelItem = GetSelection();
	if (SetWidth(mySelItem)==true) {
        ReloadData();
    }
}



void vrViewerTOCList::OnSetBrushStyle(wxCommandEvent & event){
	int mySelItem = GetSelection();
	if (SetBrushStyle(mySelItem, event.GetId()) == true) {
		ReloadData();
	}
}



void vrViewerTOCList::OnVisibleStatusChanged(wxCommandEvent & event) {
    wxASSERT(GetViewerLayerManager());
	vrRenderer * myItemRenderer = GetViewerLayerManager()->GetRenderer(event.GetInt());
	wxASSERT(myItemRenderer);
	myItemRenderer->SetVisible(m_CheckList->IsChecked(event.GetInt()));
    SymbologyModified();
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
    m_CheckedImgType = vrVIEWERTOC_IMAGE_CHECKED;
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
    if(myData == NULL){
        return wxNOT_FOUND;
    }
    
    for (int i = 0; i< GetViewerLayerManager()->GetCount(); i++) {
        wxString myTmpText = GetViewerLayerManager()->GetRenderer(i)->GetLayer()->GetDisplayName().GetFullName();
        if (myData->m_ItemType == searchtype && myTmpText.CompareTo(myItemText) == 0) {
            return i;
        }
    }
    return wxNOT_FOUND;
}



void vrViewerTOCTree::_InitBitmapList() {
    wxImageList *images = new wxImageList(16, 16, true);
    wxBitmap myCheckBmp (16,16);
    wxBitmap myUnCheckBmp (16,16);
    { 
        // unchecked
        wxMemoryDC myDC;
        myDC.SelectObject(myCheckBmp);
        myDC.SetBackground(*wxTheBrushList->FindOrCreateBrush(m_Tree->GetBackgroundColour(), wxSOLID));
        myDC.Clear();
        wxRendererNative::Get().DrawCheckBox(GetControl(), myDC, wxRect(0, 0, 16, 16), 0);
        
        // checked
        myDC.SelectObject(myUnCheckBmp);
        myDC.SetBackground(*wxTheBrushList->FindOrCreateBrush(m_Tree->GetBackgroundColour(), wxSOLID));
        myDC.Clear();
        wxRendererNative::Get().DrawCheckBox(GetControl(), myDC, wxRect(0, 0, 16, 16), wxCONTROL_CHECKED);
        myDC.SelectObject(wxNullBitmap);
    }
    
    wxBitmap myGroupBmp (*_img_tree_folder_on);
    wxBitmap myGroupBmpOff (*_img_tree_folder_off);
    
    images->Add(myUnCheckBmp);
    images->Add(myCheckBmp);
    images->Add(myGroupBmp);
    images->Add(myGroupBmpOff);
    m_Tree->AssignImageList(images);
}



void vrViewerTOCTree::_SetItemImageUnique(wxTreeItemId item, vrVIEWERTOC_IMAGES_TYPES image) {
    m_Tree->SetItemImage(item, image, wxTreeItemIcon_Normal);
    m_Tree->SetItemImage(item, image, wxTreeItemIcon_Selected);
}



void vrViewerTOCTree::_FillTreeList(wxTreeItemId root, wxArrayTreeItemIds & array) {
    wxTreeItemIdValue cookie;
	wxTreeItemId myItem = m_Tree->GetFirstChild( root, cookie );
    
	while( myItem.IsOk() ){
        array.Add(myItem);
		if( m_Tree->ItemHasChildren( myItem ) ){
            _FillTreeList(myItem, array);
		}
		myItem = m_Tree->GetNextChild( root, cookie);
	}
}


void vrViewerTOCTree::_CopyTreeItems(wxTreeItemId origin, wxTreeItemId destination, bool isRoot) {
	wxTreeItemId myDestination = destination;
    if (isRoot == true) {
        // first loop, no recursivity
        vrViewerTOCTreeData * myDataOrigin = (vrViewerTOCTreeData*) m_Tree->GetItemData(origin);
        vrViewerTOCTreeData * myDataCopy = new vrViewerTOCTreeData(*myDataOrigin);
        myDestination = m_Tree->AppendItem(destination, m_Tree->GetItemText(origin),
                                           m_Tree->GetItemImage(origin), -1,
                                           myDataCopy);
    }
    
    wxTreeItemIdValue cookie;
    wxTreeItemId myItem =  m_Tree->GetFirstChild( origin, cookie );;
    
	while( myItem.IsOk() ){
        vrViewerTOCTreeData * myDataOrigin = (vrViewerTOCTreeData*) m_Tree->GetItemData(myItem);
        vrViewerTOCTreeData * myDataCopy = new vrViewerTOCTreeData(*myDataOrigin);
        wxTreeItemId myCopyItem =  m_Tree->AppendItem(myDestination, m_Tree->GetItemText(myItem),
                                                      m_Tree->GetItemImage(myItem), -1,
                                                      myDataCopy);
        
		if( m_Tree->ItemHasChildren( myItem ) ){
            _CopyTreeItems(myItem, myCopyItem, false);
		}
		myItem = m_Tree->GetNextChild( origin, cookie);
	}
}



void vrViewerTOCTree::_MoveLayer (wxTreeItemId origin, wxTreeItemId destination){
    vrViewerTOCTreeData * myDataOrigin = (vrViewerTOCTreeData*) m_Tree->GetItemData(origin);
    vrViewerTOCTreeData * myDataCopy = new vrViewerTOCTreeData(*myDataOrigin);

    wxTreeItemId myDestParent = m_Tree->GetItemParent(destination);
    wxTreeItemId myTarget = m_Tree->GetPrevSibling(destination);
    if (myTarget.IsOk() == false) {
        myTarget = 0;
    }
    wxASSERT(myDestParent.IsOk());
    // insert item before destination
    wxTreeItemId myNewId =  m_Tree->InsertItem(myDestParent, myTarget,
                                               m_Tree->GetItemText(origin),
                                               m_Tree->GetItemImage(origin),
                                               -1,
                                               myDataCopy);
    m_Tree->SelectItem(myNewId);
    //_CopyTreeItems(origin, myNewId);
    
    _ReorderFromTree(myNewId);
}



void vrViewerTOCTree::_SetVisible(wxTreeItemId item, bool visible) {
    vrViewerTOCTreeData * myData = (vrViewerTOCTreeData*) m_Tree->GetItemData(item);
    wxASSERT(myData);
    
    int myImgOffset = 0;
    if (visible == false) {
        myImgOffset = 1;
    }
    int myImageIndex = (int) myData->m_CheckedImgType + myImgOffset;
    _SetItemImageUnique(item, (vrVIEWERTOC_IMAGES_TYPES) myImageIndex);
    
    
    if (myData->m_ItemType == vrTREEDATA_TYPE_LAYER) {
        int myItemIndex = _TreeToIndex(item, vrTREEDATA_TYPE_LAYER);
        wxASSERT(myItemIndex != wxNOT_FOUND);
        GetViewerLayerManager()->GetRenderer(myItemIndex)->SetVisible(visible);
    }
}



bool vrViewerTOCTree::_IsVisible(wxTreeItemId item) {
    vrViewerTOCTreeData * myData = (vrViewerTOCTreeData*) m_Tree->GetItemData(item);
    wxASSERT(myData);
    if (m_Tree->GetItemImage(item) - myData->m_CheckedImgType > 0) {
        return false;
    }
    return true;
}



void vrViewerTOCTree::_ReorderFromTree(wxTreeItemId moveditem) {
    // reorder items after a DND operation. items before moveditem should be moved!
    wxArrayTreeItemIds myIdList;
    _FillTreeList(m_RootNode, myIdList);
    int myLayerIndex = 0;
    for (unsigned int i = 0; i< myIdList.GetCount(); i++) {
        vrViewerTOCTreeData * myData = (vrViewerTOCTreeData*) m_Tree->GetItemData(myIdList.Item(i));
        wxASSERT(myData);
        if (myData->m_ItemType != vrTREEDATA_TYPE_LAYER) {
            continue;
        }
        wxString myLayerName = m_Tree->GetItemText(myIdList.Item(i));
        int iViewerLMIndex = _SearchIntoViewerLayerManager(myLayerName);
        if (iViewerLMIndex == wxNOT_FOUND) {
            wxLogError(_("Error layer '%s' not found into ViewerLayerMaanger!"),myLayerName);
            continue;
        }
        
        if (iViewerLMIndex != myLayerIndex) {
            GetViewerLayerManager()->Move(iViewerLMIndex, myLayerIndex);
        }
        myLayerIndex ++;
        
        wxTreeItemId myActualId = myIdList.Item(i);
        if (myActualId == moveditem) {
            break;
        }
    }
    
}


int vrViewerTOCTree::_SearchIntoViewerLayerManager(const wxString & layername){
    for (int i = 0; i< GetViewerLayerManager()->GetCount(); i++) {
        wxString myActualName = GetViewerLayerManager()->GetRenderer(i)->GetLayer()->GetDisplayName().GetFullName();
        if (myActualName == layername) {
            return i;
        }
    }  
    return wxNOT_FOUND;
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
                
                bool visible = _IsVisible(clickedid);
                _SetVisible(clickedid, !visible);
                SymbologyModified();
                ReloadData();
            }   
                break;
                
                
                
            case vrTREEDATA_TYPE_GROUP:
            {
                bool visible = _IsVisible(clickedid);
                _SetVisible(clickedid, !visible);
                
                wxArrayTreeItemIds myIds;
                _FillTreeList(clickedid, myIds);
                for (unsigned int i = 0; i< myIds.GetCount(); i++) {
                    _SetVisible(myIds.Item(i), !visible);
                }
                SymbologyModified();
                ReloadData();
            }
                break;
                
                
                
            default:
                wxLogError(_("Case not supported!"));
                break;
        }
        return;
	}
	event.Skip();
}



void vrViewerTOCTree::OnItemRightDown(wxTreeEvent & event) {
	m_Tree->SelectItem(event.GetItem());
    vrViewerTOCTreeData * myItemData = (vrViewerTOCTreeData*) m_Tree->GetItemData(event.GetItem());
    wxASSERT(myItemData);
    vrRenderer * myRenderer = NULL;
    if (myItemData->m_ItemType == vrTREEDATA_TYPE_LAYER) {
        int myItemIndex = _TreeToIndex(event.GetItem(), vrTREEDATA_TYPE_LAYER);
        myRenderer = GetViewerLayerManager()->GetRenderer(myItemIndex);
        wxASSERT(myRenderer);
    }
    
    wxMenu * myMenu = CreateContextualMenu(myRenderer, m_UseGroupMenu);
    wxASSERT(myMenu);
    GetControl()->PopupMenu(myMenu);
    event.Skip();
}



void vrViewerTOCTree::OnSetColorPen(wxCommandEvent & event) {
    int mySelItem = _TreeToIndex(m_Tree->GetSelection(), vrTREEDATA_TYPE_LAYER);
    if (SetColorPen(mySelItem) == true) {
        ReloadData();
    }
}



void vrViewerTOCTree::OnSetColorBrush(wxCommandEvent & event) {
    int mySelItem = _TreeToIndex(m_Tree->GetSelection(), vrTREEDATA_TYPE_LAYER);
	if (SetColorBrush(mySelItem) == true) {
		ReloadData();
	}
}



void vrViewerTOCTree::OnSetBrushStyle (wxCommandEvent & event){
    int mySelItem = _TreeToIndex(m_Tree->GetSelection(), vrTREEDATA_TYPE_LAYER);
	if (SetBrushStyle(mySelItem, event.GetId()) == true) {
		ReloadData();
	}
}



void vrViewerTOCTree::OnSetWidth(wxCommandEvent & event) {
    int mySelItem = _TreeToIndex(m_Tree->GetSelection(), vrTREEDATA_TYPE_LAYER);
    if (SetWidth(mySelItem)==true) {
        ReloadData();
    }
}



void vrViewerTOCTree::OnSetTransparency(wxCommandEvent & event) {
    // apply transparency recursively
    wxTreeItemId mySelectedId = m_Tree->GetSelection();
    wxArrayTreeItemIds myListIds;
    myListIds.Add(mySelectedId);
    vrViewerTOCTreeData * myItemData = (vrViewerTOCTreeData*) m_Tree->GetItemData(mySelectedId);
    wxASSERT(myItemData);
    int myActualTransparancy = 0;
    if (myItemData->m_ItemType == vrTREEDATA_TYPE_GROUP) {
        _FillTreeList(mySelectedId, myListIds);
    }
    else {
        int myLayerUniqueIndex = _TreeToIndex(myListIds.Item(0), vrTREEDATA_TYPE_LAYER);
        wxASSERT(myLayerUniqueIndex != wxNOT_FOUND);
        vrRender * myRender = GetViewerLayerManager()->GetRenderer(myLayerUniqueIndex)->GetRender();
        wxASSERT(myRender);
        myActualTransparancy = myRender->GetTransparency();
    }
    
    // ask transparency 
    wxNumberEntryDialog myNumDlg(GetControl(),
                                 "Adjust the transparency percent\n0 is fully opaque, 100 is fully transparent",
                                 "Transparency percent:",
                                 "Adjust Opacity",
                                 myActualTransparancy,
                                 0,
                                 100);
    if (myNumDlg.ShowModal()!=wxID_OK){
        return;
    }
    
    for (unsigned int i = 0; i< myListIds.GetCount(); i++) {
       int myLayerIndex = _TreeToIndex(myListIds.Item(i), vrTREEDATA_TYPE_LAYER);
        if (myLayerIndex == wxNOT_FOUND) {
            continue;
        }
        vrRender * myRender = GetViewerLayerManager()->GetRenderer(myLayerIndex)->GetRender();
        wxASSERT(myRender);
        myRender->SetTransparency(myNumDlg.GetValue());
    }
    SymbologyModified();
    ReloadData();
}





void vrViewerTOCTree::OnNewGroup(wxCommandEvent & event) {
    wxString myGroupNameOrigin = _("New group");
    wxString myGroupName = myGroupNameOrigin;
    int index = 1;
    while (_IndexToTree(m_RootNode, myGroupName, vrTREEDATA_TYPE_GROUP).IsOk() == true) {
        myGroupName = myGroupNameOrigin + wxString::Format(_T(" %d"), index);
        index++;
    }
    AddGroup(myGroupName);
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
    if(myDataStop == NULL){
        return;
    }
    
    // drag into group
    if (myDataStop->m_ItemType == vrTREEDATA_TYPE_GROUP) {
        _CopyTreeItems(myItemStart, myItemStop);
        //_ReorderFromTree(myItemStop);
        m_Tree->Delete(myItemStart);
    }
    
    if (myDataStop->m_ItemType == vrTREEDATA_TYPE_LAYER) {
        if (myItemStop == m_Tree->GetNextSibling(myItemStart)) {
            if (m_AllreadyFreezed == false) {
                FreezeEnd();
            }
            return;
        }
        _MoveLayer(myItemStart, myItemStop);
        m_Tree->Delete(myItemStart);
    }
    
    SymbologyModified();
    if (m_AllreadyFreezed == false) {
        FreezeEnd();
    }
}



void vrViewerTOCTree::OnEditStart(wxTreeEvent & event) {
    vrViewerTOCTreeData * myData = (vrViewerTOCTreeData*) m_Tree->GetItemData(event.GetItem());
    wxASSERT(myData);
    
    if (myData->m_ItemType != vrTREEDATA_TYPE_GROUP) {
        event.Veto();
        return;
    }
    wxLogMessage("Starting edition");
    event.Allow();
    event.Skip();
}



vrViewerTOCTree::vrViewerTOCTree(wxWindow * parent, wxWindowID id,
                                 const wxPoint & pos, const wxSize & size,
                                 long style) {
    m_Tree = new wxTreeCtrl(parent, id, pos, size, style);
    m_RootNode = m_Tree->AddRoot("Project");
    SetUseGroupMenu(false);
    _InitBitmapList();
    
    // connect event
    m_Tree->Bind(wxEVT_LEFT_DOWN, &vrViewerTOCTree::OnMouseDown, this);
    m_Tree->Bind(wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, &vrViewerTOCTree::OnItemRightDown, this);
    m_Tree->Bind(wxEVT_COMMAND_TREE_BEGIN_DRAG, &vrViewerTOCTree::OnDragStart, this);
    m_Tree->Bind(wxEVT_COMMAND_TREE_END_DRAG, &vrViewerTOCTree::OnDragStop, this);
    m_Tree->Bind(wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, &vrViewerTOCTree::OnEditStart, this);
    
    m_Tree->Bind(wxEVT_COMMAND_MENU_SELECTED, &vrViewerTOCTree::OnNewGroup, this, vrID_POPUP_GROUP_ADD);
    m_Tree->Bind(wxEVT_COMMAND_MENU_SELECTED, &vrViewerTOCTree::OnSetTransparency, this, vrID_POPUP_TRANSPARENCY);
    m_Tree->Bind(wxEVT_COMMAND_MENU_SELECTED, &vrViewerTOCTree::OnSetColorPen, this, vrID_POPUP_PEN_COLOR);
    m_Tree->Bind(wxEVT_COMMAND_MENU_SELECTED, &vrViewerTOCTree::OnSetWidth, this, vrID_POPUP_DRAWING_WIDTH);
    m_Tree->Bind(wxEVT_COMMAND_MENU_SELECTED, &vrViewerTOCTree::OnSetColorBrush, this, vrID_POPUP_BRUSH_COLOR);
    m_Tree->Bind(wxEVT_COMMAND_MENU_SELECTED, &vrViewerTOCTree::OnSetBrushStyle, this, vrID_POPUP_BRUSH_SOLID, vrID_POPUP_BRUSH_BDIAGONAL);

}



vrViewerTOCTree::~vrViewerTOCTree() {
    // disconnect event
    m_Tree->Unbind(wxEVT_LEFT_DOWN, &vrViewerTOCTree::OnMouseDown, this);
    m_Tree->Unbind(wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, &vrViewerTOCTree::OnItemRightDown, this);
    m_Tree->Unbind(wxEVT_COMMAND_TREE_BEGIN_DRAG, &vrViewerTOCTree::OnDragStart, this);
    m_Tree->Unbind(wxEVT_COMMAND_TREE_END_DRAG, &vrViewerTOCTree::OnDragStop, this);
    m_Tree->Unbind(wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, &vrViewerTOCTree::OnEditStart, this);

    m_Tree->Unbind(wxEVT_COMMAND_MENU_SELECTED, &vrViewerTOCTree::OnNewGroup, this, vrID_POPUP_GROUP_ADD);
    m_Tree->Unbind(wxEVT_COMMAND_MENU_SELECTED, &vrViewerTOCTree::OnSetTransparency, this, vrID_POPUP_TRANSPARENCY);
    m_Tree->Unbind(wxEVT_COMMAND_MENU_SELECTED, &vrViewerTOCTree::OnSetColorPen, this, vrID_POPUP_PEN_COLOR);
    m_Tree->Unbind(wxEVT_COMMAND_MENU_SELECTED, &vrViewerTOCTree::OnSetWidth, this, vrID_POPUP_DRAWING_WIDTH);
    m_Tree->Unbind(wxEVT_COMMAND_MENU_SELECTED, &vrViewerTOCTree::OnSetColorBrush, this, vrID_POPUP_BRUSH_COLOR);
    m_Tree->Unbind(wxEVT_COMMAND_MENU_SELECTED, &vrViewerTOCTree::OnSetBrushStyle, this, vrID_POPUP_BRUSH_SOLID, vrID_POPUP_BRUSH_BDIAGONAL);
    
    wxDELETE(m_Tree);
}



bool vrViewerTOCTree::Add(int index, vrRenderer * renderer) {
    vrViewerTOCTreeData * myData = new vrViewerTOCTreeData();
    myData->m_ItemType = vrTREEDATA_TYPE_LAYER;
    myData->m_CheckedImgType = vrVIEWERTOC_IMAGE_CHECKED;
    int myImgStatus = vrVIEWERTOC_IMAGE_CHECKED;
    if (renderer->GetVisible() == false) {
        myImgStatus = vrVIEWERTOC_IMAGE_UNCHECKED;
    }
    
    if (index >= (signed) m_Tree->GetCount()) {
		wxTreeItemId myAddedId = m_Tree->AppendItem(m_RootNode, renderer->GetLayer()->GetDisplayName().GetFullName(),myImgStatus, -1, myData);
		return true;
	}
    
	// if index is -1, insert at the begining.
	if (index == -1) {
		index = 0;
	}
    
    wxTreeItemId myAddedId = m_Tree->InsertItem(m_RootNode, index, renderer->GetLayer()->GetDisplayName().GetFullName(),myImgStatus,-1,myData);
    //m_Tree->SetItemState(myAddedId, wxTREE_ITEMSTATE_NONE);
    
    return true;
}



bool vrViewerTOCTree::Move(long oldpos, long newpos) {
    // do nothing, move is made with drag'n'drop
    return true;
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
    myData->m_CheckedImgType = vrVIEWERTOC_IMAGE_GROUP_ON;
    
    m_Tree->AppendItem(m_RootNode, name, myData->m_CheckedImgType, -1, myData);
    return true;
}


