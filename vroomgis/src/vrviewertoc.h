/***************************************************************************
								vrviewertoc.h
				Default TOC
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
#ifndef _VRVIEWERTOC_H
#define _VRVIEWERTOC_H

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif
#include <wx/treectrl.h>


class vrRenderer;
class vrViewerLayerManager;

// event ID
const int vrID_VIEWERTOC_BASE = 100;
const int vrID_POPUP_REMOVE = wxID_HIGHEST +		1 + vrID_VIEWERTOC_BASE;
const int vrID_POPUP_TRANSPARENCY = wxID_HIGHEST +	2 + vrID_VIEWERTOC_BASE;
const int vrID_POPUP_PEN_COLOR = wxID_HIGHEST +		3 + vrID_VIEWERTOC_BASE;
const int vrID_POPUP_BRUSH_COLOR = wxID_HIGHEST +	4 + vrID_VIEWERTOC_BASE;
const int vrID_POPUP_DRAWING_WIDTH = wxID_HIGHEST +	5 + vrID_VIEWERTOC_BASE;
const int vrID_POPUP_BRUSH_SOLID = wxID_HIGHEST +	6 + vrID_VIEWERTOC_BASE;
const int vrID_POPUP_BRUSH_TRANSPARENT = wxID_HIGHEST +	7 + vrID_VIEWERTOC_BASE;
const int vrID_POPUP_BRUSH_BDIAGONAL = wxID_HIGHEST +	8 + vrID_VIEWERTOC_BASE;





class vrViewerTOC{
private:
    bool m_FreezeStatus;
    vrViewerLayerManager * m_ViewerManager;
    
protected:
    virtual void ShowMenuContextual(vrRenderer * renderer);
    void ReloadData();
    
    inline vrViewerLayerManager * GetViewerLayerManager() const;
    inline const bool IsFreezed() const;
    
    
public:
    vrViewerTOC();    
    virtual ~vrViewerTOC();
    
    virtual bool Add(int index, vrRenderer * renderer) = 0;
    virtual bool Move(long oldpos, long newpos) = 0;
    virtual bool Remove(int index) = 0;
    virtual int GetSelection() = 0;
    
    virtual void FreezeBegin();
    virtual void FreezeEnd();
    
    void SetViewerLayerManager(vrViewerLayerManager * value);
    virtual wxControl * GetControl() = 0;
};



inline vrViewerLayerManager * vrViewerTOC::GetViewerLayerManager() const{
    return m_ViewerManager;
}



inline const bool vrViewerTOC::IsFreezed() const {
    return m_FreezeStatus;
}







/************************************ vrViewerTOCList **********************************************/
class vrViewerTOCList : public vrViewerTOC {
private:
    wxCheckListBox * m_CheckList;
    
    void OnMouseRightDown(wxMouseEvent & event);
    void OnMouseWheel(wxMouseEvent & event);
    void OnSetColorPen(wxCommandEvent & event);
    void OnSetColorBrush(wxCommandEvent & event);
    void OnSetTransparency(wxCommandEvent & event);
    void OnSetWidth(wxCommandEvent & event);
    void OnSetBrushStyle (wxCommandEvent & event);
    void OnVisibleStatusChanged(wxCommandEvent & event);
    
public:
    vrViewerTOCList(wxWindow * parent, wxWindowID id, 
                    const wxPoint & pos = wxDefaultPosition,
                    const wxSize & size = wxDefaultSize,
                    int  n = 0, const wxString choices[] = NULL,
                    long  style = 0);
    virtual ~vrViewerTOCList();
    
    virtual bool Add(int index, vrRenderer * renderer);
    virtual bool Move(long oldpos, long newpos);
    virtual bool Remove(int index);
    virtual int GetSelection();
    virtual wxControl * GetControl();
};



/************************************ vrViewerTOCTreeData ******************************************/
enum vrVIEWERTOC_TREEDATA_TYPES {
    vrTREEDATA_TYPE_INVALID = -1,
    vrTREEDATA_TYPE_LAYER = 0,
    vrTREEDATA_TYPE_LEGEND,
    vrTREEDATA_TYPE_GROUP
};


class vrViewerTOCTreeData : public wxTreeItemData {
public:
    vrViewerTOCTreeData();
    virtual ~vrViewerTOCTreeData();
    vrVIEWERTOC_TREEDATA_TYPES m_ItemType;
};




/************************************ vrViewerTOCTree **********************************************/
class vrViewerTOCTree : public vrViewerTOC {
private:
    wxTreeCtrl  * m_Tree;
    wxTreeItemId m_RootNode;
    
    wxTreeItemId _FindItem(wxTreeItemId root, const wxString & sSearchFor);
    void OnMouseRightDown(wxMouseEvent & event);
    void OnSetColorPen(wxCommandEvent & event);
    void OnSetColorBrush(wxCommandEvent & event);
    void OnSetTransparency(wxCommandEvent & event);
    void OnSetWidth(wxCommandEvent & event);
    virtual void OnVisibleStatusChanged(wxCommandEvent & event);
    
public:
    vrViewerTOCTree(wxWindow * parent, wxWindowID id,
                    const wxPoint & pos = wxDefaultPosition,
                    const wxSize & size = wxDefaultSize,
                    long style = wxTR_DEFAULT_STYLE | wxTR_HIDE_ROOT);
    virtual ~vrViewerTOCTree();
    
    virtual bool Add(int index, vrRenderer * renderer);
    virtual bool Move(long oldpos, long newpos);
    virtual bool Remove(int index);
    virtual int GetSelection();
    virtual wxControl * GetControl();
    
};








#endif
