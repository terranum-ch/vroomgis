/***************************************************************************
					vrlayervector.cpp

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

//

#include "vrlayervector.h"
#include "vrrender.h"


wxPoint vrLayerVector::_GetPointFromReal(const wxPoint2DDouble & realpt,
										 const wxPoint2DDouble & origin, double pxsize) {
	wxPoint myPt;
	myPt.x = wxRound((realpt.m_x - origin.m_x) / pxsize);
	myPt.y = wxRound((origin.m_y - realpt.m_y) / pxsize);
	return myPt;
}

bool vrLayerVector::_Intersects(const wxRect2DDouble & myPathRect, const wxRect2DDouble & myWndRect)
{
    wxDouble left,right,bottom,top;
    left = wxMax ( myPathRect.m_x , myWndRect.m_x );
    right = wxMin ( myPathRect.m_x+myPathRect.m_width, myWndRect.m_x + myWndRect.m_width );
    top = wxMax ( myPathRect.m_y , myWndRect.m_y );
    bottom = wxMin ( myPathRect.m_y+myPathRect.m_height, myWndRect.m_y + myWndRect.m_height );

    // Modification of the conditions from wxRect2DDouble::Intersects to allow horizontal and vertical lines.
    if ( (left < right && top <= bottom) || (left <= right && top < bottom) )
    {
        return true;
    }
    return false;
}

vrLayerVector::vrLayerVector() {
	m_Dataset = NULL;
	m_Layer = NULL;
	m_ObjectDrawn = 0;
    m_SkippedVertex = 0;
    m_DrawnVertex = 0;
}

vrLayerVector::~vrLayerVector() {

}


bool vrLayerVector::IsOK() {
	if (m_Dataset == NULL){
		wxLogError("Dataset not inited");
		return false;
	}
	if (m_Layer == NULL)
	{
		wxLogError("Layer Not inited");
		return false;
	}
	return true;
}


bool vrLayerVector::HasData () {
    if (GetFeatureCount(false) > 0) {
        return  true;
    }
    return false;
}




wxFileName vrLayerVector::GetDisplayName(){
	if (m_DriverType == vrDRIVER_VECTOR_MEMORY) {
		wxFileName myName (m_FileName);
		myName.SetExt(wxEmptyString);
		return myName;
	}

	return m_FileName;
}


OGRFeature * vrLayerVector::GetFeature(long fid) {

	if (IsOK() == false){
		return NULL;
	}

	OGRFeature * myFeature = m_Layer->GetFeature(fid);

	if (myFeature == NULL) {
		wxLogError("Unable to get feature with fid %d", fid);
		return NULL;
	}

	return myFeature;
}


long vrLayerVector::GetFeatureCount(bool onlyvisible) {
    wxASSERT(m_Layer);
    if (onlyvisible == false) {
        ClearSpatialFilter();
    }
    return m_Layer->GetFeatureCount();
}


OGRFeature * vrLayerVector::GetNextFeature(bool restart) {
	if (IsOK() == false) {
		return NULL;
	}

	if (restart == true) {
		m_Layer->ResetReading();
	}


	OGRFeature * myFeature = m_Layer->GetNextFeature();
	if (myFeature == NULL) {
		return NULL;
	}

	return myFeature;
}



bool vrLayerVector::MoveToFeatureIndex(long index) {
	if (IsOK() == false) {
		return false;
	}

	OGRErr myErr = m_Layer->SetNextByIndex(index);
	if (myErr != OGRERR_NONE) {
		wxLogError(_("Error going to index : %ld, (code %d)"),
				   index, myErr);
		return false;
	}
	return true;

}



OGRwkbGeometryType  vrLayerVector::GetGeometryType() {

	// geometry type exists, return it.
	if (m_GeometryType != wkbUnknown) {
		return m_GeometryType;
	}

	// geometry type doesn't exists try to get it from feature 0;
	if (m_Layer == NULL) {
		wxLogError("Layer not inited, call open() first");
		return wkbUnknown;
	}

	//OGRFeature * myFeature = m_Layer->GetFeature(0);
	m_Layer->ResetReading();
	OGRFeature * myFeature = m_Layer->GetNextFeature();
	m_Layer->ResetReading();
	if (myFeature == NULL) {
		wxLogMessage("Unable to get geometry from feature with fid %d", 0);
		return wkbUnknown;
	}

	m_GeometryType = myFeature->GetGeometryRef()->getGeometryType();
	OGRFeature::DestroyFeature(myFeature);
	return m_GeometryType;
}


void vrLayerVector::ClearSpatialFilter(){
	if (IsOK() == false) {
		return;
	}

	m_Layer->SetSpatialFilter(NULL);
}





bool vrLayerVector::DeleteFeature(long fid) {
	wxASSERT(m_Layer);

	OGRErr myErr = m_Layer->DeleteFeature(fid);

	if(myErr != OGRERR_NONE){
		wxLogError(_("Error deleting feature: %ld! (Error number: %d)"), fid, myErr);
		return false;
	}
	return true;
}



void vrLayerVector::SetSelectedIDs(const wxArrayLong & value) {
	m_SelectedIDs = value;
}


void vrLayerVector::SetSelectedID (long id){
    m_SelectedIDs.Clear();
    m_SelectedIDs.Add(id);
}



bool vrLayerVector::SearchFeatures(OGRGeometry * geometry, wxArrayLong & results) {
	//wxASSERT(geometry); NULL geometry indicates to search all features!
	wxASSERT(m_Layer);
	results.Clear();

	m_Layer->ResetReading();
	m_Layer->SetSpatialFilter(geometry);

	// searching all features
	OGRFeature * poFeature = NULL;
	while( (poFeature = m_Layer->GetNextFeature()) != NULL )
	{
		results.Add(poFeature->GetFID());
		OGRFeature::DestroyFeature(poFeature);
	}
	return true;
}


bool vrLayerVector::SetFeature(OGRFeature * feature) {
    if (feature == NULL) {
        return false;
    }

    if( m_Layer->SetFeature(feature) != OGRERR_NONE){
        return false;
    }
    return true;
}


bool vrLayerVector::IsFeatureSelected(long id) {
	for (unsigned int i = 0; i<m_SelectedIDs.GetCount(); i++) {
		if (id == m_SelectedIDs.Item(i)) {
			return true;
		}
	}
	return false;
}


long vrLayerVector::Select (const vrRealRect & rect){
	OGRPolygon myGeom;
	OGRLinearRing myLine;
	myLine.addPoint(rect.GetLeft(), rect.GetTop());
	myLine.addPoint(rect.GetRight(), rect.GetTop());
	myLine.addPoint(rect.GetRight(), rect.GetBottom());
	myLine.addPoint(rect.GetLeft(), rect.GetBottom());
	myLine.addPoint(rect.GetLeft(), rect.GetTop());
	myGeom.addRing(&myLine);
	wxArrayLong mySelectedIDs;
	SearchFeatures(&myGeom, mySelectedIDs);
	SetSelectedIDs(mySelectedIDs);
	return mySelectedIDs.GetCount();
}



void vrLayerVector::SetHiddenObjectID(const wxArrayLong & value) {
    m_HiddenObjectID = value;
}



void vrLayerVector::SetHiddenObjectID(long id) {
    m_HiddenObjectID.Clear();
    m_HiddenObjectID.Add(id);
}



bool vrLayerVector::IsFeatureHidden(long id) {
	for (unsigned int i = 0; i<m_HiddenObjectID.GetCount(); i++) {
		if (id == m_HiddenObjectID.Item(i)) {
			return true;
		}
	}
	return false;
}



vrLayerVectorOGR::vrLayerVectorOGR() {
	m_DriverType = vrDRIVER_VECTOR_SHP;
	m_GeometryType = wkbUnknown;
}

vrLayerVectorOGR::~vrLayerVectorOGR() {
	_Close();
}


bool vrLayerVectorOGR::Open(const wxFileName & filename, bool readwrite){
	// try to close if open
	_Close();
	wxASSERT(m_Dataset == NULL);

	m_FileName = filename;

	// open datasource
	m_Dataset = OGRSFDriverRegistrar::Open( filename.GetFullPath(), readwrite);
	//wxLogMessage(filename.GetFullPath());
    if( m_Dataset == NULL ){
		wxLogError("Unable to open %s, maybe driver not registred - OGRRegisterAll()", filename.GetFullName());
		return false;
	}

	// try to get layer
	wxASSERT(m_Layer == NULL);
	m_Layer = m_Dataset->GetLayer(0);
	if (m_Layer == NULL){
		wxLogError("Unable to get layer 0, number of layer found : %d",
				   m_Dataset->GetLayerCount());
		return false;
	}


	return true;
}



bool vrLayerVectorOGR::Create(const wxFileName & filename, int spatialtype) {
    _Close();
    wxASSERT(m_Dataset == NULL);

	m_FileName = filename;

    // get driver type
    vrDrivers myDriver;
    vrDRIVERS_TYPE myDriverType = myDriver.GetType(filename.GetExt());
    if (myDriverType == vrDRIVER_UNKNOWN) {
        wxLogError (".%s file extension not supported", filename.GetExt());
        return false;
    }
	m_DriverType = myDriverType;

    wxString myDriverName = vrDRIVERS_GDAL_NAMES[myDriverType];
    OGRSFDriver * poDriver = OGRSFDriverRegistrar::GetRegistrar()->
        GetDriverByName((const char *) myDriverName.mb_str(wxConvUTF8));

    if( poDriver == NULL){
       wxLogError("%s driver not available.", myDriverName);
        return false;
    }

    // create dataset
    m_Dataset = poDriver->CreateDataSource((const char *)filename.GetFullPath().mb_str(wxConvUTF8), NULL );
    if( m_Dataset == NULL ){
        wxLogError( "Creation of output file : %s failed.", filename.GetFullName());
        return false;
    }

    // create layer
    m_Layer = m_Dataset->CreateLayer((const char *)filename.GetFullName().mb_str(wxConvUTF8), NULL, (OGRwkbGeometryType) spatialtype, NULL );
    if( m_Layer == NULL ){
        wxLogError( "Layer creation failed." );
        return false;
    }
    m_GeometryType = (OGRwkbGeometryType) spatialtype;
	return true;
}



bool vrLayerVectorOGR::CopyLayer (vrLayerVectorOGR * layer, const wxString & newlayername){
    if (layer == NULL) {
        wxLogError(_("Origin layer is empty! copying failed!"));
        return false;
    }

    wxString myLayerName = newlayername;
    if (myLayerName == wxEmptyString) {
        myLayerName = GetDisplayName().GetName();
    }

    for (unsigned int i = 0; i< m_Dataset->GetLayerCount(); i++) {
        OGRLayer * myExistingLayer = m_Dataset->GetLayer(i);
        if (myExistingLayer == m_Layer) {
            m_Dataset->DeleteLayer(i);
            m_Layer = NULL;
        }
    }

    wxASSERT(m_Layer == NULL);
    wxASSERT(m_Dataset);
    m_Layer = m_Dataset->CopyLayer(layer->GetLayerRef(), (const char *) myLayerName.mb_str(wxConvUTF8));

    if (m_Layer == NULL){
        wxLogError(_("Error copying layer: %s"), myLayerName);
        return false;
    }

    m_Layer = m_Dataset->GetLayerByName((const char *) myLayerName.mb_str(wxConvUTF8));
    wxASSERT(m_Layer);
    return true;
}


bool vrLayerVectorOGR::AddField(OGRFieldDefn & fielddef) {
    wxASSERT(m_Layer);
    if( m_Layer->CreateField(&fielddef) != OGRERR_NONE){
        wxLogError("Error creating field : %s", fielddef.GetNameRef());
        return false;
    }
    return true;
}


long vrLayerVectorOGR::AddFeature(OGRGeometry * geometry, void * data) {
    wxASSERT(m_Layer);
	OGRFeature * myFeature = OGRFeature::CreateFeature(m_Layer->GetLayerDefn());
	wxASSERT(m_Layer);
	myFeature->SetGeometry(geometry);

	if (data != NULL) {
		wxArrayString * myArray = (wxArrayString*) data;
		wxASSERT((signed) myArray->GetCount() == myFeature->GetFieldCount());
		for (unsigned int i = 0; i< myArray->GetCount(); i++) {
            myFeature->SetField(i, myArray->Item(i).mb_str(wxCSConv(wxFONTENCODING_ISO8859_1)));
        }
	}

	if(m_Layer->CreateFeature(myFeature) != OGRERR_NONE){
		wxLogError(_("Error creating feature"));
		OGRFeature::DestroyFeature(myFeature);
		return wxNOT_FOUND;
	}
	long myFeatureID = myFeature->GetFID();
	wxASSERT(myFeatureID != OGRNullFID);
	OGRFeature::DestroyFeature(myFeature);
	return myFeatureID;

}


bool vrLayerVectorOGR::_Close() {
	m_GeometryType = wkbUnknown;
	if(m_Dataset==NULL){
		return false;
	}

	//wxLogMessage("Closing data NOW");
	OGRDataSource::DestroyDataSource( m_Dataset );
	m_Dataset = NULL;
	// layer destroyed with the data source
	m_Layer = NULL;
	return true;
}


void vrLayerVectorOGR::_DrawPoint(wxDC * dc, OGRFeature * feature, OGRGeometry * geometry, const wxRect2DDouble & coord, const vrRender * render,  vrLabel * label, double pxsize){
	OGRPoint * myGeom = (OGRPoint*) geometry;
    wxPoint myPt = _GetPointFromReal(wxPoint2DDouble(myGeom->getX(),myGeom->getY()),coord.GetLeftTop(),pxsize);
    int myWidth = 0;
    int myHeight = 0;
    dc->GetSize(&myWidth, &myHeight);
    wxRect myWndRect(0,0,myWidth, myHeight);
    if (myWndRect.Contains(myPt)==false) {
        return;
    }

    wxASSERT(render->GetType() == vrRENDER_VECTOR);
    vrRenderVector * myRender = (vrRenderVector*) render;
    wxPen myPen (myRender->GetColorPen(), myRender->GetSize());
    wxPen mySelPen (myRender->GetSelectionColour(), myRender->GetSize());
    dc->SetPen(myPen);
    if (IsFeatureSelected(feature->GetFID())==true) {
        dc->SetPen(mySelPen);
    }

#ifdef __WXMSW__
    dc->DrawLine(myPt.x, myPt.y, myPt.x + 1, myPt.y + 1);
#else
    // do not use Draw Point because width isn't supported under
    // linux
    dc->DrawLine(myPt.x, myPt.y, myPt.x, myPt.y);
#endif
	m_ObjectDrawn++;
}



void vrLayerVectorOGR::_DrawLine(wxDC * dc, OGRFeature * feature, OGRGeometry * geometry, const wxRect2DDouble & coord, const vrRender * render,  vrLabel * label, double pxsize){
	OGRLineString * myLine = (OGRLineString*) geometry;
    if (myLine == NULL) {
        // line without a geometry!! corrupted line
        wxLogError(_("Object with FID: %ld has no Geometry!"), feature->GetFID());
        return;
    }

    int iNumVertex = myLine->getNumPoints();
    wxASSERT(iNumVertex >= 2); // line cannot exists with only one vertex

    wxPointList myPtx;
    myPtx.DeleteContents(true);
    for (int i = 0; i< iNumVertex; i++) {
        wxPoint myPt = _GetPointFromReal(wxPoint2DDouble(myLine->getX(i),myLine->getY(i)),coord.GetLeftTop(),pxsize);
        if (myPt != m_PreviousPoint) {
            myPtx.push_back(new wxPoint(myPt));
            ++m_DrawnVertex;
        }
        else {
            ++ m_SkippedVertex;
        }
        m_PreviousPoint = myPt;
    }
    if (myPtx.GetCount() < 2) {
        return;
    }

    // check intersection and minimum size
    /*int myWidth = 0;
    int  myHeight = 0;
    dc->GetSize(&myWidth, &myHeight);
    wxRect2DDouble myWndRect (0,0,myWidth, myHeight);
    wxRect2DDouble myPathRect = myPath.GetBox();
    if(_Intersects(myPathRect, myWndRect)==false){
        return;
    }
    if (myPathRect.GetSize().x < 1 && myPathRect.GetSize().y < 1){
        return;
    }*/

    // Brush
    wxASSERT(render->GetType() == vrRENDER_VECTOR);
	vrRenderVector * myRender = (vrRenderVector*) render;
	wxPen myPen (myRender->GetColorPen(),myRender->GetSize());
	wxPen mySelPen (myRender->GetSelectionColour(),myRender->GetSize());
    dc->SetPen(myPen);
    if (IsFeatureSelected(feature->GetFID())==true) {
        dc->SetPen(mySelPen);
    }
    dc->DrawLines(&myPtx);
	m_ObjectDrawn++;
}



