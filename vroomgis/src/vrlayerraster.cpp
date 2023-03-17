/***************************************************************************
 vrlayerraster.cpp

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

#include "vrlayerraster.h"

#include <wx/rawbmp.h>

#include "vrrender.h"  // for symbology;

vrLayerRaster::vrLayerRaster() {
    m_dataset = NULL;
}

vrLayerRaster::~vrLayerRaster() {}

bool vrLayerRaster::IsOK() {
    if (m_dataset == NULL) {
        wxLogError("Dataset not inited");
        return false;
    }
    return true;
}

// this function is mostly for vector layers.
// raster have almost allways data
bool vrLayerRaster::HasData() {
    return IsOK();
}

bool vrLayerRasterGDAL::_Close() {
    m_oneBandMin = 0;
    m_oneBandMax = 0;
    m_oneBandNoData = 0;

    if (m_dataset == NULL) {
        return false;
    }

    // wxLogMessage("Closing raster data NOW");
    GDALClose(m_dataset);
    m_dataset = NULL;
    return true;
}

wxSize vrLayerRaster::GetPixelSize() {
    wxSize mySize = wxDefaultSize;
    if (m_dataset != NULL) {
        mySize.SetWidth(m_dataset->GetRasterXSize());
        mySize.SetHeight(m_dataset->GetRasterYSize());
    }
    return mySize;
}

bool vrLayerRaster::GetGeoTransform(wxArrayDouble& geotransform) {
    geotransform.Clear();
    if (m_dataset == NULL) {
        return false;
    }

    double myTransform[] = {0, 0, 0, 0, 0, 0};
    if (m_dataset->GetGeoTransform(&myTransform[0]) != CE_None) {
        wxLogError(_("Error getting geotransform data!"));
        return false;
    }

    for (int i = 0; i < 6; i++) {
        geotransform.Add(myTransform[i]);
    }
    return true;
}

double vrLayerRaster::GetPixelWidth() {
    wxArrayDouble myGeotransform;
    GetGeoTransform(myGeotransform);
    return myGeotransform[1];
}

double vrLayerRaster::GetPixelHeight() {
    wxArrayDouble myGeotransform;
    GetGeoTransform(myGeotransform);
    return myGeotransform[5];
}

/***************************************************************************/ /**
 @brief Compute all position info for displaying a raster
 @param pximgsize Size of image in pixels stored on disk
 @param imgextent Coordinate Extent of the raster
 @param wndextent Coordinate extent of the Window
 @param pxsize Pixel conversion Reality - Window
 @param pximginfo Part of the raster to read in pixels
 @param pximgpos Position and size of the raster for displaying it on the window
 @return  true if computing was successfull
 @author Lucien Schreiber (c) CREALP 2010
 @date 17 mars 2010
  *******************************************************************************/
bool vrLayerRasterGDAL::_ComputeDisplayPosSize(const wxSize& pximgsize, const vrRealRect& imgextent,
                                               const vrRealRect& wndextent, double pxsize, wxRect& pximginfo,
                                               wxRect& pximgpos) {
    // get intersection between display and img
    vrRealRect myWndExtentTemp = wndextent;
    wxASSERT(myWndExtentTemp == wndextent);

    pximginfo = wxRect(0, 0, 0, 0);
    pximgpos = wxRect(0, 0, 0, 0);

    vrRealRect myIntersect = myWndExtentTemp.Intersect(imgextent);
    if (!myIntersect.IsOk()) {
        wxLogMessage("Image out of the dislay, intersection is null");
        return false;
    }

    // width of image to display (in pixels)
    int pxWidthVisible = wxRound(myIntersect.m_width * pximgsize.GetWidth() / imgextent.m_width);
    int pxHeightVisible = wxRound(myIntersect.m_height * pximgsize.GetHeight() / imgextent.m_height);

    // starting position from where we get image data (px)
    int ximg = wxRound((myIntersect.GetLeft() - imgextent.GetLeft()) * pximgsize.GetWidth() / imgextent.m_width);
    int yimg = wxRound((myIntersect.GetTop() - imgextent.GetTop()) * pximgsize.GetHeight() / imgextent.m_height);

    // position for displaying the bitmap (in pixels)
    int vx = wxRound((myIntersect.GetLeft() - wndextent.GetLeft()) / pxsize);
    int vy = wxRound((wndextent.GetTop() - myIntersect.GetTop()) / pxsize);

    // pixels size of displayed bitmap
    int vw = wxRound(fabs(myIntersect.m_width / pxsize));
    int vh = wxRound(fabs(myIntersect.m_height / pxsize));

    // returning values
    pximginfo.SetTopLeft(wxPoint(ximg, yimg));
    pximginfo.width = pxWidthVisible;
    pximginfo.height = pxHeightVisible;

    pximgpos.x = vx;
    pximgpos.y = vy;
    pximgpos.SetWidth(vw);
    pximgpos.SetHeight(vh);

    if (pximginfo.IsEmpty()) {
        wxLogMessage("Image is outside the display.");
        return false;
    }

    return true;
}

