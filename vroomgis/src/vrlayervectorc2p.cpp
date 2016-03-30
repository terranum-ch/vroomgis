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


void vrLayerVectorC2P::_DrawPoint(wxDC *dc, OGRFeature *feature, OGRGeometry *geometry, const wxRect2DDouble &coord,
                                  const vrRender *render, vrLabel *label, double pxsize)
{
    wxASSERT(render->GetType() == vrRENDER_VECTOR_C2P_DIPS);
    vrRenderVectorC2PDips *myRender = (vrRenderVectorC2PDips *) render;
    const int myDipWidth = myRender->GetDipWidth();
    wxPen myDefaultPen(myRender->GetDipColour(0), myRender->GetSize());
    wxPen mySelPen(myRender->GetSelectionColour(), myRender->GetSize());

    wxGraphicsContext *gdc = dc->GetGraphicsContext();
    wxASSERT(gdc);

    double myWidth = 0, myHeight = 0;
    gdc->GetSize(&myWidth, &myHeight);
    wxRect2DDouble myWndRect(0, 0, myWidth, myHeight);

    // get direction
    double myDir = feature->GetFieldAsDouble(1);
    OGRPoint *myGeom = (OGRPoint *) geometry;
    wxPoint myPt = _GetPointFromReal(wxPoint2DDouble(myGeom->getX(), myGeom->getY()), coord.GetLeftTop(), pxsize);

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
    if (myPathRect.Intersects(myWndRect) == false) {
        return;
    }

    if (myPathRect.GetSize().x < 1 && myPathRect.GetSize().y < 1) {
        return;
    }

    // create family pen if needed
    if (myRender->IsUsingDefaultColour() == false) {
        int myFamily = feature->GetFieldAsInteger(3);
        if (myRender->IsFamilyVisible(myFamily) == false) {
            return;
        }

        myDefaultPen.SetColour(myRender->GetDipColour(myFamily));
        myDefaultPen.SetWidth(myRender->GetSize());
    }
    wxPen myActualPen = myDefaultPen;
    int bselected = 0;
    if (IsFeatureSelected(feature->GetFID()) == true) {
        myActualPen = mySelPen;
        bselected = 1;
    }

    // draw outline if asked
    if (myRender->GetOutline() == true) {
        wxPen myOutlinePen(*wxBLACK, myRender->GetSize() + 2);
        myOutlinePen.SetColour(myRender->GetOutlineColour(myActualPen.GetColour()));
        gdc->SetPen(myOutlinePen);
        gdc->StrokePath(myHPath);
        gdc->StrokePath(myVPath);
    }

    gdc->SetPen(myActualPen);
    gdc->StrokePath(myHPath);
    gdc->StrokePath(myVPath);
    ++m_objectDrawn;
    ++m_drawnVertex;

    // label feature
    if (label != NULL && label->IsActive() == true) {
        OGRPoint myImgPt;
        myImgPt.setX(myPt.x);
        myImgPt.setY(myPt.y);
        label->AddFeature(bselected, &myImgPt, wxString::Format("%1.f", feature->GetFieldAsDouble(0)), myDir);
        label->Draw(gdc, coord, render, pxsize);
    }
}


