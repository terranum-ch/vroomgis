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


vrLayerVector::vrLayerVector() {
	m_Dataset = NULL;
	m_Layer = NULL;
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



bool vrLayerVector::SearchFeatures(OGRGeometry * geometry, wxArrayLong & results) {
	wxASSERT(geometry);
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



bool vrLayerVector::IsFeatureSelected(long id) {
	for (unsigned int i = 0; i<m_SelectedIDs.GetCount(); i++) {
		if (id == m_SelectedIDs.Item(i)) {
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
	wxLogMessage(filename.GetFullPath());
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
    
    // get driver type
    vrDrivers myDriver;
    vrDRIVERS_TYPE myDriverType = myDriver.GetType(filename.GetExt());
    if (myDriverType == vrDRIVER_UNKNOWN) {
        wxLogError (".%s file extension not supported", filename.GetExt());
        return false;
    }
    
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
	return true;
}



bool vrLayerVectorOGR::AddField(const OGRFieldDefn & fielddef) {
    wxASSERT(m_Layer);
    OGRFieldDefn * myField = new OGRFieldDefn(fielddef);
    if( m_Layer->CreateField(myField) != OGRERR_NONE){
        wxLogError("Error creating field : %s", myField->GetNameRef());
        wxDELETE(myField);
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


bool vrLayerVectorOGR::_DrawLines(wxGraphicsContext * gdc, const wxRect2DDouble & coord,
								  const vrRender * render, const vrLabel * label, double pxsize) {
	wxASSERT(gdc);
	wxStopWatch sw;
	// creating pen
	
	wxASSERT(render->GetType() == vrRENDER_VECTOR);
	vrRenderVector * myRender = (vrRenderVector*) render;
	
	wxPen myPen (myRender->GetColorPen(),
				 myRender->GetSize());
	wxPen mySelPen (myRender->GetSelectionColour(),
					myRender->GetSize());
	
	// iterating and drawing geometries
	OGRLineString * myGeom = NULL;
	double myWidth = 0, myHeight = 0;
	gdc->GetSize(&myWidth, &myHeight);
	wxRect2DDouble myWndRect (0,0,myWidth, myHeight);
	long iCount = 0;
	while (1) {
		OGRFeature * myFeat = GetNextFeature(false);
		if (myFeat == NULL) {
			break;
		}
		
		myGeom = NULL;
		myGeom = (OGRLineString*) myFeat->GetGeometryRef();
		wxASSERT(myGeom);
		
		
		int iNumVertex = myGeom->getNumPoints();
		wxASSERT(iNumVertex >= 2); // line cannot exists with only one vertex
		
		// draw geometry
		wxGraphicsPath myPath = gdc->CreatePath();
		myPath.MoveToPoint(_GetPointFromReal(wxPoint2DDouble(myGeom->getX(0),
															 myGeom->getY(0)),
											 coord.GetLeftTop(),
											 pxsize));
		for (int i = 1; i< iNumVertex; i++) {
			myPath.AddLineToPoint(_GetPointFromReal(wxPoint2DDouble(myGeom->getX(i),
																	myGeom->getY(i)),
													coord.GetLeftTop(),
													pxsize));
		}
				
		wxRect2DDouble myPathRect = myPath.GetBox();
		if(myPathRect.Intersects(myWndRect) == false){
			OGRFeature::DestroyFeature(myFeat);
			myFeat = NULL;
			continue;
		}
		if (myPathRect.GetSize().x < 1 && myPathRect.GetSize().y < 1){
			OGRFeature::DestroyFeature(myFeat);
			myFeat = NULL;
			continue;			
		}
		
		
		gdc->SetPen(myPen);
		if (IsFeatureSelected(myFeat->GetFID())==true) {
			gdc->SetPen(mySelPen);
		}
		
		
		iCount++;
		gdc->StrokePath(myPath);
		OGRFeature::DestroyFeature(myFeat);
		myFeat = NULL;
	}
	
	wxLogMessage("%ld lines drawed in %ldms", iCount, sw.Time());
	return true;
}



bool vrLayerVectorOGR::_DrawPoints(wxGraphicsContext * gdc, const wxRect2DDouble & coord,
								   const vrRender * render, const vrLabel * label, double pxsize) {
	wxASSERT(gdc);
	wxStopWatch sw;
	// creating pen
	
	wxASSERT(render->GetType() == vrRENDER_VECTOR);
	vrRenderVector * myRender = (vrRenderVector*) render;
	
	wxPen myPen (myRender->GetColorPen(),
				 myRender->GetSize());
	wxPen mySelPen (myRender->GetSelectionColour(),
					myRender->GetSize());
	
	
	// iterating and drawing geometries
	OGRPoint * myGeom = NULL;
	long iCount = 0;
	while (1) {
		OGRFeature * myFeat = GetNextFeature(false);
		if (myFeat == NULL) {
			break;
		}
		
		myGeom = NULL;
		myGeom = (OGRPoint*) myFeat->GetGeometryRef();
		wxASSERT(myGeom);
		
		
		// convert and check intersection
		wxPoint myPt = _GetPointFromReal(wxPoint2DDouble(myGeom->getX(),myGeom->getY()),
										 coord.GetLeftTop(),
										 pxsize);
		double myWidth = 0, myHeight = 0;
		gdc->GetSize(&myWidth, &myHeight);
		wxRect myWndRect(0,0,myWidth, myHeight);
		if (myWndRect.Contains(myPt)==false) {
			OGRFeature::DestroyFeature(myFeat);
			myFeat = NULL;
			continue;
		}
		iCount++;

		
		gdc->SetPen(myPen);
		if (IsFeatureSelected(myFeat->GetFID())==true) {
			gdc->SetPen(mySelPen);
		}
		
#ifdef __WXMSW__
		gdc->StrokeLine (myPt.x, myPt.y, myPt.x + 0.1, myPt.y + 0.1);
#else
		gdc->StrokeLine (myPt.x, myPt.y, myPt.x, myPt.y);
#endif
		
		OGRFeature::DestroyFeature(myFeat);
		myFeat = NULL;
	}
	
	wxLogMessage("%ld points drawed in %ldms", iCount, sw.Time());
	return true;
}




bool vrLayerVectorOGR::_Close() {
	m_GeometryType = wkbUnknown;
	if(m_Dataset==NULL){
		return false;
	}
	
	wxLogMessage("Closing data NOW");
	OGRDataSource::DestroyDataSource( m_Dataset );
	m_Dataset = NULL;
	// layer destroyed with the data source
	m_Layer = NULL;
	return true;
}



bool vrLayerVectorOGR::_DrawPolygons(wxGraphicsContext * gdc, const wxRect2DDouble & coord,
									 const vrRender * render, const vrLabel * label, double pxsize) {
	wxASSERT(gdc);
	wxStopWatch sw;
	
	wxASSERT(render->GetType() == vrRENDER_VECTOR);
	vrRenderVector * myRender = (vrRenderVector*) render;
	
	// creating brush and pen
	wxPen myPen (myRender->GetColorPen(),myRender->GetSize());
	// TODO: Add support for different brush style in vrRender
	wxBrush myBrush (myRender->GetColorBrush(), wxBRUSHSTYLE_SOLID); 
	wxPen mySelPen (myRender->GetSelectionColour(),
					myRender->GetSize());
	gdc->SetBrush(myBrush);
	
	
	// iterating and drawing geometries
	OGRPolygon * myGeom = NULL;
	long iCount = 0;
	while (1) {
		OGRFeature * myFeat = GetNextFeature(false);
		if (myFeat == NULL) {
			break;
		}
		//iCount++;
		
		myGeom = NULL;
		myGeom = (OGRPolygon*) myFeat->GetGeometryRef();
		wxASSERT(myGeom);
		
		
		int iNumRing = myGeom->getNumInteriorRings() + 1;
		wxASSERT(iNumRing >= 1); // Polygon should have at least one ring
		
		// create path for polygon
		wxGraphicsPath myPath = gdc->CreatePath();
		for (int i = 0; i < iNumRing; i++) {
			wxGraphicsPath myPolyPath = gdc->CreatePath();
			OGRLineString * myRing  = NULL;
			if (i == 0) {
				myRing = myGeom->getExteriorRing();
			}
			else {
				myRing = myGeom->getInteriorRing(i -1);
			}
			wxASSERT(myRing);
			int iNumVertex = myRing->getNumPoints();
			wxASSERT(iNumVertex > 1);
			myPolyPath.MoveToPoint(_GetPointFromReal(wxPoint2DDouble(myRing->getX(0),
																	 myRing->getY(0)),
													 coord.GetLeftTop(),
													 pxsize));
			for (int v = 0; v < iNumVertex; v++) {
				myPolyPath.AddLineToPoint(_GetPointFromReal(wxPoint2DDouble(myRing->getX(v),
																		 myRing->getY(v)),
														 coord.GetLeftTop(),
														 pxsize));
			}
			myPolyPath.CloseSubpath();
			myPath.AddPath(myPolyPath);
		}
		
		// check intersection and minimum size
		double myWidth = 0, myHeight = 0;
		gdc->GetSize(&myWidth, &myHeight);
		wxRect2DDouble myWndRect (0,0,myWidth, myHeight);
		wxRect2DDouble myPathRect = myPath.GetBox();
		if(myPathRect.Intersects(myWndRect)==false){
			OGRFeature::DestroyFeature(myFeat);
			myFeat = NULL;
			continue;
		}
		
		if (myPathRect.GetSize().x < 1 && myPathRect.GetSize().y < 1){
			OGRFeature::DestroyFeature(myFeat);
			myFeat = NULL;
			continue;
		}
		
		gdc->SetPen(myPen);
		if (IsFeatureSelected(myFeat->GetFID())==true) {
			gdc->SetPen(mySelPen);
		}		
		
		// draw path
		iCount++;
		gdc->DrawPath(myPath);
		OGRFeature::DestroyFeature(myFeat);
		myFeat = NULL;
	}
	
	wxLogMessage("%ld Polygon drawed in %ldms", iCount, sw.Time());
	return true;
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



bool vrLayerVectorOGR::GetDataThread(wxImage * bmp, const vrRealRect & coord,  double pxsize,
									 const vrRender * render, const vrLabel * label) {
	wxASSERT(m_Layer);
	wxASSERT(render);
	
	wxStopWatch sw;
	// setting spatial filter
	m_Layer->SetSpatialFilterRect(coord.GetLeft(), coord.GetBottom(),
								  coord.GetRight(), coord.GetTop());
	m_Layer->ResetReading();
	
	
	
	// prepare bitmap for drawing
	if (bmp->HasAlpha() == false){
		bmp->InitAlpha();
		wxLogMessage("Initing alpha");
	}

	// adding transparency
	char myBackgroundTransparency = 0;
	unsigned char * alphachar = NULL;
	unsigned int myimglen = bmp->GetWidth() * bmp->GetHeight();
	alphachar= (unsigned char*)malloc(myimglen);
	if (alphachar == NULL)
	{
		wxLogError(_T("Error creating transparency bitmap"));
		return false;
	}
	
	
	for ( unsigned int i = 0; i< myimglen; i++) {
		*(alphachar + i) =  myBackgroundTransparency;
	}
	bmp->SetAlpha(alphachar, false);
	

	wxBitmap myBmp(*bmp);
	wxASSERT(myBmp.IsOk());

	// empty bitmap
	/*wxSize myBmpSize = bmp->GetSize();
	wxMemoryDC myDC;
	myDC.SelectObject(myBmp);
	wxGraphicsContext * myPgdc = wxGraphicsContext::Create(myDC);
	myPgdc->SetBrush(wxBrush(wxColour(255,0,0,30)));
	myPgdc->SetPen(wxPen(wxColour(255,0,0,30)));
	wxGraphicsPath path = myPgdc->CreatePath();
	path.AddRectangle(0,0,myBmpSize.x,myBmpSize.y);
	myPgdc->DrawPath(path);
	wxDELETE(myPgdc);
	myDC.SelectObject(wxNullBitmap);*/

	// draw
	wxMemoryDC dc;
	dc.SelectObject(myBmp);
	wxGraphicsContext * pgdc = wxGraphicsContext::Create(dc);

	
	bool bReturn = true;
	OGRwkbGeometryType myGeomType = GetGeometryType();
	
	wxLogMessage("Preparing data took %ldms", sw.Time());
		
	switch (myGeomType) {
		case wkbLineString:
			bReturn = _DrawLines(pgdc, coord, render, label, pxsize);
			break;
			
		case wkbPoint:
			bReturn = _DrawPoints(pgdc, coord, render, label, pxsize);
			break;
			
		case wkbPolygon:
			bReturn = _DrawPolygons(pgdc, coord, render, label, pxsize);
			break;
		
		default: // unsupported case
			wxLogMessage("Geometry type of %s isn't supported or defined (%d)",
					   m_FileName.GetFullName(), myGeomType);
			bReturn = false;
			break;
	}
	
	if (bReturn == false) {
		return false;
	}
	
	wxDELETE(pgdc);
	dc.SelectObject(wxNullBitmap);
	*bmp = myBmp.ConvertToImage();

	return true;
}




bool vrLayerVectorOGR::GetData(wxBitmap * bmp, const vrRealRect & coord, double pxsize, 
							   const vrRender * render, const vrLabel * label) {
	return false;
}




bool vrLayerVectorOGR::SetAttributeFilter(const wxString & query) {
	wxASSERT(m_Layer);
	OGRErr myErr = m_Layer->SetAttributeFilter((const char*) query.mb_str(wxConvUTF8));
	if (myErr != OGRERR_NONE) {
		wxLogError(_("Error setting following attribute filter : %s, (code %d)"),
				   query, myErr);
		return false;
	}
	m_Layer->ResetReading();
	return true;
}