void vrLayerVectorOGR::_DrawPolygon(wxDC * dc, OGRFeature * feature, OGRGeometry * geometry, const wxRect2DDouble & coord, const vrRender * render,  vrLabel * label, double pxsize){
    OGRPolygon * myPolygon = (OGRPolygon*) geometry;
    int iNumRing = myPolygon->getNumInteriorRings() + 1;

    wxGCDC * mygdc = static_cast<wxGCDC*>(dc);

    wxGraphicsPath myPath = mygdc->GetGraphicsContext()->CreatePath();
    for (int i = 0; i < iNumRing; i++) {
        wxGraphicsPath myPolyPath = mygdc->GetGraphicsContext()->CreatePath();
        OGRLineString * myRing  = NULL;
        if (i == 0) {
            myRing = myPolygon->getExteriorRing();
        }
        else {
            myRing = myPolygon->getInteriorRing(i -1);
        }
        wxASSERT(myRing);
        int iNumVertex = myRing->getNumPoints();
        if (iNumVertex <= 1) {
            wxLogWarning(_("Polygon with FID: %ld has an incorrect ring (less than a vertex!). Total number of ring for that polygon is: %d"), feature->GetFID(),iNumRing);
            continue;
        }
        myPolyPath.MoveToPoint(_GetPointFromReal(wxPoint2DDouble(myRing->getX(0),myRing->getY(0)),coord.GetLeftTop(),pxsize));
        for (int v = 0; v < iNumVertex; v++) {
            myPolyPath.AddLineToPoint(_GetPointFromReal(wxPoint2DDouble(myRing->getX(v), myRing->getY(v)),coord.GetLeftTop(),pxsize));
        }
        myPolyPath.CloseSubpath();
        myPath.AddPath(myPolyPath);
    }

    // check intersection and minimum size
    int myWidth = 0;
    int myHeight = 0;
    dc->GetSize(&myWidth, &myHeight);
    wxRect2DDouble myWndRect (0,0,myWidth, myHeight);
    wxRect2DDouble myPathRect = myPath.GetBox();
    if(_Intersects(myPathRect, myWndRect)==false){
        return;
    }

    if (myPathRect.GetSize().x < 1 && myPathRect.GetSize().y < 1){
        return;
    }

    // Brush and Pen
    wxASSERT(render->GetType() == vrRENDER_VECTOR);
	vrRenderVector * myRender = (vrRenderVector*) render;
	wxPen myPen (myRender->GetColorPen(),myRender->GetSize());
	wxBrush myBrush (myRender->GetColorBrush(), myRender->GetBrushStyle());
	wxPen mySelPen (myRender->GetSelectionColour(),
					myRender->GetSize());
    dc->SetBrush(myBrush);
    dc->SetPen(myPen);
    if (IsFeatureSelected(feature->GetFID())==true) {
        dc->SetPen(mySelPen);
    }
    mygdc->GetGraphicsContext()->DrawPath(myPath);
	m_ObjectDrawn++;
    return;
}