bool vrLayerRasterGDAL::_GetRasterData(unsigned char** imgdata, const wxSize& outimgpxsize, const wxRect& readimgpxinfo,
                                       const vrRender* render) {
    wxASSERT(m_dataset);
    m_dataset->FlushCache();
    int myRasterCount = m_dataset->GetRasterCount();

    // check image size bigger than 0
    if (outimgpxsize.GetWidth() <= 0 || outimgpxsize.GetHeight() <= 0) {
        wxLogError("Dimentions of the image are invalid (%d, %d)", outimgpxsize.GetWidth(), outimgpxsize.GetHeight());
        return false;
    }

    // create array for image data (RGBRGBRGB...)
    unsigned int myimgLen = outimgpxsize.GetWidth() * outimgpxsize.GetHeight() * 3;
    *imgdata = (unsigned char*)malloc(myimgLen);
    if (*imgdata == NULL) {
        wxLogError("Image creation failed, out of memory");
        return false;
    }

    //
    // one band is either a palette based image, or greyscale
    //
    if (myRasterCount == 1) {
        double myRange = 0;
        GDALDataType myDataType;
        int myScanSize = 0;
        void* myScanData = NULL;
        GDALColorTable* pal = NULL;
        GDALPaletteInterp pal_interp;
        GDALRasterBand* band = m_dataset->GetRasterBand(1);

        switch (band->GetColorInterpretation()) {
            case GCI_PaletteIndex:

                pal = band->GetColorTable();
                if (pal == NULL) {
                    wxLogError("Couldn't find a palette for palette-based image");
                    if (*imgdata != NULL) {
                        CPLFree(*imgdata);
                        *imgdata = NULL;
                    }
                    return false;
                }

                pal_interp = pal->GetPaletteInterpretation();

                //
                // copy over all the palette indices and then
                // loop through the buffer replacing the values
                // with the correct RGB triples.
                //
                if (band->RasterIO(GF_Read, readimgpxinfo.GetX(), readimgpxinfo.GetY(), readimgpxinfo.GetWidth(),
                                   readimgpxinfo.GetHeight(), *imgdata, outimgpxsize.GetWidth(),
                                   outimgpxsize.GetHeight(), GDT_UInt16, 3, 0) != CE_None) {
                    wxLogError("Unknown error occured while reading palette based raster %s", m_fileName.GetFullName());
                    if (*imgdata != NULL) {
                        CPLFree(*imgdata);
                        *imgdata = NULL;
                    }
                    return false;
                }

                for (unsigned char* data = *imgdata; data != (*imgdata + myimgLen); data += 3) {
                    unsigned short int val = *((unsigned short int*)data);
                    const GDALColorEntry* color = pal->GetColorEntry(val);
                    if (pal_interp == GPI_Gray) {
                        *(data + 0) = color->c1;
                        *(data + 1) = color->c1;
                        *(data + 2) = color->c1;
                    } else {
                        *(data + 0) = color->c1;
                        *(data + 1) = color->c2;
                        *(data + 2) = color->c3;
                    }
                }

                break;

            case GCI_Undefined:  // can we try to make a greyscale image?
            case GCI_GrayIndex:

                // computing statistics if not existing
                if (!_HasStat()) {
                    if (!_ComputeStat()) {
                        if (*imgdata != NULL) {
                            CPLFree(*imgdata);
                            *imgdata = NULL;
                        }
                        return false;
                    }
                }

                myRange = m_oneBandMax - m_oneBandMin;
                if (myRange <= 0) {
                    myRange = 1;
                }
                // wxASSERT(myRange > 0);
                myDataType = band->GetRasterDataType();

                // getting monoband image
                myScanSize = GDALGetDataTypeSize(myDataType) / 8;
                myScanData = CPLMalloc(myScanSize * outimgpxsize.GetWidth() * outimgpxsize.GetHeight());
                wxASSERT(myScanData);
                if (band->RasterIO(GF_Read, readimgpxinfo.GetX(), readimgpxinfo.GetY(), readimgpxinfo.GetWidth(),
                                   readimgpxinfo.GetHeight(), myScanData, outimgpxsize.GetWidth(),
                                   outimgpxsize.GetHeight(), myDataType, 0, 0) != CE_None) {
                    // if reading doesn't work, clean and exit.
                    wxLogError("Error gettign monoband raster, maybe out of memory");
                    if (myScanData != NULL) {
                        CPLFree(myScanData);
                        myScanData = NULL;
                    }
                    if (*imgdata != NULL) {
                        CPLFree(*imgdata);
                        *imgdata = NULL;
                    }
                    return false;
                }

                // transform monoband in RGB
                for (unsigned int i = 0; i < myimgLen; i += 3) {
                    double myGrayValDouble = _ReadGDALValueToDouble(myScanData, myDataType, i / 3);
                    // set nodata values to white!
                    // TODO: take nodata into account
                    int myGrayValInt = 255;
                    if (!wxIsSameDouble(myGrayValDouble, m_oneBandNoData)) {
                        myGrayValInt = static_cast<int>((myGrayValDouble - m_oneBandMin) * (255 / myRange));
                    }

                    if (myGrayValInt < 0) myGrayValInt = 0;
                    if (myGrayValInt > 255) myGrayValInt = 255;

                    *(*imgdata + i) = myGrayValInt;
                    *(*imgdata + i + 1) = myGrayValInt;
                    *(*imgdata + i + 2) = myGrayValInt;
                }
                wxASSERT(myScanData != NULL);
                CPLFree(myScanData);
                myScanData = NULL;
                break;

            default:  // unsupported case

                wxLogError("Unsupported color interpretation %s",
                           GDALGetColorInterpretationName(band->GetColorInterpretation()));
                if (*imgdata != NULL) {
                    CPLFree(*imgdata);
                    *imgdata = NULL;
                }
                return false;
                break;
        }
    }

    //
    // if there are three or more banm_dataSet assume that the first three
    // are for RGB, unless told otherwise
    //
    if (myRasterCount >= 3) {
        for (int i = 1; i <= 3; i++) {
            int offs = 0;
            GDALRasterBand* band = m_dataset->GetRasterBand(i);

            switch (band->GetColorInterpretation()) {
                case GCI_Undefined:
                    offs = i - 1;
                    break;
                case GCI_RedBand:
                    offs = 0;
                    break;
                case GCI_GreenBand:
                    offs = 1;
                    break;
                case GCI_BlueBand:
                    offs = 2;
                    break;
                default:
                    offs = -1;
                    break;
            }

            //
            // copy the image into the buffer using the proper offset
            // so we first copy over all Red values, then all Green
            // values, and then all Blue values
            //

            if (0 <= offs && offs < 3) {
                if (band->RasterIO(GF_Read, readimgpxinfo.GetX(), readimgpxinfo.GetY(), readimgpxinfo.GetWidth(),
                                   readimgpxinfo.GetHeight(), *imgdata + offs, outimgpxsize.GetWidth(),
                                   outimgpxsize.GetHeight(), GDT_Byte, 3, 0) != CE_None) {
                    wxLogError(_T("Unknown error while reading band %i from %s"), i, m_fileName.GetFullName());
                    if (*imgdata != NULL) {
                        CPLFree(*imgdata);
                        *imgdata = NULL;
                    }
                    return false;
                }
            }
        }
    }

    return true;
}

