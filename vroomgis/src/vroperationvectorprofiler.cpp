/***************************************************************************
 vroperationvectorprofiler.cpp
 -------------------
 copyright            : (C) 2013 CREALP Lucien Schreiber 
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

#include "vroperationvectorprofiler.h"
#include "vrlayerraster.h"

vrOperationVectorProfiler::vrOperationVectorProfiler(OGRGeometry * geometry, vrLayerRasterGDAL * raster) {
    m_LineString = NULL;
    if(geometry->getGeometryType() == wkbFlatten(wkbLineString)){
        m_LineString = static_cast<OGRLineString*>(geometry);
    }
    m_RasterLayer = raster;
    m_PixelWidth = 0;
    m_PixelHeight = 0;
    
    wxArrayDouble myGeoTransform;
    m_RasterLayer->GetGeoTransform(myGeoTransform);
    m_PixelWidth = myGeoTransform.Item(1);
    m_PixelHeight = myGeoTransform.Item(5);
}



vrOperationVectorProfiler::~vrOperationVectorProfiler() {
}



bool vrOperationVectorProfiler::IsOk() {
    if (m_LineString == NULL) {
        return false;
    }
    
    if (m_RasterLayer == NULL) {
        return false;
    }
    return true;
}


bool vrOperationVectorProfiler::DoProfile(int bandindex) {
    if (IsOk() == false) {
        return false;
    }
    
    // work only on two vertex lines
    if (m_LineString->getNumPoints() != 2) {
        wxLogError(_("Actually Profile works only on line with 2 vertex, found: %d vertex"), m_LineString->getNumPoints());
        return false;
    }
    
    OGRPoint p1;
    OGRPoint p2;
    m_LineString->getPoint(0, &p1);
    m_LineString->getPoint(1, &p2);
    
    double dx = p1.getX() - p2.getX();
    double dy = p1.getY() - p2.getY();
    
    int myPointX = wxRound(fabs(dx / m_PixelWidth));
    int myPointY = wxRound(fabs(dy / m_PixelHeight));
    int iNbPoints = MAX(myPointX, myPointY);
    
    if (myPointX == 0 && myPointY == 0) {
        wxLogError(_("Unable to create profile, line is too small"));
        return false;
    }
    
    double xstep = m_PixelWidth;
    double ystep = m_PixelHeight;
    
    if (iNbPoints  == myPointX) {
        ystep = dy / iNbPoints;
    } else {
        xstep = dx / iNbPoints;
    }
    
    wxArrayDouble myValues;
    for (int i = 0; i < iNbPoints; i++) {
        double myX = p1.getX() + (i* xstep);
        double myY = p1.getY() + (i* ystep);
        m_RasterLayer->GetPixelValue(myX, myY, myValues);
        if (myValues.GetCount() == 0) {
            //wxLogMessage(_("-----SKIPPED"));
            continue;
        }
        double myZVal = myValues.Item(bandindex);
        wxLogMessage(_("%f"), myZVal);
        m_ZResults.Add(myZVal);
    }
    
    return true;
}

