/***************************************************************************
 vrshapeeditor.h
 
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

#ifndef _VRSHAPEEDITOR_H_
#define _VRSHAPEEDITOR_H_

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
// Include wxWidgets' headers
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/graphics.h>
#include <wx/overlay.h>

#include "ogrsf_frmts.h"

class vrViewerDisplay;
class vrRender;



enum vrSHAPEEDITOR_TYPES {
    vrSHAPEEDITOR_TYPE_POINT = 0,
    vrSHAPEEDITOR_TYPE_LINE,
    vrSHAPEEDITOR_TYPE_POLYGON,
    vrSHAPEEDITOR_TYPE_UNKHNOWN = 100
};


class vrShapeEditor {
protected:
    OGRGeometry * m_Geometry;
    vrViewerDisplay * m_Display;
    vrSHAPEEDITOR_TYPES m_Type;

	
public:
    vrShapeEditor(vrViewerDisplay * display);
    virtual ~vrShapeEditor();
    inline const vrSHAPEEDITOR_TYPES GetType() const;

	
    virtual bool AddVertex(const wxPoint2DDouble & point){return false;}
    virtual bool RemoveVertex(int index = wxNOT_FOUND){return false;}
	
    virtual OGRGeometry * GetGeometryRef();
    bool SetGeometry(OGRGeometry * geometry);
    
    virtual void ViewChanged(){;}
    
    virtual void DrawShapeEdit(vrRender * render){;}
	virtual void DrawShapeFinish(vrRender * render){;}
    virtual void DrawShapeModify(vrRender * render);
};


inline const vrSHAPEEDITOR_TYPES vrShapeEditor::GetType() const {
    return m_Type;
}




/*************************************************************************************//**
@brief Draw temporarily points
@author Lucien Schreiber copyright CREALP
@date 30 mars 2012
*****************************************************************************************/
class vrShapeEditorPoint : public vrShapeEditor {
public:
    vrShapeEditorPoint(vrViewerDisplay * display);
    virtual ~vrShapeEditorPoint();
	
    virtual bool AddVertex(const wxPoint2DDouble & point);
    virtual void DrawShapeFinish(vrRender * render);

};




/*************************************************************************************//**
@brief Draw temporarily lines
@author Lucien Schreiber copyright CREALP
@date 30 mars 2012
*****************************************************************************************/
class vrShapeEditorLine : public vrShapeEditor {
protected:
    wxOverlay m_Overlay;
    
public:
    vrShapeEditorLine(vrViewerDisplay * display);
    virtual ~vrShapeEditorLine();
    
    virtual void ViewChanged();
    
    virtual bool AddVertex(const wxPoint2DDouble & point);
    virtual void DrawShapeEdit(vrRender * render);
};



/*************************************************************************************//**
@brief Draw temporarily polygons
@author Lucien Schreiber copyright CREALP
@date 02 avril 2012
*****************************************************************************************/
class vrShapeEditorPolygon : public vrShapeEditorLine {
public:
    vrShapeEditorPolygon(vrViewerDisplay * display);
    virtual ~vrShapeEditorPolygon();
    
    virtual void ViewChanged();
    
    virtual OGRGeometry * GetGeometryRef();
};

#endif

