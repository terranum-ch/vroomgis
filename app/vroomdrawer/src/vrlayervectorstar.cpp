/***************************************************************************
 vrlayervectorstar.cpp

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

#include "vrlayervectorstar.h"

#include "vrlabel.h"
#include "vrrender.h"

void vrLayerVectorStar::_DrawPoint(wxDC* dc, OGRFeature* feature, OGRGeometry* geometry, const wxRect2DDouble& coord,
                                   const vrRender* render, vrLabel* label, double pxsize) {
    wxASSERT(render->GetType() == vrRENDER_VECTOR);
    vrRenderVector* myRender = (vrRenderVector*)render;
    wxPen myPen(myRender->GetColorPen(), myRender->GetSize());
    wxPen mySelPen(myRender->GetSelectionColour(), myRender->GetSize());
    dc->SetPen(myPen);
    if (IsFeatureSelected(feature->GetFID())) {
        dc->SetPen(mySelPen);
    }

    OGRPoint* myGeom = (OGRPoint*)geometry;
    wxPoint myPt = _GetPointFromReal(wxPoint2DDouble(myGeom->getX(), myGeom->getY()), coord.GetLeftTop(), pxsize);
    int myStarSize = feature->GetFieldAsInteger(0);

    // Create star
    wxPointList myPtx;
    _CreateStarPath(myPtx, myPt, myStarSize);
    if (myPtx.GetCount() < 2) {
        return;
    }
    dc->DrawLines(&myPtx);
    myPtx.DeleteContents(true);
    return;
}

void vrLayerVectorStar::_CreateStarPath(wxPointList& starpoints, const wxPoint& center, int radius) {
    const int myAngle = 72;
    const int myNumberPeak = 5;

    wxArrayInt myX;
    wxArrayInt myY;

    // computing peak coordinates
    int myActualAngle = 0;
    for (int i = 0; i < myNumberPeak; i++) {
        if (myActualAngle >= 360) {
            myActualAngle = 0;
        }
        double x = cos(_DegToRad(myActualAngle));
        double y = sin(_DegToRad(myActualAngle));
        myX.Add(wxRound(x * radius + center.x));
        myY.Add(wxRound(y * radius + center.y));
        myActualAngle = myActualAngle + myAngle;
    }

    // drawing star
    starpoints.push_back(new wxPoint(myX[0], myY[0]));
    int myPoint = 2;
    for (unsigned int i = 0; i < myX.GetCount(); i++) {
        starpoints.push_back(new wxPoint(myX[myPoint], myY[myPoint]));
        myPoint = myPoint + 2;
        if (myPoint >= myNumberPeak) {
            myPoint = myPoint - myNumberPeak;
        }
    }
}

vrLayerVectorStar::vrLayerVectorStar() {
    wxASSERT(m_dataset == NULL);
    wxASSERT(m_layer == NULL);
    m_driverType = vrDRIVER_VECTOR_MEMORY;
}

vrLayerVectorStar::~vrLayerVectorStar() {}

long vrLayerVectorStar::AddFeature(OGRGeometry* geometry, void* data) {
    wxASSERT(m_layer);
    OGRFeature* myFeature = OGRFeature::CreateFeature(m_layer->GetLayerDefn());
    wxASSERT(m_layer);
    myFeature->SetGeometry(geometry);

    if (data != NULL) {
        int* mySize = (int*)data;
        myFeature->SetField(0, *mySize);
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
