/***************************************************************************
 vroperationvectorprofiler.h
 -------------------
 copyright            : (C) Lucien Schreiber
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
#ifndef _VROPERATIONVECTORPROFILER_H_
#define _VROPERATIONVECTORPROFILER_H_

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include "ogrsf_frmts.h"

class vrLayerRasterGDAL;

class vrOperationVectorProfiler {
public:
    vrOperationVectorProfiler(OGRGeometry * geometry, vrLayerRasterGDAL * raster);
    virtual ~vrOperationVectorProfiler();
    bool IsOk();
    
    bool DoProfile(int bandindex = 0);
    
    wxArrayDouble * GetResultRef() {return &m_ZResults; };
    bool GetResultPoint(int index, OGRPoint * point);
    bool GetResultLine(OGRGeometry * line);
    
    int GetNoDataValue() {return m_NoDataValue;}
    void SetNoDataValue(int value) {m_NoDataValue = value;}
    
    double GetIncrementX() {return m_Increment_X;}
    double GetIncrementY() {return m_Increment_Y;}
    double GetIncrementDistance();
    
protected:
    OGRLineString * m_LineString;
    vrLayerRasterGDAL * m_RasterLayer;
    double m_PixelWidth;
    double m_PixelHeight;
    int m_NoDataValue;
    double m_Increment_X;
    double m_Increment_Y;
    
private:
    wxArrayDouble m_ZResults;
    
};

#endif
