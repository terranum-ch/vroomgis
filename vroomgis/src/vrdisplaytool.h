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
#include "ogrsf_frmts.h"
class vrCoordinate;

class vrViewerDisplay;
class vrRubberBand;
class vrViewerLayerManager;


enum vrMOUSE_STATUS {
	vrMOUSE_UNKNOWN = -1,
    vrMOUSE_UP = 0,
	vrMOUSE_DOWN,
	vrMOUSE_MOVE,
};



/***************************************************************************//**
@brief Used for passing message from tool to application
@author Lucien Schreiber (c) CREALP 2010
@date 20 avril 2010
*******************************************************************************/
class vrDisplayToolMessage {
private:
    void _InitMembers();


public:
    wxRect m_rect;
	wxPoint m_position;
    wxEventType m_EvtType;
    vrMOUSE_STATUS m_mouseStatus;
	vrViewerLayerManager * m_parentManager;
    long m_longData;


    vrDisplayToolMessage(const wxEventType & eventtype,
						 vrViewerDisplay * parent, const wxRect & rect, 
                         vrMOUSE_STATUS mousestat = vrMOUSE_UNKNOWN);
    vrDisplayToolMessage(const wxEventType & eventtype,
						 vrViewerDisplay * parent, const wxPoint & pos, 
                         vrMOUSE_STATUS mousestat = vrMOUSE_UNKNOWN);
    virtual ~vrDisplayToolMessage();
    
};



/***************************************************************************//**
@brief Base tool class
@author Lucien Schreiber (c) CREALP 2010
@date 20 avril 2010
*******************************************************************************/
class vrDisplayTool {
private:
    wxString m_name;
    int m_iD;
    wxCursor m_cursor;
    vrViewerDisplay * m_display;


protected:
	vrRubberBand * m_rubber;
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
    virtual bool MouseDClickLeft(const wxMouseEvent & event);

	void SendMessage(vrDisplayToolMessage * message);

};

inline vrViewerDisplay * vrDisplayTool::GetDisplay() const {
	return m_display;
}

inline const wxString vrDisplayTool::GetName() const {
  return m_name;
}

inline const int vrDisplayTool::GetID() const {
  return m_iD;
}

inline const wxCursor vrDisplayTool::GetCursor() const {
  return m_cursor;
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
@brief Selection tool
@author Lucien Schreiber (c) CREALP 2010
@date 28 september 2010
*******************************************************************************/
class vrDisplayToolSelect : public vrDisplayTool {
public:
    vrDisplayToolSelect(vrViewerDisplay * display);
    virtual ~vrDisplayToolSelect();

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
@brief Default Zoom out tool
@author Pascal Horton (c) UNIL 2011
@date 27 avril 2011
*******************************************************************************/
class vrDisplayToolZoomOut : public vrDisplayToolZoom {
  public:
    vrDisplayToolZoomOut(vrViewerDisplay * display);
    virtual ~vrDisplayToolZoomOut();

    virtual bool MouseUp(const wxMouseEvent & event);

};




/***************************************************************************//**
@brief Default pan tool
@author Lucien Schreiber (c) CREALP 2010
@date 28 avril 2010
 *******************************************************************************/
class vrDisplayToolPan : public vrDisplayTool {
private:
    wxPoint m_point;
	wxBitmap * m_panBitmap;

public:
    vrDisplayToolPan(vrViewerDisplay * display);
    virtual ~vrDisplayToolPan();

    virtual bool MouseDown(const wxMouseEvent & event);
    virtual bool MouseUp(const wxMouseEvent & event);
    virtual bool MouseMove(const wxMouseEvent & event);
    virtual bool MouseDClickLeft(const wxMouseEvent & event);


};


/***************************************************************************//**
@brief Default viewfinder tool
@author Lucien Schreiber (c) CREALP 2010
@date 29 avril 2010
 *******************************************************************************/
class vrDisplayToolSight : public vrDisplayTool {
private:
   wxOverlay m_overlay;

public:
    vrDisplayToolSight(vrViewerDisplay * display);
    virtual ~vrDisplayToolSight();

    virtual bool MouseDown(const wxMouseEvent & event);
    virtual bool MouseUp(const wxMouseEvent & event);
    virtual bool MouseMove(const wxMouseEvent & event);

};




/*************************************************************************************//**
@brief Generic editing tool
@author Lucien Schreiber copyright CREALP
@date 30 mars 2012
 *****************************************************************************************/
class vrDisplayToolEdit : public vrDisplayTool {
private:
    wxOverlay m_overlay;
    
public:
    vrDisplayToolEdit(vrViewerDisplay * display, int toolid = wxID_EDIT);
    virtual ~vrDisplayToolEdit();
    
    virtual bool MouseDown(const wxMouseEvent & event){return false;}
    virtual bool MouseUp(const wxMouseEvent & event);
    virtual bool MouseMove(const wxMouseEvent & event){return false;}

};




/*************************************************************************************//**
@brief Line and polygons editing tool
@author Lucien Schreiber copyright CREALP
@date 30 mars 2012
*****************************************************************************************/
class vrDisplayToolEditLine : public vrDisplayToolEdit {
private:
    wxPoint m_previousPoint;
    wxOverlay m_overlay;
    bool m_doubleClicked;
    
public:
    vrDisplayToolEditLine(vrViewerDisplay * display, int toolid = wxID_EDIT);
    virtual ~vrDisplayToolEditLine();
    
    virtual bool MouseDown(const wxMouseEvent & event);
    virtual bool MouseUp(const wxMouseEvent & event);
    virtual bool MouseMove(const wxMouseEvent & event);
    virtual bool MouseDClickLeft(const wxMouseEvent & event);
};





/***************************************************************************//**
@brief Points, lines, polygons, modification tool
@author Lucien Schreiber (c) CREALP 2012
@date 01 mai 2012
*******************************************************************************/
class vrDisplayToolModify : public vrDisplayTool {
protected:
    wxArrayInt m_pointsX;
    wxArrayInt m_pointsY;
    wxOverlay m_overlay;
    int m_activeVertex;
    OGRwkbGeometryType m_geometryType;
    
public:
    vrDisplayToolModify(vrViewerDisplay * display);
    virtual ~vrDisplayToolModify();
    
    void ClearPoints();
    void AddToPoints(const wxPoint & pts);
    bool SetActiveGeometry(OGRGeometry * geometry, OGRwkbGeometryType geometrytype, vrCoordinate * coordinate);
    
    virtual bool MouseDown(const wxMouseEvent & event);
    virtual bool MouseMove(const wxMouseEvent & event);
    virtual bool MouseUp(const wxMouseEvent & event);
};




#endif