bool vrLayerRasterGDAL::_GetRasterNoData(unsigned char** imgdata, const wxSize& outimgpxsize,
                                         const wxRect& readimgpxinfo, const vrRender* render) {
    wxASSERT(m_dataset);
    m_dataset->FlushCache();
    // int myRasterCount  = m_dataset->GetRasterCount();

    // check image size bigger than 0
    if (outimgpxsize.GetWidth() <= 0 || outimgpxsize.GetHeight() <= 0) {
        wxLogError("Dimentions of the image are invalid (%d, %d)", outimgpxsize.GetWidth(), outimgpxsize.GetHeight());
        return false;
    }

    // computing statistics if not existing
    if (!_HasStat()) {
        if (!_ComputeStat()) {
            return false;
        }
    }

    /*double myRange = 0;


    void * myScanData = NULL;
    GDALColorTable *pal = NULL;
    GDALPaletteInterp pal_interp;*/

    GDALRasterBand* band = m_dataset->GetRasterBand(1)->GetMaskBand();
    wxASSERT(band);
    int myMaskFlag = m_dataset->GetRasterBand(1)->GetMaskFlags();
    if (myMaskFlag == GMF_ALL_VALID) {
        wxLogMessage("this raster hasn't nodata values");
        return false;
    }

    // create array for image data (RGBRGBRGB...)
    /*unsigned int myimgLen = outimgpxsize.GetWidth() * outimgpxsize.GetHeight();
    *imgdata = (unsigned char *) malloc(myimgLen);
    if (*imgdata == NULL) {
        wxLogError("Image creation failed, out of memory");
        return false;
    }*/

    GDALDataType myDataType;
    int myScanSize = 0;
    myDataType = band->GetRasterDataType();
    myScanSize = GDALGetDataTypeSize(myDataType) / 8;
    *imgdata = (unsigned char*)malloc(myScanSize * outimgpxsize.GetWidth() * outimgpxsize.GetHeight());
    if (*imgdata == NULL) {
        wxLogError("Alpha Image creation failed, out of memory");
        return false;
    }

    /*for (int i = 0; i< myScanSize * outimgpxsize.GetWidth() * outimgpxsize.GetHeight(); i++) {
        *(*imgdata + i) = 0;
        if (i >= (myScanSize * outimgpxsize.GetWidth() * outimgpxsize.GetHeight()) / 2) {
            *(*imgdata + i) = 255;
        }

    }
    return true;*/

    if (band->RasterIO(GF_Read, readimgpxinfo.GetX(), readimgpxinfo.GetY(), readimgpxinfo.GetWidth(),
                       readimgpxinfo.GetHeight(), *imgdata, outimgpxsize.GetWidth(), outimgpxsize.GetHeight(),
                       myDataType, 0, 0) != CE_None) {
        // if reading doesn't work, clean and exit.
        wxLogError("Error gettign Alpha raster, maybe out of memory");
        if (*imgdata != NULL) {
            CPLFree(*imgdata);
            *imgdata = NULL;
        }
        return false;
    }
    return true;
}