void vrLayerVectorC2P::_DrawPolygon(wxDC *dc, OGRFeature *feature, OGRGeometry *geometry, const wxRect2DDouble &coord,
                                    const vrRender *render, vrLabel *label, double pxsize)
{
    OGRPolygon *myPolygon = (OGRPolygon *) geometry;
    int iNumRing = myPolygon->getNumInteriorRings() + 1;

    wxGraphicsContext *gdc = dc->GetGraphicsContext();
    wxASSERT(gdc);

    wxGraphicsPath myPath = gdc->CreatePath();
    for (int i = 0; i < iNumRing; i++) {
        wxGraphicsPath myPolyPath = gdc->CreatePath();
        OGRLineString *myRing = NULL;
        if (i == 0) {
            myRing = myPolygon->getExteriorRing();
        } else {
            myRing = myPolygon->getInteriorRing(i - 1);
        }
        wxASSERT(myRing);
        int iNumVertex = myRing->getNumPoints();
        if (iNumVertex <= 1) {
            wxLogWarning(
                        _("Polygon with FID: %ld has an incorrect ring (less than a vertex!). Total number of ring for that polygon is: %d"),
                        feature->GetFID(), iNumRing);
            continue;
        }
        myPolyPath.MoveToPoint(
                _GetPointFromReal(wxPoint2DDouble(myRing->getX(0), myRing->getY(0)), coord.GetLeftTop(), pxsize));
        for (int v = 0; v < iNumVertex; v++) {
            myPolyPath.AddLineToPoint(
                    _GetPointFromReal(wxPoint2DDouble(myRing->getX(v), myRing->getY(v)), coord.GetLeftTop(), pxsize));
        }
        myPolyPath.CloseSubpath();
        myPath.AddPath(myPolyPath);
    }

    // check intersection and minimum size
    double myWidth = 0, myHeight = 0;
    gdc->GetSize(&myWidth, &myHeight);
    wxRect2DDouble myWndRect(0, 0, myWidth, myHeight);
    wxRect2DDouble myPathRect = myPath.GetBox();
    if (_Intersects(myPathRect, myWndRect) == false) {
        return;
    }

    if (myPathRect.GetSize().x < 1 && myPathRect.GetSize().y < 1) {
        return;
    }

    // Brush and Pen
    wxASSERT(render->GetType() == vrRENDER_VECTOR_C2P_POLY);
    vrRenderVectorC2PPoly *myRender = (vrRenderVectorC2PPoly *) render;
    wxPen myPen(myRender->GetColorPen(), myRender->GetSize());
    wxPen mySelPen(myRender->GetSelectionColour(), myRender->GetSize());
    wxBrush myBrush(myRender->GetColorBrush(), myRender->GetBrushStyle());

    // family based brush
    gdc->SetBrush(myBrush);
    if (myRender->IsUsingDefaultBrush() == false) {
        int myFamily = feature->GetFieldAsInteger(1);
        myBrush.SetStyle(wxBRUSHSTYLE_SOLID);
        myBrush.SetColour(myRender->GetPolyColour(myFamily));
        gdc->SetBrush(myBrush);
    }

    gdc->SetPen(myPen);
    if (IsFeatureSelected(feature->GetFID()) == true) {
        gdc->SetPen(mySelPen);
    }
    gdc->DrawPath(myPath);
}


vrLayerVectorC2P::vrLayerVectorC2P()
{
    wxASSERT(m_dataset == NULL);
    wxASSERT(m_layer == NULL);
    m_driverType = vrDRIVER_VECTOR_C2P;
    m_activeLayerIndex = CT_DIP;
}


vrLayerVectorC2P::~vrLayerVectorC2P()
{
}


bool vrLayerVectorC2P::Open(const wxFileName &filename, bool readwrite)
{
    wxFileName myFileName = filename;
    // Support layer number to open in extension (.c2p 1) open layer 1
    m_activeLayerIndex = 0;
    wxRegEx reLayer(_T("(c2p) ([0-9]+)"), wxRE_ADVANCED);
    if (reLayer.Matches(myFileName.GetExt()) == true) {
        wxASSERT(reLayer.GetMatchCount() == 2 + 1);
        wxString myLayerNumTxt = reLayer.GetMatch(myFileName.GetExt(), 2);
        m_activeLayerIndex = wxAtoi(myLayerNumTxt);
        myFileName.SetExt(_T("c2p"));
    }

    // Need to reimplement Open to allow opening c2p files as if it was sqlite.
    _Close();
    wxASSERT(m_dataset == NULL);

    m_fileName = filename;
    OGRSFDriverRegistrar *myRegistar = OGRSFDriverRegistrar::GetRegistrar();
    OGRSFDriver *myDriver = myRegistar->GetDriverByName("SQLite");
    if (myDriver == NULL) {
        wxLogError(_("Unable to load SQLite Driver! GDAL is maybe not compiled with SQLite support"));
        return false;
    }

    m_dataset = myDriver->Open(myFileName.GetFullPath(), readwrite);
    if (m_dataset == NULL) {
        wxLogError("Unable to open %s", myFileName.GetFullName());
        return false;
    }
    m_dataset->SetDriver(myDriver);

    // get layer
    wxASSERT(m_layer == NULL);
    wxLogMessage("Found %d layer", m_dataset->GetLayerCount());
    m_layer = m_dataset->GetLayer(m_activeLayerIndex);
    if (m_layer == NULL) {
        wxLogError("Unable to get layer %d, number of layer found : %d", m_activeLayerIndex,
                   m_dataset->GetLayerCount());
        return false;
    }
    return true;
}


