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
    wxArrayLong m_HiddenObjectID;


	long m_ObjectDrawn;
    long m_SkippedVertex;
    long m_DrawnVertex;

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
	return &m_SelectedIDs;
}


inline OGRLayer * vrLayerVector::GetLayerRef(){
    wxASSERT(m_Layer);
    return m_Layer;
}


inline wxArrayLong * vrLayerVector::GetHiddenObjectID() {
    return &m_HiddenObjectID;
}


inline const long vrLayerVector::GetObjectDrawn() const {
	return m_ObjectDrawn;
}


inline const long vrLayerVector::GetVertexDrawn() const {
	return m_DrawnVertex;
}


inline const long vrLayerVector::GetVertexSkipped() const {
	return m_SkippedVertex;
}



class vrLayerVectorOGR : public vrLayerVector {
protected:

    virtual void _DrawPoint(wxGraphicsContext * gdc, OGRFeature * feature, OGRGeometry * geometry, const wxRect2DDouble & coord, const vrRender * render,  vrLabel * label, double pxsize);
    virtual void _DrawLine(wxGraphicsContext * gdc, OGRFeature * feature, OGRGeometry * geometry, const wxRect2DDouble & coord, const vrRender * render,  vrLabel * label, double pxsize);
    virtual void _DrawPolygon(wxGraphicsContext * gdc, OGRFeature * feature, OGRGeometry * geometry, const wxRect2DDouble & coord, const vrRender * render,  vrLabel * label, double pxsize);

    virtual bool _Close();

public:
	vrLayerVectorOGR();
    virtual ~vrLayerVectorOGR();

    virtual bool Open(const wxFileName & filename, bool readwrite = false);
    virtual bool Create(const wxFileName & filename, int spatialtype = wkbPoint);
    virtual bool CopyLayer (vrLayerVectorOGR * layer, const wxString & newlayername = wxEmptyString);

    bool AddField(const OGRFieldDefn & fielddef);
    virtual long AddFeature(OGRGeometry * geometry, void * data = NULL);

	virtual bool GetExtent(vrRealRect & rect);
	virtual bool GetData(wxBitmap * bmp, const vrRealRect & coord, double pxsize,const vrRender * render = NULL, vrLabel * label = NULL);
    
    bool SetAttributeFilter(const wxString & query);
    
private:
    wxPoint m_PreviousPoint;
    
};
#endif
