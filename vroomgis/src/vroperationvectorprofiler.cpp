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

vrOperationVectorProfiler::vrOperationVectorProfiler(OGRGeometry* geometry, vrLayerRasterGDAL* raster) {
    m_lineString = NULL;
    if (geometry->getGeometryType() == wkbFlatten(wkbLineString)) {
        m_lineString = static_cast<OGRLineString*>(geometry);
    }
    m_rasterLayer = raster;
    m_pixelWidth = 0;
    m_pixelHeight = 0;
    m_noDataValue = -9999;

    wxArrayDouble myGeoTransform;
    m_rasterLayer->GetGeoTransform(myGeoTransform);
    m_pixelWidth = myGeoTransform.Item(1);
    m_pixelHeight = myGeoTransform.Item(5);

    m_increment_X = fabs(m_pixelWidth);
    m_increment_Y = fabs(m_pixelHeight);
}

vrOperationVectorProfiler::~vrOperationVectorProfiler() {}

bool vrOperationVectorProfiler::IsOk() {
    if (m_lineString == NULL) {
        return false;
    }

    return m_rasterLayer != NULL;
}

bool vrOperationVectorProfiler::DoProfile(int bandindex) {
    if (!IsOk()) {
        return false;
    }

    // work only on two vertex lines
    if (m_lineString->getNumPoints() != 2) {
        wxLogError(_("Actually Profile works only on line with 2 vertex, found: %d vertex"),
                   m_lineString->getNumPoints());
        return false;
    }

    OGRPoint p1;
    OGRPoint p2;
    m_lineString->getPoint(0, &p1);
    m_lineString->getPoint(1, &p2);

    double dx = p2.getX() - p1.getX();
    double dy = p2.getY() - p1.getY();

    int myPointX = wxRound(fabs(dx / m_pixelWidth));
    int myPointY = wxRound(fabs(dy / m_pixelHeight));
    int iNbPoints = MAX(myPointX, myPointY);

    if (myPointX == 0 && myPointY == 0) {
        wxLogError(_("Unable to create profile, line is too small"));
        return false;
    }

    m_increment_X = fabs(m_pixelWidth);
    m_increment_Y = fabs(m_pixelHeight);

    if (iNbPoints != 0) {
        if (iNbPoints == myPointX) {
            m_increment_Y = dy / iNbPoints;
        } else {
            m_increment_X = dx / iNbPoints;
        }
    }

    if (dx < 0 && m_increment_X > 0) {
        m_increment_X = m_increment_X * -1.0;
    }
    if (dy < 0 && m_increment_Y > 0) {
        m_increment_Y = m_increment_Y * -1.0;
    }

    wxArrayDouble myValues;
    for (int i = 0; i < iNbPoints + 1; i++) {
        double myX = p1.getX() + (i * m_increment_X);
        double myY = p1.getY() + (i * m_increment_Y);
        m_rasterLayer->GetPixelValue(myX, myY, myValues);
        if (myValues.GetCount() == 0) {
            m_zResults.Add(m_noDataValue);
            continue;
        }
        double myZVal = myValues.Item(bandindex);
        // wxLogMessage(_("%f"), myZVal);
        m_zResults.Add(myZVal);
    }

    return true;
}

bool vrOperationVectorProfiler::GetResultPoint(int index, OGRPoint* point) {
    if (!IsOk()) {
        return false;
    }

    if (m_zResults.GetCount() == 0) {
        return false;
    }

    if (index < 0 || index >= m_zResults.GetCount()) {
        wxLogError(_("Index out of bounds!"));
        return false;
    }

    if (point == NULL) {
        wxLogError(_("Point Geometry is NULL!"));
        return false;
    }

    point->setX(m_lineString->getX(0) + index * GetIncrementX());
    point->setY(m_lineString->getY(0) + index * GetIncrementY());
    point->setZ(m_zResults.Item(index));
    return true;
}

bool vrOperationVectorProfiler::GetResultLine(OGRGeometry* line) {
    if (!IsOk()) {
        return false;
    }

    if (m_zResults.GetCount() == 0) {
        return false;
    }

    if (line == NULL) {
        wxLogError(_("Line Geometry is NULL!"));
        return false;
    }

    OGRLineString* myLine = static_cast<OGRLineString*>(line);
    for (unsigned int i = 0; i < m_zResults.GetCount(); i++) {
        OGRPoint* myPt = static_cast<OGRPoint*>(OGRGeometryFactory::createGeometry(wkbPoint));
        if (!GetResultPoint(i, myPt)) {
            wxLogError(_("Error getting point: %ld"), i);
            OGRGeometryFactory::destroyGeometry(myPt);
            continue;
        }

        myLine->addPoint(myPt);
    }

    return myLine;
}

double vrOperationVectorProfiler::GetIncrementDistance() {
    double myDistance = sqrt((m_increment_X * m_increment_X) + (m_increment_Y * m_increment_Y));
    return myDistance;
}