CT_LAYER_TYPE vrLayerVectorC2P::GetActiveLayerType()
{
    return (CT_LAYER_TYPE) m_activeLayerIndex;
}


wxFileName vrLayerVectorC2P::GetDisplayName()
{
    wxASSERT(m_layer);
    wxString myLayerName(m_layer->GetName());
    wxFileName myDisplayName(wxEmptyString, wxEmptyString, _T("UNKNOWN"), wxEmptyString);
    if (myLayerName == _T("CT_DIP")) {
        myDisplayName.Assign(wxEmptyString, wxEmptyString, _T("DIPS"), wxEmptyString);
    }
    if (myLayerName == _T("CT_POLYGON")) {
        myDisplayName.Assign(wxEmptyString, wxEmptyString, "POLYGONS", wxEmptyString);
    }
    return myDisplayName;
}


long vrLayerVectorC2P::AddFeature(OGRGeometry *geometry, void *data)
{
    wxASSERT(m_layer);
    OGRFeature *myFeature = OGRFeature::CreateFeature(m_layer->GetLayerDefn());
    wxASSERT(m_layer);
    myFeature->SetGeometry(geometry);

    if (data != NULL) {
        if (m_activeLayerIndex == CT_DIP) {
            wxArrayDouble *myArray = (wxArrayDouble *) data;
            wxASSERT(myArray->GetCount() == 4);
            myFeature->SetField(0, myArray->Item(1));
            myFeature->SetField(1, myArray->Item(2));
            myFeature->SetField(2, myArray->Item(0));
            myFeature->SetField(3, myArray->Item(3));
        } else if (m_activeLayerIndex == CT_POLYGON) {
            wxArrayString *myArray = (wxArrayString *) data;
            wxASSERT(myArray->GetCount() == 2);
            myFeature->SetField(0, (const char *) myArray->Item(0).mb_str(wxConvUTF8));
            myFeature->SetField(1, wxAtoi(myArray->Item(1)));
        }
    }

    if (m_layer->CreateFeature(myFeature) != OGRERR_NONE) {
        wxLogError(_("Error creating feature"));
        OGRFeature::DestroyFeature(myFeature);
        return wxNOT_FOUND;
    }
    long myFeatureID = myFeature->GetFID();
    wxASSERT(myFeatureID != OGRNullFID);
    OGRFeature::DestroyFeature(myFeature);
    return myFeatureID;
}


bool vrLayerVectorC2P::DeleteFeature(long fid)
{
    if (m_layer->DeleteFeature(fid) != OGRERR_NONE) {
        return false;
    }
    return true;
}


bool vrLayerVectorC2P::GetExtent(vrRealRect &rect)
{
    if (m_layer == NULL) {
        wxLogError("Layer isn't inited");
        return false;
    }

    wxASSERT(m_layer);
    m_layer->SetSpatialFilter(NULL);
    rect = vrRealRect();
    if (m_layer->GetFeatureCount() == 0) {
        rect.SetLeftBottom(wxPoint2DDouble(0.0, 0.0));
        rect.SetRightTop(wxPoint2DDouble(1000.0, 1000.0));
        return true;
    }

    OGREnvelope myExtent;
    m_layer->GetExtent(&myExtent);

    /*
    wxASSERT(myExtent.IsInit() == false);
    while (1) {
        OGRFeature * myFeat = m_layer->GetNextFeature();
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
    }*/

    rect.SetLeftBottom(wxPoint2DDouble(myExtent.MinX, myExtent.MinY));
    rect.SetRightTop(wxPoint2DDouble(myExtent.MaxX, myExtent.MaxY));
    return true;
}


double vrLayerVectorC2P::GetNoDataValue()
{
    return -9999;
}


