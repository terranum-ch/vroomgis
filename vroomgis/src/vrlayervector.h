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

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <wx/dcgraph.h>
#include <wx/graphics.h>

#include "vrlayer.h"

class vrLayerVector : public vrLayer {
protected:
    OGRDataSource * m_dataset;
	OGRLayer * m_layer;
	OGRwkbGeometryType m_geometryType;
	wxArrayLong m_selectedIDs;
    wxArrayLong m_hiddenObjectID;


	long m_objectDrawn;
    long m_skippedVertex;
    long m_drawnVertex;

    bool _Intersects(const wxRect2DDouble & myPathRect, const wxRect2DDouble & myWndRect);

	wxPoint _GetPointFromReal(const wxPoint2DDouble & realpt,
							  const wxPoint2DDouble & origin, double pxsize);

public:
	vrLayerVector();
    virtual ~vrLayerVector();
    virtual bool Create(const wxFileName & filename){return false;}
    virtual bool Open(const wxFileName & filename, bool readwrite = false){return false;}

	virtual wxFileName GetDisplayName();
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
    virtual bool HasData ();

    bool SetFeature(OGRFeature * feature);

	inline wxArrayLong * GetSelectedIDs();
    void SetSelectedIDs(const wxArrayLong & value);
    void SetSelectedID (long id);
    virtual bool SearchFeatures(OGRGeometry * geometry, wxArrayLong & results);
	bool IsFeatureSelected(long id);
    long Select (const vrRealRect & rect);
    
    inline wxArrayLong * GetHiddenObjectID();
    void SetHiddenObjectID(const wxArrayLong & value);
    void SetHiddenObjectID(long id);
    bool IsFeatureHidden(long id);

	inline const long GetObjectDrawn() const;
    inline OGRLayer * GetLayerRef();
    inline const long GetVertexSkipped() const;
    inline const long GetVertexDrawn() const;
};

inline wxArrayLong * vrLayerVector::GetSelectedIDs() {
	return &m_selectedIDs;
}


inline OGRLayer * vrLayerVector::GetLayerRef(){
    wxASSERT(m_layer);
    return m_layer;
}


inline wxArrayLong * vrLayerVector::GetHiddenObjectID() {
    return &m_hiddenObjectID;
}


inline const long vrLayerVector::GetObjectDrawn() const {
	return m_objectDrawn;
}


inline const long vrLayerVector::GetVertexDrawn() const {
	return m_drawnVertex;
}


inline const long vrLayerVector::GetVertexSkipped() const {
	return m_skippedVertex;
}



class vrLayerVectorOGR : public vrLayerVector {
protected:

    virtual void _DrawPoint(wxDC * dc, OGRFeature * feature, OGRGeometry * geometry, const wxRect2DDouble & coord, const vrRender * render,  vrLabel * label, double pxsize);
    virtual void _DrawLine(wxDC * dc, OGRFeature * feature, OGRGeometry * geometry, const wxRect2DDouble & coord, const vrRender * render,  vrLabel * label, double pxsize);
    virtual void _DrawPolygon(wxDC * dc, OGRFeature * feature, OGRGeometry * geometry, const wxRect2DDouble & coord, const vrRender * render,  vrLabel * label, double pxsize);

    virtual bool _Close();

public:
	vrLayerVectorOGR();
    virtual ~vrLayerVectorOGR();

    virtual bool Open(const wxFileName & filename, bool readwrite = false);
    virtual bool Create(const wxFileName & filename, int spatialtype = wkbPoint);
    virtual bool CopyLayer (vrLayerVectorOGR * layer, const wxString & newlayername = wxEmptyString);

    bool AddField(OGRFieldDefn & fielddef);
    virtual long AddFeature(OGRGeometry * geometry, void * data = NULL);

	virtual bool GetExtent(vrRealRect & rect);
	virtual bool GetData(wxBitmap * bmp, const vrRealRect & coord, double pxsize,const vrRender * render = NULL, vrLabel * label = NULL);
    
    bool SetAttributeFilter(const wxString & query);
    
private:
    wxPoint m_previousPoint;
    
};
#endif