bool vrLayerVectorOGR::GetExtent(vrRealRect & rect) {
	if (m_Layer == NULL) {
		wxLogError("Layer isn't inited");
		return false;
	}

	wxASSERT(m_Layer);
	m_Layer->SetSpatialFilter(NULL);
	rect = vrRealRect();
	if (m_Layer->GetFeatureCount() == 0) {
		rect.SetLeftBottom(wxPoint2DDouble(0.0, 0.0));
		rect.SetRightTop(wxPoint2DDouble(1000.0, 1000.0));
		return true;
	}

	OGREnvelope myEnveloppe;
	if (m_Layer->GetExtent(&myEnveloppe, true)==OGRERR_FAILURE) {
		wxLogError("Unable to compute extent for layer %s", m_FileName.GetFullName());
		return false;
	}

	rect.SetLeftBottom(wxPoint2DDouble(myEnveloppe.MinX, myEnveloppe.MinY));
	rect.SetRightTop(wxPoint2DDouble(myEnveloppe.MaxX, myEnveloppe.MaxY));
	return true;
}



bool vrLayerVectorOGR::GetData(wxBitmap * bmp, const vrRealRect & coord, double pxsize,
							   const vrRender * render, vrLabel * label) {
	wxASSERT(m_Layer);
	wxASSERT(render);

	// setting spatial filter
	m_Layer->SetSpatialFilterRect(coord.GetLeft(), coord.GetBottom(), coord.GetRight(), coord.GetTop());
	m_Layer->ResetReading();

    wxMemoryDC dc (*bmp);
    wxGCDC myGCDC (dc);

    // wxDC is way faster under Windows but didn't support transparancy nor antialiasing
    // default is to use wxGCDC. To use faster, uglier wxDC set parameter in Render.
    wxDC * pDC = &myGCDC;
    if (render->GetTransparency() == 0 && static_cast<const vrRenderVector*>(render)->IsUsingFastAndUglyDC() == true){
        pDC = &dc;
    }

	bool bReturn = true;
	m_ObjectDrawn = 0;
    m_PreviousPoint = wxDefaultPosition;
    m_SkippedVertex = 0;
    m_DrawnVertex = 0;
    while (1) {
		OGRFeature * myFeat = GetNextFeature(false);
		if (myFeat == NULL) {
			break;
		}

        if (IsFeatureHidden(myFeat->GetFID()) == true) {
            OGRFeature::DestroyFeature(myFeat);
			myFeat = NULL;
			continue;
        }

        OGRwkbGeometryType myGeomType = wkbFlatten(myFeat->GetGeometryRef()->getGeometryType());
        // multigeometries support
        if (myGeomType == wkbMultiPolygon || myGeomType == wkbMultiLineString || myGeomType == wkbMultiPoint) {
            OGRGeometryCollection * myCollection = (OGRGeometryCollection*) myFeat->GetGeometryRef();
            for (unsigned int i = 0 ; i < myCollection->getNumGeometries() ; i++) {
                switch (myGeomType) {
                    case wkbMultiPoint:
                        _DrawPoint(pDC, myFeat, myCollection->getGeometryRef(i), coord, render, label, pxsize);
                        break;

                    case wkbMultiLineString:
                        _DrawLine(pDC, myFeat, myCollection->getGeometryRef(i), coord, render, label, pxsize);
                        break;

                    case wkbMultiPolygon:
                        // Allways use GCDC for polygons
                        pDC = &myGCDC;
                        _DrawPolygon(pDC, myFeat, myCollection->getGeometryRef(i), coord, render, label, pxsize);
                        break;

                    default:
                        wxLogError("Geometry type of %s, feature: %ld isn't supported or defined (%d)", m_FileName.GetFullName(), myFeat->GetFID(), myGeomType);
                        OGRFeature::DestroyFeature(myFeat);
                        return false;
                }
            }

        }
        else{
            switch (myGeomType) {
                case wkbPoint:
                    _DrawPoint(pDC, myFeat, myFeat->GetGeometryRef(), coord, render, label, pxsize);
                    break;

                case wkbLineString:
                    _DrawLine(pDC, myFeat, myFeat->GetGeometryRef(), coord, render, label, pxsize);
                    break;

                case wkbPolygon:
                    // Allways use GCDC for polygons
                    pDC = &myGCDC;
                    _DrawPolygon(pDC, myFeat, myFeat->GetGeometryRef(), coord, render, label, pxsize);
                    break;

                default:
                    wxLogError("Geometry type of %s, feature: %ld isn't supported or defined (%d)", m_FileName.GetFullName(), myFeat->GetFID(), myGeomType);
                    OGRFeature::DestroyFeature(myFeat);
                    return false;
            }

        }
        OGRFeature::DestroyFeature(myFeat);
     }

    if (bReturn == false) {
        return false;
    }
    return true;
}




bool vrLayerVectorOGR::SetAttributeFilter(const wxString & query) {
	wxASSERT(m_Layer);
	if (query == wxEmptyString) {
		OGRErr myErr = m_Layer->SetAttributeFilter(NULL);
		if (myErr != OGRERR_NONE) {
			wxLogError(_("Error cleaning attribute filter: (code %d)"), myErr);
			return false;
		}
		m_Layer->ResetReading();
		return true;
	}


	OGRErr myErr = m_Layer->SetAttributeFilter((const char*) query.mb_str(wxConvUTF8));
	if (myErr != OGRERR_NONE) {
		wxLogError(_("Error setting following attribute filter : %s, (code %d)"),
				   query, myErr);
		return false;
	}
	m_Layer->ResetReading();
	return true;
}




