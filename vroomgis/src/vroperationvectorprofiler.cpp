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
    m_NoDataValue = -9999;
    
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
    
    double dx = p2.getX() - p1.getX();
    double dy = p2.getY() - p1.getY();
    
    int myPointX = wxRound(fabs(dx / m_PixelWidth));
    int myPointY = wxRound(fabs(dy / m_PixelHeight));
    int iNbPoints = MAX(myPointX, myPointY);
    
    if (myPointX == 0 && myPointY == 0) {
        wxLogError(_("Unable to create profile, line is too small"));
        return false;
    }
    
    m_Increment_X = fabs(m_PixelWidth);
    m_Increment_Y = fabs(m_PixelHeight);
    
    if (iNbPoints  == myPointX) {
        m_Increment_Y = dy / iNbPoints;        
    } else {
        m_Increment_X = dx / iNbPoints;
    }
    
    if (dx < 0 && m_Increment_X > 0) {
        m_Increment_X = m_Increment_X * -1.0;
    }
    if (dy < 0 && m_Increment_Y > 0) {
        m_Increment_Y = m_Increment_Y * -1.0;
    }
    
    
    wxArrayDouble myValues;
    for (int i = 0; i < iNbPoints + 1; i++) {
        double myX = p1.getX() + (i* m_Increment_X);
        double myY = p1.getY() + (i* m_Increment_Y);
        m_RasterLayer->GetPixelValue(myX, myY, myValues);
        if (myValues.GetCount() == 0) {
            m_ZResults.Add(m_NoDataValue);
            continue;
        }
        double myZVal = myValues.Item(bandindex);
        //wxLogMessage(_("%f"), myZVal);
        m_ZResults.Add(myZVal);
    }
    
    return true;
}



bool vrOperationVectorProfiler::GetResultPoint(int index, OGRPoint * point) {
    if (IsOk() == false) {
        return false;
    }
    
    if (m_ZResults.GetCount() == 0) {
        return false;
    }
    
    if (index < 0 || index >= m_ZResults.GetCount()) {
        wxLogError(_("Index out of bounds!"));
        return false;
    }
    
    if (point == NULL) {
        wxLogError(_("Point Geometry is NULL!"));
        return false;
    }
    
    point->setX(m_LineString->getX(0) + index * GetIncrementX());
    point->setY(m_LineString->getY(0) + index * GetIncrementY());
    point->setZ(m_ZResults.Item(index));
    return true;
}



bool vrOperationVectorProfiler::GetResultLine(OGRGeometry * line) {
    if (IsOk() == false) {
        return false;
    }
    
    if (m_ZResults.GetCount() == 0) {
        return false;
    }
        
    if (line == NULL) {
        wxLogError(_("Line Geometry is NULL!"));
        return false;
    }
    
    OGRLineString * myLine = static_cast<OGRLineString*>(line);
    for (unsigned int i = 0; i< m_ZResults.GetCount(); i++) {
        OGRPoint * myPt = static_cast<OGRPoint*>(OGRGeometryFactory::createGeometry(wkbPoint));
        if(GetResultPoint(i, myPt) == false){
            wxLogError(_("Error getting point: %ld"), i);
            OGRGeometryFactory::destroyGeometry(myPt);
            continue;
        }
        
        myLine->addPoint(myPt);
    }

    return myLine;
}


double vrOperationVectorProfiler::GetIncrementDistance() {
    double myDistance = sqrt( (m_Increment_X * m_Increment_X) + (m_Increment_Y * m_Increment_Y) );
    return myDistance;
}