double vrLayerRasterGDAL::_ReadGDALValueToDouble(void*& data, const GDALDataType& type, int index) {
    switch (type) {
        case GDT_Byte:
            return (double)((GByte*)data)[index];
            break;
        case GDT_UInt16:
            return (double)((GUInt16*)data)[index];
            break;
        case GDT_Int16:
            return (double)((GInt16*)data)[index];
            break;
        case GDT_UInt32:
            return (double)((GUInt32*)data)[index];
            break;
        case GDT_Int32:
            return (double)((GInt32*)data)[index];
            break;
        case GDT_Float32:
            return (double)((float*)data)[index];
            break;
        case GDT_Float64:
            return (double)((double*)data)[index];
            break;
        default:
            wxLogError("GDAL data type is not supported");
    }
    return 0.0;
}

bool vrLayerRasterGDAL::_ComputeStat() {
    int bResultMin = 0;
    int bResultMax = 0;
    int bResultNoData = 0;
    // bool bReturn = true;

    m_oneBandMin = 0;
    m_oneBandMax = 0;
    m_oneBandNoData = 0;

    GDALRasterBand* myRasterBand = m_dataset->GetRasterBand(1);
    m_oneBandMin = myRasterBand->GetMinimum(&bResultMin);
    m_oneBandMax = myRasterBand->GetMaximum(&bResultMax);
    m_oneBandNoData = myRasterBand->GetNoDataValue(&bResultNoData);

    if (bResultNoData == 0) {
        m_oneBandNoData = 0;
    }

    if (bResultMin == 1 && bResultMax == 1) {
        return true;
    }

    // try computing statistics (take more time...)
    if (myRasterBand->ComputeStatistics(true, &m_oneBandMin, &m_oneBandMax, NULL, NULL, NULL, NULL) != CE_None) {
        wxLogWarning(_("Getting information for raster ‘%s’ failed!\nThis image is either empty or corrupted."),
                     m_fileName.GetFullName());
        return false;
    }
    return true;
}

bool vrLayerRasterGDAL::_HasStat() {
    return !(m_oneBandMin == 0 || m_oneBandMax == 0);
}

bool vrLayerRasterGDAL::_HasExtent() {
    if (m_dataset == NULL) {
        wxLogError("No layer opened");
        return false;
    }

    if (!m_imgExtent.IsOk()) {
        return false;
    }

    if (m_imgPxSize == wxDefaultSize) {
        wxLogError("Image size (pixels) isn't defined correctly");
        return false;
    }

    return true;
}

