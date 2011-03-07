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
	wxArrayLong m_SelectedIDs;


	wxPoint _GetPointFromReal(const wxPoint2DDouble & realpt, 
							  const wxPoint2DDouble & origin, double pxsize);

public:
	vrLayerVector();
    virtual ~vrLayerVector();
    virtual bool Create(const wxFileName & filename){return false;}
    virtual bool Open(const wxFileName & filename, bool readwrite = false){return false;}
	
	virtual bool GetExtent(vrRealRect & rect){return false;}
	
	OGRFeature * GetFeature(long fid);
    long GetFeatureCount(bool onlyvisible = true);
    OGRFeature * GetNextFeature(bool restart);
	virtual bool MoveToFeatureIndex(long index);
	OGRwkbGeometryType GetGeometryType();
	void ClearSpatialFilter();


	virtual long AddFeature(OGRGeometry * geometry, void * data = NULL){return wxNOT_FOUND;}
	virtual bool DeleteFeature(long fid);
	bool IsOK();
	
	inline wxArrayLong * GetSelectedIDs();
    void SetSelectedIDs(const wxArrayLong & value);
    virtual bool SearchFeatures(OGRGeometry * geometry, wxArrayLong & results);
	bool IsFeatureSelected(long id);

	
};

inline wxArrayLong * vrLayerVector::GetSelectedIDs() {
	return &m_SelectedIDs;
}


class vrLayerVectorOGR : public vrLayerVector {
protected:
	virtual bool _DrawLines(wxGraphicsContext * gdc, const wxRect2DDouble & coord,
							const vrRender * render, const vrLabel * label, double pxsize);
	virtual bool _DrawPoints(wxGraphicsContext * gdc, const wxRect2DDouble & coord,
							 const vrRender * render, const vrLabel * label, double pxsize);
    virtual bool _DrawPolygons(wxGraphicsContext * gdc, const wxRect2DDouble & coord,
							   const vrRender * render, const vrLabel * label, double pxsize);	
    virtual bool _Close();
	
public:
	vrLayerVectorOGR();
    virtual ~vrLayerVectorOGR();
	
    virtual bool Open(const wxFileName & filename, bool readwrite = false);
    virtual bool Create(const wxFileName & filename, int spatialtype = wkbPoint);
	
    bool AddField(const OGRFieldDefn & fielddef);
    virtual long AddFeature(OGRGeometry * geometry, void * data = NULL);

    
	virtual bool GetExtent(vrRealRect & rect);
	virtual bool GetData(wxImage * bmp, const vrRealRect & coord,  double pxsize,
						 const vrRender * render = NULL, const vrLabel * label = NULL);

    bool SetAttributeFilter(const wxString & query);

	
};
#endif
