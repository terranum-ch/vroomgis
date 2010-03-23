/***************************************************************************
				vrlayervector.h
                    
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

#ifndef _VRLAYERVECTOR_H
#define _VRLAYERVECTOR_H

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
// Include wxWidgets' headers
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif


#include "vrlayer.h"
#include <wx/graphics.h>

class vrLayerVector : public vrLayer {
protected:
    OGRDataSource * m_Dataset;
	OGRLayer * m_Layer;
	OGRwkbGeometryType m_GeometryType;

	wxPoint _GetPointFromReal(const wxPoint2DDouble & realpt, 
							  const wxPoint2DDouble & origin, double pxsize);

public:
	vrLayerVector();
    virtual ~vrLayerVector();
    virtual bool Create(const wxFileName & filename){return false;}
    virtual bool Open(const wxFileName & filename, bool readwrite = false){return false;}
	
	virtual bool GetExtent(vrRealRect & rect){return false;}
	
	OGRFeature * GetFeature(long fid);
    OGRFeature * GetNextFeature(bool restart);
	OGRwkbGeometryType GetGeometryType();

	
	bool IsOK();
	
};


class vrLayerVectorOGR : public vrLayerVector {
private:
	virtual bool _DrawLines(wxMemoryDC * mdc, const wxRect2DDouble & coord,
							const vrRender * render, const vrLabel * label, double pxsize);
	
protected:
    virtual bool _Close();
	
public:
	vrLayerVectorOGR();
    virtual ~vrLayerVectorOGR();
	
    virtual bool Open(const wxFileName & filename, bool readwrite = false);
    virtual bool Create(const wxFileName & filename);
	
	virtual bool GetExtent(vrRealRect & rect);
	virtual bool GetData(wxImage * bmp, const vrRealRect & coord,  double pxsize,
						 const vrRender * render = NULL, const vrLabel * label = NULL);


	
};
#endif