bool vrLayerRasterGDAL::_ComputeExtent() {
    if (m_dataset == NULL) {
        wxLogError("No layer opened");
        return false;
    }

    wxASSERT(m_dataset);
    double adfGeoTransform[6];
    if (m_dataset->GetGeoTransform(adfGeoTransform) != CE_None) {
        wxLogWarning("Error getting geometric informations for layer : %s", m_fileName.GetFullName());
        // return false;
    }

    if (adfGeoTransform[5] > 0) {
        wxLogMessage("vertical pixel size is Wrong (should be negative) : %.2f", adfGeoTransform[5]);
        adfGeoTransform[5] = -1.0 * adfGeoTransform[5];
    }

    // top left corner
    wxPoint2DDouble mytopleft;
    mytopleft.m_x = adfGeoTransform[0] + adfGeoTransform[1] * 0.0 + adfGeoTransform[2] * 0.0;
    mytopleft.m_y = adfGeoTransform[3] + adfGeoTransform[4] * 0.0 + adfGeoTransform[5] * 0.0;

    // bottom left corner
    wxPoint2DDouble mybottomleft;
    mybottomleft.m_x = adfGeoTransform[0] + adfGeoTransform[1] * 0.0 + adfGeoTransform[2] * m_dataset->GetRasterYSize();
    mybottomleft.m_y = adfGeoTransform[3] + adfGeoTransform[4] * 0.0 + adfGeoTransform[5] * m_dataset->GetRasterYSize();

    // top right corner
    wxPoint2DDouble mytopright;
    mytopright.m_x = adfGeoTransform[0] + adfGeoTransform[1] * m_dataset->GetRasterXSize() + adfGeoTransform[2] * 0.0;
    mytopright.m_y = adfGeoTransform[3] + adfGeoTransform[4] * m_dataset->GetRasterXSize() + adfGeoTransform[5] * 0.0;

    // bottom right corner
    wxPoint2DDouble mybottomright;
    mybottomright.m_x = adfGeoTransform[0] + adfGeoTransform[1] * m_dataset->GetRasterXSize() +
                        adfGeoTransform[2] * m_dataset->GetRasterYSize();
    mybottomright.m_y = adfGeoTransform[3] + adfGeoTransform[4] * m_dataset->GetRasterXSize() +
                        adfGeoTransform[5] * m_dataset->GetRasterYSize();

    // compute extent
    // left
    double myleft = mytopleft.m_x;
    if (mybottomleft.m_x < mytopleft.m_x) {
        myleft = mybottomleft.m_x;
    }
    m_imgExtent.SetLeft(myleft);

    // right
    double myright = mytopright.m_x;
    if (mybottomright.m_x > mytopright.m_x) {
        myright = mybottomright.m_x;
    }
    m_imgExtent.SetRight(myright);

    // top
    double mytop = mytopleft.m_y;
    if (mytopright.m_y > mytopleft.m_y) {
        mytop = mytopright.m_y;
    }
    m_imgExtent.SetTop(mytop);

    // bottom
    double mybottom = mybottomleft.m_y;
    if (mybottomright.m_y < mybottomleft.m_y) {
        mybottom = mybottomright.m_y;
    }
    m_imgExtent.SetBottom(mybottom);

    m_imgPxSize = wxSize(m_dataset->GetRasterXSize(), m_dataset->GetRasterYSize());

    if (adfGeoTransform[2] != 0 || adfGeoTransform[4] != 0) {
        wxLogWarning("Layer %s contain following rotation informations (%.4f, %.4f). It may not be displayed correctly",
                     m_fileName.GetFullName(), adfGeoTransform[2], adfGeoTransform[4]);
    }

    return true;
}

vrLayerRasterGDAL::vrLayerRasterGDAL() {
    m_oneBandMin = 0;
    m_oneBandMax = 0;
    m_oneBandNoData = 0;
}

vrLayerRasterGDAL::~vrLayerRasterGDAL() {
    _Close();
}

bool vrLayerRasterGDAL::Create(const wxFileName& filename) {
    return false;
}

