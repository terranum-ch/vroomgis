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

wxPoint vrLayerVector::_GetPointFromReal(const wxPoint2DDouble& realpt, const wxPoint2DDouble& origin, double pxsize) {
    wxPoint myPt;
    myPt.x = wxRound((realpt.m_x - origin.m_x) / pxsize);
    myPt.y = wxRound((origin.m_y - realpt.m_y) / pxsize);
    return myPt;
}

bool vrLayerVector::_Intersects(const wxRect2DDouble& myPathRect, const wxRect2DDouble& myWndRect) {
    wxDouble left, right, bottom, top;
    left = wxMax(myPathRect.m_x, myWndRect.m_x);
    right = wxMin(myPathRect.m_x + myPathRect.m_width, myWndRect.m_x + myWndRect.m_width);
    top = wxMax(myPathRect.m_y, myWndRect.m_y);
    bottom = wxMin(myPathRect.m_y + myPathRect.m_height, myWndRect.m_y + myWndRect.m_height);

    // Modification of the conditions from wxRect2DDouble::Intersects to allow horizontal and vertical lines.
    return (left < right && top <= bottom) || (left <= right && top < bottom);
}

vrLayerVector::vrLayerVector() {
    m_dataset = nullptr;
    m_layer = nullptr;
    m_objectDrawn = 0;
    m_skippedVertex = 0;
    m_drawnVertex = 0;
    m_geometryType = wkbUnknown;
}

vrLayerVector::~vrLayerVector() {}

bool vrLayerVector::IsOK() {
    if (m_dataset == nullptr) {
        wxLogError("Dataset not inited");
        return false;
    }
    if (m_layer == nullptr) {
        wxLogError("Layer Not inited");
        return false;
    }
    return true;
}

bool vrLayerVector::HasData() {
    return GetFeatureCount(false) > 0;
}

wxFileName vrLayerVector::GetDisplayName() {
    if (m_driverType == vrDRIVER_VECTOR_MEMORY) {
        wxFileName myName(m_fileName);
        myName.SetExt(wxEmptyString);
        return myName;
    }

    return m_fileName;
}

OGRFeature* vrLayerVector::GetFeature(long fid) {
    if (!IsOK()) {
        return nullptr;
    }

    OGRFeature* myFeature = m_layer->GetFeature(fid);

    if (myFeature == nullptr) {
        wxLogError("Unable to get feature with fid %d", fid);
        return nullptr;
    }

    return myFeature;
}

long vrLayerVector::GetFeatureCount(bool onlyvisible) {
    wxASSERT(m_layer);
    if (!onlyvisible) {
        ClearSpatialFilter();
    }
    return m_layer->GetFeatureCount();
}

OGRFeature* vrLayerVector::GetNextFeature(bool restart) {
    if (!IsOK()) {
        return nullptr;
    }

    if (restart) {
        m_layer->ResetReading();
    }

    OGRFeature* myFeature = m_layer->GetNextFeature();
    if (myFeature == nullptr) {
        return nullptr;
    }

    return myFeature;
}

bool vrLayerVector::MoveToFeatureIndex(long index) {
    if (!IsOK()) {
        return false;
    }

    OGRErr myErr = m_layer->SetNextByIndex(index);
    if (myErr != OGRERR_NONE) {
        wxLogError(_("Error going to index : %ld, (code %d)"), index, myErr);
        return false;
    }
    return true;
}

OGRwkbGeometryType vrLayerVector::GetGeometryType() {
    // geometry type exists, return it.
    if (m_geometryType != wkbUnknown) {
        return m_geometryType;
    }

    // geometry type doesn't exists try to get it from feature 0;
    if (m_layer == nullptr) {
        wxLogError("Layer not inited, call open() first");
        return wkbUnknown;
    }

    // OGRFeature * myFeature = m_layer->GetFeature(0);
    m_layer->ResetReading();
    OGRFeature* myFeature = m_layer->GetNextFeature();
    m_layer->ResetReading();
    if (myFeature == nullptr) {
        wxLogMessage("Unable to get geometry from feature with fid %d", 0);
        return wkbUnknown;
    }

    m_geometryType = myFeature->GetGeometryRef()->getGeometryType();
    OGRFeature::DestroyFeature(myFeature);
    return m_geometryType;
}

void vrLayerVector::ClearSpatialFilter() {
    if (!IsOK()) {
        return;
    }

    m_layer->SetSpatialFilter(nullptr);
}

