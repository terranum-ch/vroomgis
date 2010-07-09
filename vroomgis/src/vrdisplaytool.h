/***************************************************************************
 vrdisplaytool.h

 -------------------
 copyright            : (C) 2010 CREALP Lucien Schreiber 
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
#ifndef _VRDISPLAYTOOL_H
#define _VRDISPLAYTOOL_H

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/overlay.h>

class vrViewerDisplay;
class vrRubberBand;
class vrViewerLayerManager;



/***************************************************************************//**
@brief Used for passing message from tool to application
@author Lucien Schreiber (c) CREALP 2010
@date 20 avril 2010
*******************************************************************************/
class vrDisplayToolMessage {
private:
    void _InitMembers();
	
	
public:
    wxRect m_Rect;
	wxPoint m_Position;
    wxEventType m_EvtType;
	vrViewerLayerManager * m_ParentManager;

	
    vrDisplayToolMessage(const wxEventType & eventtype, 
						 vrViewerDisplay * parent, const wxRect & rect);
    vrDisplayToolMessage(const wxEventType & eventtype, 
						 vrViewerDisplay * parent, const wxPoint & pos);
    virtual ~vrDisplayToolMessage();
	
};




/***************************************************************************//**
@brief Base tool class
@author Lucien Schreiber (c) CREALP 2010
@date 20 avril 2010
*******************************************************************************/
class vrDisplayTool {
private:
    wxString m_Name;
    int m_ID;
    wxCursor m_Cursor;
    vrViewerDisplay * m_Display;
	
	
protected:
	vrRubberBand * m_Rubber;
    inline vrViewerDisplay * GetDisplay() const;
	
	
public:
    vrDisplayTool();
    vrDisplayTool(vrViewerDisplay * display, int id, wxString name, wxCursor cursor);
    void Create(vrViewerDisplay * display, int id, wxString name, wxCursor cursor);
    virtual ~vrDisplayTool();
	
    inline const wxString GetName() const;
    inline const int GetID() const;
    inline const wxCursor GetCursor() const;
	
    virtual bool MouseDown(const wxMouseEvent & event);
    virtual bool MouseUp(const wxMouseEvent & event);
    virtual bool MouseMove(const wxMouseEvent & event);
	
	void SendMessage(vrDisplayToolMessage * message);
	
};

inline vrViewerDisplay * vrDisplayTool::GetDisplay() const {
	return m_Display;
}

inline const wxString vrDisplayTool::GetName() const {
  return m_Name;
}

inline const int vrDisplayTool::GetID() const {
  return m_ID;
}

inline const wxCursor vrDisplayTool::GetCursor() const {
  return m_Cursor;
}





/***************************************************************************//**
@brief Default tool
@author Lucien Schreiber (c) CREALP 2010
@date 20 avril 2010
*******************************************************************************/
class vrDisplayToolDefault : public vrDisplayTool {
  public:
    vrDisplayToolDefault(vrViewerDisplay * display);
    virtual ~vrDisplayToolDefault();

	virtual bool MouseDown(const wxMouseEvent & event);
    virtual bool MouseUp(const wxMouseEvent & event);
    virtual bool MouseMove(const wxMouseEvent & event);

};




/***************************************************************************//**
@brief Default Zoom tool
@author Lucien Schreiber (c) CREALP 2010
@date 20 avril 2010
*******************************************************************************/
class vrDisplayToolZoom : public vrDisplayTool {
  public:
    vrDisplayToolZoom(vrViewerDisplay * display);
    virtual ~vrDisplayToolZoom();

	virtual bool MouseDown(const wxMouseEvent & event);
    virtual bool MouseUp(const wxMouseEvent & event);
    virtual bool MouseMove(const wxMouseEvent & event);

};



/***************************************************************************//**
@brief Default pan tool
@author Lucien Schreiber (c) CREALP 2010
@date 28 avril 2010
 *******************************************************************************/
class vrDisplayToolPan : public vrDisplayTool {
private:
    wxPoint m_Point;
	wxBitmap * m_PanBitmap;
	
public:
    vrDisplayToolPan(vrViewerDisplay * display);
    virtual ~vrDisplayToolPan();
	
    virtual bool MouseDown(const wxMouseEvent & event);
    virtual bool MouseUp(const wxMouseEvent & event);
    virtual bool MouseMove(const wxMouseEvent & event);
	
};


/***************************************************************************//**
@brief Default viewfinder tool
@author Lucien Schreiber (c) CREALP 2010
@date 29 avril 2010
 *******************************************************************************/
class vrDisplayToolSight : public vrDisplayTool {
private:
   wxOverlay m_Overlay;
	
public:
    vrDisplayToolSight(vrViewerDisplay * display);
    virtual ~vrDisplayToolSight();
	
    virtual bool MouseDown(const wxMouseEvent & event);
    virtual bool MouseUp(const wxMouseEvent & event);
    virtual bool MouseMove(const wxMouseEvent & event);
	
};




#endif