bool vrLayerRasterGDAL::Open(const wxFileName& filename, bool readwrite) {
    // try to close
    _Close();
    wxASSERT(m_dataset == NULL);

    // init filename and type
    m_fileName = filename;
    vrDrivers myDriver;
    m_driverType = myDriver.GetType(filename.GetExt());

    GDALAccess myAcces = GA_ReadOnly;
    if (readwrite) myAcces = GA_Update;

    m_dataset = (GDALDataset*)GDALOpen(filename.GetFullPath(), myAcces);
    if (m_dataset == NULL) {
        wxLogError("Unable to open %s, maybe driver not registered -  GDALAllRegister()\nGDAL Error: '%s'",
                   filename.GetFullName(), wxString(CPLGetLastErrorMsg()));

        return false;
    }

    // load aux.xml informations
    m_dataset->SetDescription(filename.GetFullPath());
    return true;
}

bool vrLayerRasterGDAL::GetExtent(vrRealRect& rect) {
    // if extent exist : return it. otherwise, compute it
    if (_HasExtent()) {
        rect = m_imgExtent;
        return true;
    }

    if (_ComputeExtent()) {
        rect = m_imgExtent;
        return true;
    }

    wxLogError("Unable to compute the layer %s extent", m_fileName.GetFullName());
    return false;
}

bool vrLayerRasterGDAL::GetDataThread(wxImage* bmp, const vrRealRect& coord, double pxsize, const vrRender* render,
                                      vrLabel* label) {
    // get extent of raster
    vrRealRect myImgExtent;
    if (!GetExtent(myImgExtent)) {
        return false;
    }

    // compute visible part, position for raster
    wxRect myImgInfo;
    wxRect myImgPos;

    if (!_ComputeDisplayPosSize(m_imgPxSize, myImgExtent, coord, pxsize, myImgInfo, myImgPos)) {
        wxLogMessage("Raster %s invalid. Maybe outside the display", m_fileName.GetFullName());
        return false;
    }

    // raster inside display
    unsigned char* myimgdata = NULL;
    if (!_GetRasterData(&myimgdata, wxSize(myImgPos.GetWidth(), myImgPos.GetHeight()), myImgInfo, render)) {
        wxASSERT(myimgdata == NULL);
        return false;
    }

    wxImage myImg(myImgPos.GetWidth(), myImgPos.GetHeight());
    myImg.SetData(myimgdata, false);
    if (!myImg.IsOk()) {
        wxLogError("Creating raster failed");
        return false;
    }

    unsigned char* mynodata = NULL;
    if (_GetRasterNoData(&mynodata, wxSize(myImgPos.GetWidth(), myImgPos.GetHeight()), myImgInfo, render)) {
        wxASSERT(mynodata != NULL);
        myImg.SetAlpha(mynodata, false);
    }

    // user transparency
    int myUserTransparency = 255 - (render->GetTransparency() * 255 / 100);
    if (myUserTransparency != 255) {
        wxImagePixelData data(myImg);
        wxImagePixelData::Iterator row(data);
        for (int y = 0; y < myImgPos.GetHeight(); y++) {
            wxImagePixelData::Iterator col = row;
            for (int x = 0; x < myImgPos.GetWidth(); x++, ++row) {
                if (row.Alpha() != 0) {
                    row.Alpha() = myUserTransparency;
                }
            }
            row = col;
            row.OffsetY(data, 1);
        }
    }

    // drawing the image on the passed bmp
    wxBitmap myBmp(bmp->GetSize());
    wxMemoryDC dc(myBmp);
    // dc.Clear();

    dc.DrawBitmap(myImg, myImgPos.GetX(), myImgPos.GetY(), true);
    dc.SelectObject(wxNullBitmap);
    *bmp = myBmp.ConvertToImage();

    //  transparency problem under linux
#if __LINUX__
    // set image transparency. Image is fully transparent, except for pixels where bitmap
    // was drawn. Those are set the the user defined transparency
    char myBackgroundTransparency = 0;
    // int myUserTransparency = 255 - (render->GetTransparency() * 255 / 100);
    unsigned char* alphachar = NULL;

    unsigned int myimglen = bmp->GetWidth() * bmp->GetHeight();
    alphachar = (unsigned char*)malloc(myimglen);
    if (alphachar == NULL) {
        wxLogError(_T("Error creating translucency"));
        return false;
    }

    for (unsigned int i = 0; i < myimglen; i++) {
        *(alphachar + i) = myBackgroundTransparency;
    }
    if (!bmp->HasAlpha()) {
        bmp->InitAlpha();
        // wxLogMessage("Initing alpha");
    }
    bmp->SetAlpha(alphachar, false);

    if (myImgPos.GetHeight() + myImgPos.GetY() > bmp->GetHeight()) {
        myImgPos.height = bmp->GetHeight() - bmp->GetHeight();
        // wxLogMessage("Height wasn't correct... saving situation");
    }

    if (myImgPos.GetWidth() + myImgPos.GetX() > bmp->GetWidth()) {
        myImgPos.width = bmp->GetWidth() - bmp->GetWidth();
        // wxLogMessage("Width wasn't correct... saving situation");
    }

    wxASSERT(myImgPos.GetWidth() + myImgPos.GetX() <= bmp->GetWidth());
    wxASSERT(myImgPos.GetHeight() + myImgPos.GetY() <= bmp->GetHeight());

    for (int x = myImgPos.GetX(); x < myImgPos.GetWidth() + myImgPos.GetX(); x++) {
        for (int y = myImgPos.y; y < myImgPos.GetHeight() + myImgPos.y; y++) {
            bmp->SetAlpha(x, y, (char)myUserTransparency);
        }
    }
#endif

    return true;
}

