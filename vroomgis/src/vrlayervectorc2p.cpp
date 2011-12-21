/***************************************************************************
				vrlayervectorc2p.cpp

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

#include "vrlayervectorc2p.h"
#include "vrrendervectorc2p.h"
#include "vrlabel.h"

bool vrLayerVectorC2P::_DrawPoints(wxGraphicsContext * gdc, const wxRect2DDouble & coord,
								   const vrRender * render, const vrLabel * label, double pxsize) {
	m_ObjectDrawn = 0;
	wxASSERT(gdc);
	wxStopWatch sw;
	// creating pen

	wxASSERT(render->GetType() == vrRENDER_VECTOR_C2P_DIPS);
	vrRenderVectorC2PDips * myRender = (vrRenderVectorC2PDips*) render;
	const int myDipWidth = myRender->GetDipWidth();
	const bool myUseDefaultColour = myRender->IsUsingDefaultColour();
	wxPen myDefaultPen (myRender->GetDipColour(0), myRender->GetSize());
	wxPen mySelPen (myRender->GetSelectionColour(), myRender->GetSize());

	// iterating and drawing geometries
	OGRPoint * myGeom = NULL;
	long iCount = 0;
    long iIgnored = 0;
	double myWidth = 0, myHeight = 0;
	gdc->GetSize(&myWidth, &myHeight);
	wxRect2DDouble myWndRect (0,0,myWidth, myHeight);
	while (1) {
		OGRFeature * myFeat = GetNextFeature(false);
		if (myFeat == NULL) {
			break;
		}
		myGeom = NULL;
		myGeom = (OGRPoint*) myFeat->GetGeometryRef();
		wxASSERT(myGeom);
        
        // filter nodata values
        if (wxIsSameDouble(myGeom->getX(), GetNoDataValue()) &&
            wxIsSameDouble(myGeom->getY(), GetNoDataValue())){
            OGRFeature::DestroyFeature(myFeat);
            iIgnored++;
            continue;
        }

		// get direction
		double myDir = myFeat->GetFieldAsDouble(1);
		wxPoint myPt = _GetPointFromReal(wxPoint2DDouble(myGeom->getX(),myGeom->getY()),
										 coord.GetLeftTop(),
										 pxsize);


		// Create | line
		wxGraphicsPath myVPath = gdc->CreatePath();
		myVPath.MoveToPoint(0.0, 0.0);
		myVPath.AddLineToPoint(0.0, -1.0 * myDipWidth);
		// Create -- line
		wxGraphicsPath myHPath = gdc->CreatePath();
		myHPath.MoveToPoint(-1.5 * myDipWidth, 0.0);
		myHPath.AddLineToPoint(1.5 * myDipWidth, 0.0);

		// Rotate
		wxGraphicsMatrix myRotateMatrix = gdc->CreateMatrix();
		myRotateMatrix.Rotate((myDir * M_PI) / 180);
		myVPath.Transform(myRotateMatrix);
		myHPath.Transform(myRotateMatrix);

		// Translate to position
		wxGraphicsMatrix myTranslateMatrix = gdc->CreateMatrix();
		myTranslateMatrix.Translate(myPt.x, myPt.y);
		myVPath.Transform(myTranslateMatrix);
		myHPath.Transform(myTranslateMatrix);

		// ensure intersecting display
		wxRect2DDouble myPathRect = myVPath.GetBox();
		myPathRect.Union(myHPath.GetBox());
		if (myPathRect.Intersects(myWndRect) ==false) {
			OGRFeature::DestroyFeature(myFeat);
			myFeat = NULL;
			continue;
		}
		if (myPathRect.GetSize().x < 1 && myPathRect.GetSize().y < 1){
			OGRFeature::DestroyFeature(myFeat);
			myFeat = NULL;
			continue;
		}
		iCount++;

		// create family pen if needed
		if (myUseDefaultColour == false) {
			int myFamily = myFeat->GetFieldAsInteger(3);
			myDefaultPen.SetColour(myRender->GetDipColour(myFamily));
			myDefaultPen.SetWidth(myRender->GetSize());
		}
        wxPen myActualPen = myDefaultPen;
		//gdc->SetPen(myDefaultPen);
		if (IsFeatureSelected(myFeat->GetFID())==true) {
			//gdc->SetPen(mySelPen);
            myActualPen = mySelPen;
		}

        // draw outline if asked
        if (myRender->GetOutline() == true) {
            wxPen myOutlinePen (*wxBLACK, myRender->GetSize() + 2);
            myOutlinePen.SetColour(myRender->GetOutlineColour(myActualPen.GetColour()));
            gdc->SetPen(myOutlinePen);
            gdc->StrokePath(myHPath);
            gdc->StrokePath(myVPath);
        }

		gdc->SetPen(myActualPen);
		gdc->StrokePath(myHPath);
		gdc->StrokePath(myVPath);


		OGRFeature::DestroyFeature(myFeat);
		myFeat = NULL;
	}

	m_ObjectDrawn = iCount;
	wxLogMessage("%ld dips drawed (%ld ignored) in %ldms", iCount, iIgnored, sw.Time());
	if (iCount == 0){
		return false;
	}
	return true;
}



bool vrLayerVectorC2P::_DrawLines(wxGraphicsContext * gdc, const wxRect2DDouble & coord,
								  const vrRender * render, const vrLabel * label, double pxsize) {
	m_ObjectDrawn = 0;
	return false;
}



bool vrLayerVectorC2P::_DrawPolygons(wxGraphicsContext * gdc, const wxRect2DDouble & coord,
									 const vrRender * render, const vrLabel * label, double pxsize) {
	m_ObjectDrawn = 0;
	return false;
}

bool vrLayerVectorC2P::_DrawMultiPolygons(wxGraphicsContext * gdc, const wxRect2DDouble & coord,
									 const vrRender * render, const vrLabel * label, double pxsize) {
	m_ObjectDrawn = 0;
	return false;
}

vrLayerVectorC2P::vrLayerVectorC2P() {
	wxASSERT(m_Dataset==NULL);
	wxASSERT(m_Layer==NULL);
	m_DriverType = vrDRIVER_VECTOR_C2P;
}


vrLayerVectorC2P::~vrLayerVectorC2P() {
}



bool vrLayerVectorC2P::Open(const wxFileName & filename, bool readwrite) {
	// Need to reimplement Open to allow opening c2p files as if it was sqlite.
	_Close();
	wxASSERT(m_Dataset == NULL);

	m_FileName = filename;

	OGRSFDriverRegistrar * myRegistar = OGRSFDriverRegistrar::GetRegistrar();
	OGRSFDriver * myDriver = myRegistar->GetDriverByName("SQLite");
	if (myDriver == NULL) {
		wxLogError(_("Unable to load SQLite Driver! GDAL is maybe not compiled with SQLite support"));
		return false;
	}

	m_Dataset = myDriver->Open(filename.GetFullPath(), readwrite);
	if( m_Dataset == NULL ){
		wxLogError("Unable to open %s", filename.GetFullName());
		return false;
	}
	m_Dataset->SetDriver(myDriver);

	// get layer
	wxASSERT(m_Layer == NULL);
	wxLogMessage("Found %d layer", m_Dataset->GetLayerCount());
	m_Layer = m_Dataset->GetLayer(0);
	if (m_Layer == NULL) {
		wxLogError("Unable to get layer 0, number of layer found : %d",
				   m_Dataset->GetLayerCount());
		return false;
	}
	return true;
}

wxFileName vrLayerVectorC2P::GetDisplayName() {
	wxFileName myDisplayName (wxEmptyString, wxEmptyString, "DIPS", wxEmptyString);
	return myDisplayName;
}


long vrLayerVectorC2P::AddFeature(OGRGeometry * geometry, void * data) {
	wxASSERT(m_Layer);
	OGRFeature * myFeature = OGRFeature::CreateFeature(m_Layer->GetLayerDefn());
	wxASSERT(m_Layer);
	myFeature->SetGeometry(geometry);

	if (data != NULL) {
		wxArrayDouble * myArray = (wxArrayDouble*) data;
		wxASSERT(myArray->GetCount() == 4);
		myFeature->SetField(0, myArray->Item(1));
		myFeature->SetField(1, myArray->Item(2));
		myFeature->SetField(2, myArray->Item(0));
		myFeature->SetField(3, myArray->Item(3));
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


bool vrLayerVectorC2P::DeleteFeature(long fid) {
	wxString myDelQuery = wxString::Format("DELETE FROM CT_DIP WHERE id=%ld", fid);
	wxString myDialect ("generic");
	wxASSERT(m_Dataset);
	OGRLayer * myLayer = m_Dataset->ExecuteSQL(myDelQuery.mb_str(), NULL, NULL);
	wxASSERT(myLayer == NULL);
	return true;
}



bool vrLayerVectorC2P::GetExtent(vrRealRect & rect) {
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
    
    OGREnvelope myExtent;
    wxASSERT(myExtent.IsInit() == false);
    while (1) {
        OGRFeature * myFeat = m_Layer->GetNextFeature();
        if (myFeat == NULL) {
            break;
        }
        
        OGRPoint *  myPt = (OGRPoint*) myFeat->GetGeometryRef();
        wxASSERT(myPt);
        
        // filter nodata values
        if (wxIsSameDouble(myPt->getX(), GetNoDataValue()) &&
            wxIsSameDouble(myPt->getY(), GetNoDataValue())){
            OGRFeature::DestroyFeature(myFeat);
            continue;
        }

        myExtent.Merge(myPt->getX(), myPt->getY());
        OGRFeature::DestroyFeature(myFeat);
    }
    
    if (myExtent.IsInit() == false) {
        myExtent.Merge(0.0, 0.0);
        myExtent.Merge(1000.0,1000.0);
    }
    
	rect.SetLeftBottom(wxPoint2DDouble(myExtent.MinX, myExtent.MinY));
	rect.SetRightTop(wxPoint2DDouble(myExtent.MaxX, myExtent.MaxY));
	return true;
}



double vrLayerVectorC2P::GetNoDataValue() {
    return -9999;
}


