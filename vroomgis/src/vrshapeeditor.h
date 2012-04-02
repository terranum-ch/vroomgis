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

class vrShapeEditor {
protected:
    OGRGeometry * m_Geometry;
    vrViewerDisplay * m_Display;
	
	
public:
    vrShapeEditor(vrViewerDisplay * display);
    virtual ~vrShapeEditor();
	
    virtual bool AddVertex(const wxPoint2DDouble & point){return false;}
    virtual bool RemoveVertex(int index = wxNOT_FOUND){return false;}
	
    inline OGRGeometry * GetGeometryRef() const;
    virtual void DrawShapeEdit(vrRender * render){;}
	virtual void DrawShapeFinish(vrRender * render){;}
};



inline OGRGeometry * vrShapeEditor::GetGeometryRef() const {
	return m_Geometry;
}



class vrShapeEditorPoint : public vrShapeEditor {
public:
    vrShapeEditorPoint(vrViewerDisplay * display);
    virtual ~vrShapeEditorPoint();
	
    virtual bool AddVertex(const wxPoint2DDouble & point);
    virtual void DrawShapeFinish(vrRender * render);
	
};




/*************************************************************************************//**
@brief Drawing temporary shape
@author Lucien Schreiber copyright CREALP
@date 30 mars 2012
*****************************************************************************************/
class vrShapeEditorLine : public vrShapeEditor {
protected:
    wxOverlay m_Overlay;
    
public:
    vrShapeEditorLine(vrViewerDisplay * display);
    virtual ~vrShapeEditorLine();
    
    virtual bool AddVertex(const wxPoint2DDouble & point);
    virtual void DrawShapeEdit(vrRender * render);
    virtual void DrawShapeFinish(vrRender * render){;}

};


#endif