bool vrLayerRasterGDAL::GetData(wxBitmap* bmp, const vrRealRect& coord, double pxsize, const vrRender* render,
                                vrLabel* label) {
    // get extent of raster
    vrRealRect myImgExtent;
    if (!GetExtent(myImgExtent)) {
        return false;
    }

    // compute visible part, position for raster
    wxRect myImgInfo;
    wxRect myImgPos;

    if (!_ComputeDisplayPosSize(m_imgPxSize, myImgExtent, coord, pxsize, myImgInfo, myImgPos)) {
        wxLogMessage("Raster %s invalid. Maybe outside the display", m_fileName.GetFullName());
        return false;
    }

    // raster inside display
    unsigned char* myimgdata = NULL;
    if (!_GetRasterData(&myimgdata, wxSize(myImgPos.GetWidth(), myImgPos.GetHeight()), myImgInfo, render)) {
        wxASSERT(myimgdata == NULL);
        return false;
    }

    wxImage myImg(myImgPos.GetWidth(), myImgPos.GetHeight());
    myImg.SetData(myimgdata, false);
    if (!myImg.IsOk()) {
        wxLogError("Creating raster failed");
        return false;
    }

    unsigned char* mynodata = NULL;
    if (_GetRasterNoData(&mynodata, wxSize(myImgPos.GetWidth(), myImgPos.GetHeight()), myImgInfo, render)) {
        wxASSERT(mynodata != NULL);
        myImg.SetAlpha(mynodata, false);
    }

    // user transparency
    int myUserTransparency = 255 - (render->GetTransparency() * 255 / 100);
    if (myUserTransparency != 255) {
        bool bTransparancyAdded = false;
        if (!myImg.HasAlpha()) {
            myImg.SetAlpha();
            bTransparancyAdded = true;
        }
        wxImagePixelData data(myImg);
        wxImagePixelData::Iterator row(data);
        for (int y = 0; y < myImgPos.GetHeight(); y++) {
            wxImagePixelData::Iterator col = row;
            for (int x = 0; x < myImgPos.GetWidth(); x++, ++row) {
                if (bTransparancyAdded || row.Alpha() != 0) {
                    row.Alpha() = myUserTransparency;
                }
            }
            row = col;
            row.OffsetY(data, 1);
        }
    }

    // drawing the image on the passed bmp
    wxMemoryDC dc(*bmp);
    dc.DrawBitmap(myImg, myImgPos.GetX(), myImgPos.GetY(), true);
    return true;
}

