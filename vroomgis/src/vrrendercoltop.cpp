/***************************************************************************
 vrrendercoltop.cpp
                    
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

#include "vrrendercoltop.h"

vrRenderRasterColtop::vrRenderRasterColtop() {
    m_type = vrRENDER_RASTER_C2D;
    m_northAngle = 0;
    m_isColorInverted = false;
    m_isLowerHemisphere = true;
    m_colorStretchMin = 0;
    m_colorStretchMax = 90;
}

vrRenderRasterColtop::~vrRenderRasterColtop() {}

wxImage::RGBValue vrRenderRasterColtop::GetColorFromDipDir(double dip, double dipdir) {
    double myDir = dipdir;
    double myDip = dip;

    if (!m_isLowerHemisphere) {
        myDir = myDir + 180.0;
    }

    myDir = myDir + m_northAngle;
    while (myDir > 360.0) {
        myDir = myDir - 360.0;
    }
    while (myDir < 0) {
        myDir = myDir + 360;
    }
    if (wxIsSameDouble(myDir, 360.0)) {
        myDir = 0;
    }
    wxASSERT(myDir <= 360.0);

    // Stretch colors
    double myNDip = 0;
    if (myDip >= m_colorStretchMin && myDip <= m_colorStretchMax) {
        myNDip = (myDip - m_colorStretchMin) * 90.0 / (m_colorStretchMax - m_colorStretchMin) / 90.0;
    }

    double myNDir = myDir / 360.0;
    if (m_isColorInverted) {
        myNDir = 1.0 - myNDir;
    }

    wxImage::HSVValue myHSVValue(myNDir, myNDip, 1);
    wxImage::RGBValue myRGBValue = wxImage::HSVtoRGB(myHSVValue);
    return myRGBValue;
}

wxImage::RGBValue vrRenderRasterColtop::GetColorFromCircleCoord(const wxPoint& coord, int radius) {
    double myStretchRadius = radius;

    // STRETCHING COORDINATES
    double pixelbydegree = radius / 90.0;
    double minstretchradius = pixelbydegree * m_colorStretchMin;
    if (m_colorStretchMin > 0) {
        if (coord.x * coord.x + coord.y * coord.y < minstretchradius * minstretchradius) {
            return wxImage::RGBValue(255, 255, 255);
        }
    }

    if (m_colorStretchMax < 90) {
        double maxstretchradius = pixelbydegree * m_colorStretchMax;
        if (coord.x * coord.x + coord.y * coord.y >= maxstretchradius * maxstretchradius) {
            return wxImage::RGBValue(255, 255, 255);
        }
        myStretchRadius = maxstretchradius;
    }

    // HUE (Color rotation)
    double myHue = 0;
    if (!m_isColorInverted) {
        myHue = 180.0 * (1.0 + atan2(wxDouble(coord.x), wxDouble(coord.y)) * (1 / M_PI));
        myHue = myHue + m_northAngle;
    } else {
        myHue = 180.0 * (1.0 - atan2(wxDouble(coord.x), wxDouble(coord.y)) * (1 / M_PI));
        myHue = myHue - m_northAngle;
    }
    while (myHue > 360.0) {
        myHue = myHue - 360.0;
    }
    while (myHue < 0) {
        myHue = myHue + 360.0;
    }
    if (wxIsSameDouble(myHue, 360.0)) {
        myHue = 0;
    }

    // SATURATION (Color intensity)
    int myRadius = sqrt(wxDouble(coord.x * coord.x + coord.y * coord.y));
    if (myRadius > myStretchRadius) {
        myRadius = myStretchRadius;
    }
    if (myRadius > minstretchradius) {
        myRadius = (myRadius - minstretchradius) * myStretchRadius / (myStretchRadius - minstretchradius);
    } else {
        myRadius = 0;
    }

    double mySature = (2.0 * asin(sqrt(2.0) * 0.5 * myRadius * (1.0 / myStretchRadius))) * (1.0 / M_PI) * 2.0;
    wxImage::HSVValue myHSVValue(myHue / 360.0, mySature, 1);
    wxImage::RGBValue myRGBValue = wxImage::HSVtoRGB(myHSVValue);
    return myRGBValue;
}

void vrRenderRasterColtop::SetNorthAngle(int value) {
    m_northAngle = 360.0 - value;
}

void vrRenderRasterColtop::SetColorInverted(bool value) {
    m_isColorInverted = value;
}

void vrRenderRasterColtop::SetLowerHemisphere(bool value) {
    m_isLowerHemisphere = value;
}

void vrRenderRasterColtop::SetColorStretchMin(int value) {
    m_colorStretchMin = value;
}

void vrRenderRasterColtop::SetColorStretchMax(int value) {
    m_colorStretchMax = value;
}

bool vrRenderRasterColtop::Serialize(vrSerialize& serialobj) {
    return false;
}
