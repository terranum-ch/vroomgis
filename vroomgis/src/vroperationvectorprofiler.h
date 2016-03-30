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

class vrOperationVectorProfiler
{
public:
    vrOperationVectorProfiler(OGRGeometry *geometry, vrLayerRasterGDAL *raster);

    virtual ~vrOperationVectorProfiler();

    bool IsOk();

    bool DoProfile(int bandindex = 0);

    wxArrayDouble *GetResultRef()
    {
        return &m_zResults;
    };

    bool GetResultPoint(int index, OGRPoint *point);

    bool GetResultLine(OGRGeometry *line);

    int GetNoDataValue()
    {
        return m_noDataValue;
    }

    void SetNoDataValue(int value)
    {
        m_noDataValue = value;
    }

    double GetIncrementX()
    {
        return m_increment_X;
    }

    double GetIncrementY()
    {
        return m_increment_Y;
    }

    double GetIncrementDistance();

protected:
    OGRLineString *m_lineString;
    vrLayerRasterGDAL *m_rasterLayer;
    double m_pixelWidth;
    double m_pixelHeight;
    int m_noDataValue;
    double m_increment_X;
    double m_increment_Y;

private:
    wxArrayDouble m_zResults;

};

#endif