bool vrLayerRasterGDAL::GetPixelValue(double coordx, double coordy, wxArrayDouble& values) {
    values.Clear();

    // is coordinate inside raster
    vrRealRect myRasterExtent;
    if (!GetExtent(myRasterExtent)) {
        return false;
    }
    wxPoint2DDouble myPoint(coordx, coordy);
    if (!myRasterExtent.Contains(myPoint)) {
        return false;
    }

    // convert real coord to pixel coord
    double myMin = wxMin(m_imgExtent.m_y, m_imgExtent.m_y + m_imgExtent.m_height);
    int pxcoordx = wxRound((coordx - m_imgExtent.m_x) / fabs(m_imgExtent.m_width / m_imgPxSize.x));
    int pxcoordy = wxRound((coordy - myMin) / fabs(m_imgExtent.m_height / m_imgPxSize.y));
    // invert y axis
    pxcoordy = m_imgPxSize.y - pxcoordy;

    // get data for all bands
    wxASSERT(m_dataset);
    int myRasterCount = m_dataset->GetRasterCount();
    void* pData = CPLMalloc((GDALGetDataTypeSize(GDT_Float32) / 8) * myRasterCount);

    if (m_dataset->RasterIO(GF_Read, pxcoordx, pxcoordy, 1, 1, pData, 1, 1, GDT_Float32, myRasterCount, NULL, 0, 0,
                            0) != CE_None) {
        wxLogMessage("Error reading value at pixel (%d, %d) in %s", pxcoordx, pxcoordy, m_fileName.GetFullName());
        if (pData != NULL) {
            CPLFree(pData);
            pData = NULL;
        }
        return false;
    }
    double myVal;
    for (int i = 0; i < myRasterCount; i++) {
        myVal = _ReadGDALValueToDouble(pData, GDT_Float32, i);
        if (wxIsSameDouble(myVal, m_oneBandNoData)) {
            // wxLogMessage("Reading no data !");
            CPLFree(pData);
            values.Clear();
            return false;
        }
        values.Add(myVal);
    }
    CPLFree(pData);
    return true;
}

bool vrLayerRasterGDAL::BuildOverviews(const wxArrayInt& factors, vrOVERVIEW_TYPE type, GDALProgressFunc progress,
                                       void* pfProgressData) {
    wxASSERT(m_dataset);
    int* myInts = new int[factors.GetCount()];
    for (unsigned int i = 0; i < factors.GetCount(); i++) {
        myInts[i] = factors.Item(i);
    }

    wxString myConstRRD = "USE_RRD";
    wxString myConstRRDValue = "YES";

    CPLSetConfigOption(myConstRRD.mb_str(), myConstRRDValue.mb_str());
    if (m_dataset->BuildOverviews(vrOVERVIEW_TYPE_NAME[type].mb_str(), factors.GetCount(), myInts, 0, NULL, progress,
                                  pfProgressData) == CE_None) {
        wxDELETEA(myInts);
        return true;
    }

    wxDELETEA(myInts);
    wxLogError("%s", CPLGetLastErrorMsg());
    return false;
}

bool vrLayerRasterGDAL::HasOverviews() {
    wxASSERT(m_dataset);

    GDALRasterBand* myBand = m_dataset->GetRasterBand(1);
    wxASSERT(myBand);

    return myBand->GetOverviewCount() > 0;
}

bool vrLayerRasterGDAL::GetMetadata(wxArrayString& names, wxArrayString& values, const wxString& domain) {
    wxASSERT(m_dataset);
    names.Clear();
    values.Clear();

    // get metadata for dataset and then for each band
    for (unsigned int i = 0; i < m_dataset->GetRasterCount() + 1; i++) {
        char** pmyMetadata;
        if (i == 0) {
            pmyMetadata = m_dataset->GetMetadata(domain);
        } else {
            pmyMetadata = m_dataset->GetRasterBand(i)->GetMetadata(domain);
        }

        if (CSLCount(pmyMetadata) == 0) {
            continue;
        }

        if (pmyMetadata) {
            bool bHasWrongStruture = false;
            while (*pmyMetadata != NULL) {
                wxString myMetadataTxt(*pmyMetadata);
                myMetadataTxt.Replace(_T("\n"), _T(" "));

                wxArrayString myTokens;
                myTokens = wxStringTokenize(myMetadataTxt, "=");
                if (myTokens.GetCount() != 2) {
                    myTokens.Clear();
                    myTokens = wxStringTokenize(myMetadataTxt, ":");
                    if (myTokens.GetCount() != 2) {
                        wxLogMessage(myMetadataTxt);
                        bHasWrongStruture = true;
                        pmyMetadata++;
                        continue;
                    }
                }

                if (i > 0) {
                    names.Add(wxString::Format(_T("B%d-"), i) + myTokens[0]);
                } else {
                    names.Add(myTokens[0]);
                }

                values.Add(myTokens[1]);
                pmyMetadata++;
            }

            if (bHasWrongStruture) {
                wxLogWarning(_("Some metadata weren't fetched! wrong structure."));
            }
        }
    }

    return names.GetCount() != 0;
}

vrLayerRasterEGRID::vrLayerRasterEGRID() {}

vrLayerRasterEGRID::~vrLayerRasterEGRID() {}

wxFileName vrLayerRasterEGRID::GetDisplayName() {
    wxFileName myName(GetFileName().GetPath());
    return myName;
}