bool vrLayerVector::DeleteFeature(long fid) {
    wxASSERT(m_layer);

    OGRErr myErr = m_layer->DeleteFeature(fid);

    if (myErr != OGRERR_NONE) {
        wxLogError(_("Error deleting feature: %ld! (Error number: %d)"), fid, myErr);
        return false;
    }
    return true;
}

void vrLayerVector::SetSelectedIDs(const wxArrayLong& value) {
    m_selectedIDs = value;
}

void vrLayerVector::SetSelectedID(long id) {
    m_selectedIDs.Clear();
    m_selectedIDs.Add(id);
}

bool vrLayerVector::SearchFeatures(OGRGeometry* geometry, wxArrayLong& results) {
    // wxASSERT(geometry); nullptr geometry indicates to search all features!
    wxASSERT(m_layer);
    results.Clear();

    m_layer->ResetReading();
    m_layer->SetSpatialFilter(geometry);

    // searching all features
    OGRFeature* poFeature = nullptr;
    while ((poFeature = m_layer->GetNextFeature()) != nullptr) {
        results.Add(poFeature->GetFID());
        OGRFeature::DestroyFeature(poFeature);
    }
    return true;
}

bool vrLayerVector::SetFeature(OGRFeature* feature) {
    if (feature == nullptr) {
        return false;
    }

    return m_layer->SetFeature(feature) == OGRERR_NONE;
}

bool vrLayerVector::IsFeatureSelected(long id) {
    for (unsigned int i = 0; i < m_selectedIDs.GetCount(); i++) {
        if (id == m_selectedIDs.Item(i)) {
            return true;
        }
    }
    return false;
}

