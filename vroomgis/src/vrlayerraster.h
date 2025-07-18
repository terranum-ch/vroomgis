/***************************************************************************
 vrlayerraster.h

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

#ifndef _VRLAYERRASTER_H
#define _VRLAYERRASTER_H

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
// Include wxWidgets' headers
#ifndef WX_PRECOMP

#include <wx/wx.h>

#endif

#include "vrlayer.h"

enum vrOVERVIEW_TYPE {
    vrOVERVIEW_NEAREST = 0,
    vrOVERVIEW_GAUSS,
    vrOVERVIEW_CUBIC,
    vrOVERVIEW_AVERAGE,
    vrOVERVIEW_MODE,
    vrOVERVIEW_AVERAGE_MAGPHASE,
    vrOVERVIEW_NONE
};

const wxString vrOVERVIEW_TYPE_NAME[] = {"NEAREST", "GAUSS", "CUBIC", "AVERAGE", "MODE", "AVERAGE_MAGPHASE", "NONE"};

class vrLayerRaster : public vrLayer {
  protected:
    GDALDataset* m_dataset;

  public:
    vrLayerRaster();

    virtual ~vrLayerRaster();

    virtual bool Create(const wxFileName& filename) {
        return false;
    }

    virtual bool Open(const wxFileName& filename, bool readwrite = false) {
        return false;
    }

    virtual bool GetExtent(vrRealRect& rect) {
        return false;
    }

    virtual wxSize GetPixelSize();

    bool GetGeoTransform(wxArrayDouble& geotransform);

    double GetPixelWidth();

    double GetPixelHeight();

    virtual bool IsOK();

    virtual bool HasData();

    GDALDataset* GetDatasetRef() {
        return m_dataset;
    }
};

class vrLayerRasterGDAL : public vrLayerRaster {
  protected:
    vrRealRect m_imgExtent;
    wxSize m_imgPxSize;

    // stat for one band data (Greyscale, MNT, etc)
    double m_oneBandMin;
    double m_oneBandMax;
    double m_oneBandNoData;

    bool _Close();

    bool _ComputeDisplayPosSize(const wxSize& pximgsize, const vrRealRect& imgextent, const vrRealRect& wndextent,
                                double pxsize, wxRect& pximginfo, wxRect& pximgpos);

    bool _ComputeStat();

    bool _HasStat();

    virtual bool _ComputeExtent();

    bool _HasExtent();

  protected:
    double _ReadGDALValueToDouble(void*& data, const GDALDataType& type, int index);

    virtual bool _GetRasterData(unsigned char** imgdata, const wxSize& outimgpxsize, const wxRect& readimgpxinfo,
                                const vrRender* render);

    virtual bool _GetRasterNoData(unsigned char** imgdata, const wxSize& outimgpxsize, const wxRect& readimgpxinfo,
                                  const vrRender* render);

  public:
    vrLayerRasterGDAL();

    virtual ~vrLayerRasterGDAL();

    virtual bool Create(const wxFileName& filename);

    virtual bool Open(const wxFileName& filename, bool readwrite = false);

    virtual bool GetExtent(vrRealRect& rect);

    virtual bool GetDataThread(wxImage* bmp, const vrRealRect& coord, double pxsize, const vrRender* render = nullptr,
                               vrLabel* label = nullptr);

    virtual bool GetData(wxBitmap* bmp, const vrRealRect& coord, double pxsize, const vrRender* render = nullptr,
                         vrLabel* label = nullptr);

    virtual bool GetPixelValue(double coordx, double coordy, wxArrayDouble& values);

    bool BuildOverviews(const wxArrayInt& factors, vrOVERVIEW_TYPE type = vrOVERVIEW_NEAREST,
                        GDALProgressFunc progress = nullptr, void* pfProgressData = nullptr);

    bool HasOverviews();

    bool GetMetadata(wxArrayString& names, wxArrayString& values, const wxString& domain = _T(""));
};

class vrLayerRasterEGRID : public vrLayerRasterGDAL {
  public:
    vrLayerRasterEGRID();

    virtual ~vrLayerRasterEGRID();

    virtual wxFileName GetDisplayName();
};

#endif