long vrLayerVector::Select(const vrRealRect& rect) {
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

void vrLayerVector::SetHiddenObjectID(const wxArrayLong& value) {
    m_hiddenObjectID = value;
}

void vrLayerVector::SetHiddenObjectID(long id) {
    m_hiddenObjectID.Clear();
    m_hiddenObjectID.Add(id);
}

bool vrLayerVector::IsFeatureHidden(long id) {
    for (unsigned int i = 0; i < m_hiddenObjectID.GetCount(); i++) {
        if (id == m_hiddenObjectID.Item(i)) {
            return true;
        }
    }
    return false;
}

vrLayerVectorOGR::vrLayerVectorOGR() {
    m_driverType = vrDRIVER_VECTOR_SHP;
    m_geometryType = wkbUnknown;
}

vrLayerVectorOGR::~vrLayerVectorOGR() {
    _Close();
}

bool vrLayerVectorOGR::Open(const wxFileName& filename, bool readwrite) {
    // try to close if open
    _Close();
    wxASSERT(m_dataset == nullptr);

    m_fileName = filename;

    // open datasource
    int flags = GDAL_OF_VECTOR;
    if (readwrite == true) {
        flags = flags | GDAL_OF_UPDATE;
    }

    // m_dataset = OGRSFDriverRegistrar::Open(filename.GetFullPath(), readwrite);
    // wxLogMessage(filename.GetFullPath());
    m_dataset = (GDALDataset*)GDALOpenEx(filename.GetFullPath(), flags, nullptr, nullptr, nullptr);
    if (m_dataset == nullptr) {
        wxLogError("Unable to open %s, maybe driver not registred - OGRRegisterAll()", filename.GetFullName());
        return false;
    }

    // try to get layer
    wxASSERT(m_layer == nullptr);
    m_layer = m_dataset->GetLayer(0);
    if (m_layer == nullptr) {
        wxLogError("Unable to get layer 0, number of layer found : %d", m_dataset->GetLayerCount());
        return false;
    }

    return true;
}

bool vrLayerVectorOGR::Create(const wxFileName& filename, int spatialtype) {
    _Close();
    wxASSERT(m_dataset == nullptr);

    m_fileName = filename;

    // get driver type
    vrDrivers myDriver;
    vrDRIVERS_TYPE myDriverType = myDriver.GetType(filename.GetExt());
    if (myDriverType == vrDRIVER_UNKNOWN) {
        wxLogError(".%s file extension not supported", filename.GetExt());
        return false;
    }
    m_driverType = myDriverType;

    wxString myDriverName = vrDRIVERS_GDAL_NAMES[myDriverType];

    GDALDriver* poDriver = GetGDALDriverManager()->GetDriverByName((const char*)myDriverName.mb_str(wxConvUTF8));
    // OGRSFDriver *poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(
    //         (const char *) myDriverName.mb_str(wxConvUTF8));

    if (poDriver == nullptr) {
        wxLogError("%s driver not available.", myDriverName);
        return false;
    }

    // create dataset
    m_dataset = poDriver->Create((const char*)filename.GetFullPath().mb_str(wxConvUTF8), 0, 0, 0, GDT_Unknown, nullptr);
    if (m_dataset == nullptr) {
        wxLogError("Creation of output file : %s failed.", filename.GetFullName());
        return false;
    }

    // create layer
    m_layer = m_dataset->CreateLayer((const char*)filename.GetFullName().mb_str(wxConvUTF8), nullptr,
                                     (OGRwkbGeometryType)spatialtype, nullptr);
    if (m_layer == nullptr) {
        wxLogError("Layer creation failed.");
        return false;
    }
    m_geometryType = (OGRwkbGeometryType)spatialtype;
    return true;
}

bool vrLayerVectorOGR::CopyLayer(vrLayerVectorOGR* layer, const wxString& newlayername) {
    if (layer == nullptr) {
        wxLogError(_("Origin layer is empty! copying failed!"));
        return false;
    }

    wxString myLayerName = newlayername;
    if (myLayerName == wxEmptyString) {
        myLayerName = GetDisplayName().GetName();
    }

    for (unsigned int i = 0; i < m_dataset->GetLayerCount(); i++) {
        OGRLayer* myExistingLayer = m_dataset->GetLayer(i);
        if (myExistingLayer == m_layer) {
            m_dataset->DeleteLayer(i);
            m_layer = nullptr;
        }
    }

    wxASSERT(m_layer == nullptr);
    wxASSERT(m_dataset);
    m_layer = m_dataset->CopyLayer(layer->GetLayerRef(), (const char*)myLayerName.mb_str(wxConvUTF8));

    if (m_layer == nullptr) {
        wxLogError(_("Error copying layer: %s"), myLayerName);
        return false;
    }

    m_layer = m_dataset->GetLayerByName((const char*)myLayerName.mb_str(wxConvUTF8));
    wxASSERT(m_layer);
    return true;
}

bool vrLayerVectorOGR::AddField(OGRFieldDefn& fielddef) {
    wxASSERT(m_layer);
    if (m_layer->CreateField(&fielddef) != OGRERR_NONE) {
        wxLogError("Error creating field : %s", fielddef.GetNameRef());
        return false;
    }
    return true;
}

long vrLayerVectorOGR::AddFeature(OGRGeometry* geometry, void* data) {
    wxASSERT(m_layer);
    OGRFeature* myFeature = OGRFeature::CreateFeature(m_layer->GetLayerDefn());
    wxASSERT(m_layer);
    myFeature->SetGeometry(geometry);

    if (data != nullptr) {
        wxArrayString* myArray = (wxArrayString*)data;
        wxASSERT((signed)myArray->GetCount() == myFeature->GetFieldCount());
        for (unsigned int i = 0; i < myArray->GetCount(); i++) {
            myFeature->SetField(i, myArray->Item(i).mb_str(wxCSConv(wxFONTENCODING_ISO8859_1)));
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

bool vrLayerVectorOGR::_Close() {
    m_geometryType = wkbUnknown;
    if (m_dataset == nullptr) {
        return false;
    }

    // wxLogMessage("Closing data NOW");
    GDALClose(m_dataset);
    m_dataset = nullptr;
    // layer destroyed with the data source
    m_layer = nullptr;
    return true;
}

void vrLayerVectorOGR::_DrawPoint(wxDC* dc, OGRFeature* feature, OGRGeometry* geometry, const wxRect2DDouble& coord,
                                  const vrRender* render, vrLabel* label, double pxsize) {
    OGRPoint* myGeom = (OGRPoint*)geometry;
    wxPoint myPt = _GetPointFromReal(wxPoint2DDouble(myGeom->getX(), myGeom->getY()), coord.GetLeftTop(), pxsize);
    int myWidth = 0;
    int myHeight = 0;
    dc->GetSize(&myWidth, &myHeight);
    wxRect myWndRect(0, 0, myWidth, myHeight);
    if (!myWndRect.Contains(myPt)) {
        return;
    }

    wxASSERT(render->GetType() == vrRENDER_VECTOR);
    vrRenderVector* myRender = (vrRenderVector*)render;
    wxPen myPen(myRender->GetColorPen(), myRender->GetSize());
    wxPen mySelPen(myRender->GetSelectionColour(), myRender->GetSize());
    dc->SetPen(myPen);
    if (IsFeatureSelected(feature->GetFID())) {
        dc->SetPen(mySelPen);
    }

#ifdef __WXMSW__
    dc->DrawLine(myPt.x, myPt.y, myPt.x + 1, myPt.y + 1);
#else
    // do not use Draw Point because width isn't supported under
    // linux
    dc->DrawLine(myPt.x, myPt.y, myPt.x, myPt.y);
#endif
    m_objectDrawn++;
}

void vrLayerVectorOGR::_DrawLine(wxDC* dc, OGRFeature* feature, OGRGeometry* geometry, const wxRect2DDouble& coord,
                                 const vrRender* render, vrLabel* label, double pxsize) {
    OGRLineString* myLine = (OGRLineString*)geometry;
    if (myLine == nullptr) {
        // line without a geometry!! corrupted line
        wxLogError(_("Object with FID: %ld has no Geometry!"), feature->GetFID());
        return;
    }

    int iNumVertex = myLine->getNumPoints();
    wxASSERT(iNumVertex >= 2);  // line cannot exist with only one vertex

    wxPointList myPtx;
    myPtx.DeleteContents(true);
    for (int i = 0; i < iNumVertex; i++) {
        wxPoint myPt = _GetPointFromReal(wxPoint2DDouble(myLine->getX(i), myLine->getY(i)), coord.GetLeftTop(), pxsize);
        if (myPt != m_previousPoint) {
            myPtx.push_back(new wxPoint(myPt));
            ++m_drawnVertex;
        } else {
            ++m_skippedVertex;
        }
        m_previousPoint = myPt;
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
    vrRenderVector* myRender = (vrRenderVector*)render;
    wxPen myPen(myRender->GetColorPen(), myRender->GetSize());
    wxPen mySelPen(myRender->GetSelectionColour(), myRender->GetSize());
    dc->SetPen(myPen);
    if (IsFeatureSelected(feature->GetFID())) {
        dc->SetPen(mySelPen);
    }
    dc->DrawLines(&myPtx);
    m_objectDrawn++;
}

void vrLayerVectorOGR::_DrawPolygon(wxDC* dc, OGRFeature* feature, OGRGeometry* geometry, const wxRect2DDouble& coord,
                                    const vrRender* render, vrLabel* label, double pxsize) {
    OGRPolygon* myPolygon = (OGRPolygon*)geometry;
    int iNumRing = myPolygon->getNumInteriorRings() + 1;

    wxGCDC* mygdc = static_cast<wxGCDC*>(dc);

    wxGraphicsPath myPath = mygdc->GetGraphicsContext()->CreatePath();
    for (int i = 0; i < iNumRing; i++) {
        wxGraphicsPath myPolyPath = mygdc->GetGraphicsContext()->CreatePath();
        OGRLineString* myRing = nullptr;
        if (i == 0) {
            myRing = myPolygon->getExteriorRing();
        } else {
            myRing = myPolygon->getInteriorRing(i - 1);
        }
        wxASSERT(myRing);
        int iNumVertex = myRing->getNumPoints();
        if (iNumVertex <= 1) {
            wxLogWarning(_("Polygon with FID: %ld has an incorrect ring (less than a vertex!). Total number of ring "
                           "for that polygon is: %d"),
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
    int myWidth = 0;
    int myHeight = 0;
    dc->GetSize(&myWidth, &myHeight);
    wxRect2DDouble myWndRect(0, 0, myWidth, myHeight);
    wxRect2DDouble myPathRect = myPath.GetBox();
    if (!_Intersects(myPathRect, myWndRect)) {
        return;
    }

    if (myPathRect.GetSize().x < 1 && myPathRect.GetSize().y < 1) {
        return;
    }

    // Brush and Pen
    wxASSERT(render->GetType() == vrRENDER_VECTOR);
    vrRenderVector* myRender = (vrRenderVector*)render;
    wxPen myPen(myRender->GetColorPen(), myRender->GetSize());
    wxBrush myBrush(myRender->GetColorBrush(), myRender->GetBrushStyle());
    wxPen mySelPen(myRender->GetSelectionColour(), myRender->GetSize());
    dc->SetBrush(myBrush);
    dc->SetPen(myPen);
    if (IsFeatureSelected(feature->GetFID())) {
        dc->SetPen(mySelPen);
    }
    mygdc->GetGraphicsContext()->DrawPath(myPath);
    m_objectDrawn++;
    return;
}

bool vrLayerVectorOGR::GetExtent(vrRealRect& rect) {
    if (m_layer == nullptr) {
        wxLogError("Layer isn't inited");
        return false;
    }

    wxASSERT(m_layer);
    m_layer->SetSpatialFilter(nullptr);
    rect = vrRealRect();
    if (m_layer->GetFeatureCount() == 0) {
        rect.SetLeftBottom(wxPoint2DDouble(0.0, 0.0));
        rect.SetRightTop(wxPoint2DDouble(1000.0, 1000.0));
        return true;
    }

    OGREnvelope myEnveloppe;
    if (m_layer->GetExtent(&myEnveloppe, true) == OGRERR_FAILURE) {
        wxLogError("Unable to compute extent for layer %s", m_fileName.GetFullName());
        return false;
    }

    rect.SetLeftBottom(wxPoint2DDouble(myEnveloppe.MinX, myEnveloppe.MinY));
    rect.SetRightTop(wxPoint2DDouble(myEnveloppe.MaxX, myEnveloppe.MaxY));
    return true;
}

bool vrLayerVectorOGR::GetData(wxBitmap* bmp, const vrRealRect& coord, double pxsize, const vrRender* render,
                               vrLabel* label) {
    wxASSERT(m_layer);
    wxASSERT(render);

    // setting spatial filter
    m_layer->SetSpatialFilterRect(coord.GetLeft(), coord.GetBottom(), coord.GetRight(), coord.GetTop());
    m_layer->ResetReading();

    wxMemoryDC dc(*bmp);
    wxGCDC myGCDC(dc);

    // wxDC is way faster under Windows but didn't support transparancy nor antialiasing
    // default is to use wxGCDC. To use faster, uglier wxDC set parameter in Render.
    wxDC* pDC = &myGCDC;
    if (render->GetTransparency() == 0 && static_cast<const vrRenderVector*>(render)->IsUsingFastAndUglyDC()) {
        pDC = &dc;
    }

    bool bReturn = true;
    m_objectDrawn = 0;
    m_skippedVertex = 0;
    m_drawnVertex = 0;
    while (1) {
        m_previousPoint = wxDefaultPosition;

        OGRFeature* myFeat = GetNextFeature(false);
        if (myFeat == nullptr) {
            break;
        }

        if (IsFeatureHidden(myFeat->GetFID())) {
            OGRFeature::DestroyFeature(myFeat);
            myFeat = nullptr;
            continue;
        }

        OGRwkbGeometryType myGeomType = wkbFlatten(myFeat->GetGeometryRef()->getGeometryType());
        // multigeometries support
        if (myGeomType == wkbMultiPolygon || myGeomType == wkbMultiLineString || myGeomType == wkbMultiPoint) {
            OGRGeometryCollection* myCollection = (OGRGeometryCollection*)myFeat->GetGeometryRef();
            for (unsigned int i = 0; i < myCollection->getNumGeometries(); i++) {
                m_previousPoint = wxDefaultPosition;
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
                        wxLogError("Geometry type of %s, feature: %ld isn't supported or defined (%d)",
                                   m_fileName.GetFullName(), myFeat->GetFID(), myGeomType);
                        OGRFeature::DestroyFeature(myFeat);
                        return false;
                }
            }

        } else {
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
                    wxLogError("Geometry type of %s, feature: %ld isn't supported or defined (%d)",
                               m_fileName.GetFullName(), myFeat->GetFID(), myGeomType);
                    OGRFeature::DestroyFeature(myFeat);
                    return false;
            }
        }
        OGRFeature::DestroyFeature(myFeat);
    }

    return bReturn;
}

bool vrLayerVectorOGR::SetAttributeFilter(const wxString& query) {
    wxASSERT(m_layer);
    if (query == wxEmptyString) {
        OGRErr myErr = m_layer->SetAttributeFilter(nullptr);
        if (myErr != OGRERR_NONE) {
            wxLogError(_("Error cleaning attribute filter: (code %d)"), myErr);
            return false;
        }
        m_layer->ResetReading();
        return true;
    }

    OGRErr myErr = m_layer->SetAttributeFilter((const char*)query.mb_str(wxConvUTF8));
    if (myErr != OGRERR_NONE) {
        wxLogError(_("Error setting following attribute filter : %s, (code %d)"), query, myErr);
        return false;
    }
    m_layer->ResetReading();
    return true;
}
