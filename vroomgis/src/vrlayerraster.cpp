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
#include "vrrender.h"	// for symbology;
#include <wx/rawbmp.h>



vrLayerRaster::vrLayerRaster() {
	m_Dataset = NULL;
}

vrLayerRaster::~vrLayerRaster() {
}


bool vrLayerRaster::IsOK() {
	if (m_Dataset==NULL){
		wxLogError("Dataset not inited");
		return false;
	}
	return true;
}







bool vrLayerRasterGDAL::_Close() {
	m_OneBandMin = 0;
	m_OneBandMax = 0;
	m_OneBandNoData = 0;


	if (m_Dataset==NULL){
		return false;
	}

	wxLogMessage("Closing raster data NOW");
	GDALClose(m_Dataset);
	m_Dataset = NULL;
	return true;
}





/***************************************************************************//**
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
bool vrLayerRasterGDAL::_ComputeDisplayPosSize(const wxSize & pximgsize,
							const vrRealRect & imgextent,
							const vrRealRect & wndextent,
							double pxsize,
							wxRect & pximginfo, wxRect & pximgpos){

	// get intersection between display and img
	vrRealRect myWndExtentTemp = wndextent;
	wxASSERT(myWndExtentTemp == wndextent);


	pximginfo = wxRect(0,0,0,0);
	pximgpos = wxRect(0,0,0,0);

	vrRealRect myIntersect = myWndExtentTemp.Intersect(imgextent);
	if (myIntersect.IsOk() == false) {
		wxLogMessage("Image out of the dislay, intersection is null");
		return false;
	}


	// width of image to display (in pixels)
	int pxWidthVisible = wxRound(myIntersect.m_width * pximgsize.GetWidth() / imgextent.m_width);
	int pxHeightVisible = wxRound(myIntersect.m_height * pximgsize.GetHeight() / imgextent.m_height);

	// starting position from where we get image data (px)
	int ximg = wxRound((myIntersect.GetLeft() - imgextent.GetLeft())  * pximgsize.GetWidth() / imgextent.m_width);
	int yimg = wxRound((myIntersect.GetTop() - imgextent.GetTop()) * pximgsize.GetHeight() / imgextent.m_height);

	// position for displaying the bitmap (in pixels)
	int vx = wxRound((myIntersect.GetLeft() - wndextent.GetLeft()) / pxsize);
	int vy = wxRound((wndextent.GetTop()- myIntersect.GetTop()) / pxsize);

	//pixels size of displayed bitmap
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



bool vrLayerRasterGDAL::_GetRasterData(unsigned char ** imgdata,
									   const wxSize & outimgpxsize,
									   const wxRect & readimgpxinfo,
									   const vrRender * render) {

	wxASSERT(m_Dataset);
	m_Dataset->FlushCache();
    int myRasterCount  = m_Dataset->GetRasterCount();

	// check image size bigger than 0
	if (outimgpxsize.GetWidth() <= 0 || outimgpxsize.GetHeight() <= 0) {
		wxLogError("Dimentions of the image are invalid (%d, %d)",
				   outimgpxsize.GetWidth(),
				   outimgpxsize.GetHeight());
		return false;
	}

	// create array for image data (RGBRGBRGB...)
	unsigned int myimgLen = outimgpxsize.GetWidth() * outimgpxsize.GetHeight() * 3;
	*imgdata = (unsigned char *) malloc(myimgLen);
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
		void * myScanData = NULL;
		GDALColorTable *pal = NULL;
		GDALPaletteInterp pal_interp;
		GDALRasterBand *band = m_Dataset->GetRasterBand(1);

		switch (band->GetColorInterpretation()) {

			case GCI_PaletteIndex:

                pal = band->GetColorTable();
                if (pal == NULL){
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
				if (band->RasterIO(GF_Read, readimgpxinfo.GetX(), readimgpxinfo.GetY(),
								   readimgpxinfo.GetWidth(), readimgpxinfo.GetHeight(),
								   *imgdata,
								   outimgpxsize.GetWidth(), outimgpxsize.GetHeight(),
								   GDT_UInt16, 3, 0) != CE_None){
					wxLogError("Unknown error occured while reading palette based raster %s",
							   m_FileName.GetFullName());
					if (*imgdata != NULL) {
						CPLFree(*imgdata);
						*imgdata = NULL;
					}
					return false;
				}


				for (unsigned char *data = *imgdata; data != (*imgdata + myimgLen);data += 3){
					unsigned short int val = *((unsigned short int *)data);
					const GDALColorEntry *color = pal->GetColorEntry(val);
					if (pal_interp == GPI_Gray){
						*(data + 0) = color->c1;
						*(data + 1) = color->c1;
						*(data + 2) = color->c1;
					}
					else{
						*(data + 0) = color->c1;
						*(data + 1) = color->c2;
						*(data + 2) = color->c3;
					}
				}

                break;


			case GCI_Undefined: // can we try to make a greyscale image?
            case GCI_GrayIndex:

				// computing statistics if not existing
				if (_HasStat() == false) {
					if (_ComputeStat() == false) {
						if (*imgdata != NULL) {
							CPLFree(*imgdata);
							*imgdata = NULL;
						}
						return false;
					}
				}

				myRange = m_OneBandMax - m_OneBandMin;
				if (myRange <= 0) {
					myRange = 1;
				}
				//wxASSERT(myRange > 0);
				myDataType = band->GetRasterDataType();

				// getting monoband image
				myScanSize = GDALGetDataTypeSize(myDataType) / 8;
				myScanData = CPLMalloc(myScanSize * outimgpxsize.GetWidth() * outimgpxsize.GetHeight());
				wxASSERT(myScanData);
				if (band->RasterIO(GF_Read,
								   readimgpxinfo.GetX(), readimgpxinfo.GetY(),
								   readimgpxinfo.GetWidth(), readimgpxinfo.GetHeight(),
								   myScanData,
								   outimgpxsize.GetWidth(), outimgpxsize.GetHeight(),
								   myDataType, 0, 0) != CE_None) {

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
				for (unsigned int i = 0; i< myimgLen ; i += 3)
                {

					double myGrayValDouble = _ReadGDALValueToDouble(myScanData,myDataType, i / 3);
					// set nodata values to white!
                    // TODO: take nodata into account
                    int myGrayValInt = 255;
                    if (wxIsSameDouble(myGrayValDouble, m_OneBandNoData)==false) {
                        myGrayValInt = static_cast < int >((myGrayValDouble - m_OneBandMin) * (255 / myRange));
                    }

					if (myGrayValInt < 0) myGrayValInt = 0;
					if (myGrayValInt > 255) myGrayValInt = 255;

					*(*imgdata + i)		= myGrayValInt;
					*(*imgdata + i + 1) = myGrayValInt;
					*(*imgdata + i + 2) = myGrayValInt;

				}
				wxASSERT(myScanData != NULL);
				CPLFree (myScanData);
				myScanData = NULL;
				break;



			default: // unsupported case


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
    // if there are three or more banm_DataSet assume that the first three
    // are for RGB, unless told otherwise
    //
    if (myRasterCount >= 3)
    {
        for (int i=1; i <= 3; i++)
        {
            int offs = 0;
            GDALRasterBand *band = m_Dataset->GetRasterBand(i);

            switch (band->GetColorInterpretation())
            {
                case GCI_Undefined: offs = i-1; break;
				case GCI_RedBand:   offs = 0; break;
				case GCI_GreenBand: offs = 1; break;
				case GCI_BlueBand:  offs = 2; break;
                default:            offs = -1; break;
            }

            //
            // copy the image into the buffer using the proper offset
            // so we first copy over all Red values, then all Green
            // values, and then all Blue values
            //

            if (0 <= offs && offs < 3)
            {
                if ( band->RasterIO(GF_Read,
									 readimgpxinfo.GetX(), readimgpxinfo.GetY(),
                                     readimgpxinfo.GetWidth(), readimgpxinfo.GetHeight(),
                                     *imgdata + offs,
									 outimgpxsize.GetWidth(), outimgpxsize.GetHeight(),
                                     GDT_Byte, 3, 0) != CE_None){
					wxLogError(_T( "Unknown error while reading band %i from %s"),
							   i, m_FileName.GetFullName());
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


bool vrLayerRasterGDAL::_GetRasterNoData(unsigned char ** imgdata, const wxSize & outimgpxsize,
										 const wxRect & readimgpxinfo, const vrRender * render) {
	wxASSERT(m_Dataset);
	m_Dataset->FlushCache();
    //int myRasterCount  = m_Dataset->GetRasterCount();

	// check image size bigger than 0
	if (outimgpxsize.GetWidth() <= 0 || outimgpxsize.GetHeight() <= 0) {
		wxLogError("Dimentions of the image are invalid (%d, %d)",
				   outimgpxsize.GetWidth(),
				   outimgpxsize.GetHeight());
		return false;
	}


	// computing statistics if not existing
	if (_HasStat() == false) {
		if (_ComputeStat() == false) {
			return false;
		}
	}


	/*double myRange = 0;


	void * myScanData = NULL;
	GDALColorTable *pal = NULL;
	GDALPaletteInterp pal_interp;*/

	GDALRasterBand *band = m_Dataset->GetRasterBand(1)->GetMaskBand();
	wxASSERT(band);
	int myMaskFlag = m_Dataset->GetRasterBand(1)->GetMaskFlags();
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
	*imgdata = (unsigned char *) malloc(myScanSize * outimgpxsize.GetWidth() * outimgpxsize.GetHeight());
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

	if (band->RasterIO(GF_Read,
					   readimgpxinfo.GetX(), readimgpxinfo.GetY(),
					   readimgpxinfo.GetWidth(), readimgpxinfo.GetHeight(),
					   *imgdata,
					   outimgpxsize.GetWidth(), outimgpxsize.GetHeight(),
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




double vrLayerRasterGDAL::_ReadGDALValueToDouble(void* & data, const GDALDataType & type, int index) {
	double val;

	switch ( type )
	{
		case GDT_Byte:
			return (double) ((GByte *)data)[index];
			break;
		case GDT_UInt16:
			return (double) ((GUInt16 *)data)[index];
			break;
		case GDT_Int16:
			return (double) ((GInt16 *)data)[index];
			break;
		case GDT_UInt32:
			return (double) ((GUInt32 *)data)[index];
			break;
		case GDT_Int32:
			return (double) ((GInt32 *)data)[index];
			break;
		case GDT_Float32:
			return (double) ((float *)data)[index];
			break;
		case GDT_Float64:
			val = ((double *)data)[index];
			return (double) ((double *)data)[index];
			break;
		default:
			wxLogError("GDAL data type is not supported");
	}
	return 0.0;
}



bool vrLayerRasterGDAL::_ComputeStat() {
	int bResultMin = false;
    int bResultMax = false;
    int bResultNoData = false;
	//bool bReturn = true;

	m_OneBandMin = 0;
	m_OneBandMax = 0;
	m_OneBandNoData = 0;


	GDALRasterBand * myRasterBand = m_Dataset->GetRasterBand(1);
	m_OneBandMin = myRasterBand->GetMinimum(&bResultMin);
	m_OneBandMax = myRasterBand->GetMaximum(&bResultMax);
	m_OneBandNoData = myRasterBand->GetNoDataValue(&bResultNoData);

    if (bResultNoData == false){
		m_OneBandNoData = 0;
	}

    if (bResultMin == true && bResultMax == true) {
        return true;
    }

    // try computing statistics (take more time...)
    if (myRasterBand->ComputeStatistics(true, &m_OneBandMin, &m_OneBandMax, NULL, NULL, NULL, NULL) != CE_None) {
        wxLogWarning("Computing statistics for raster %s failed!", m_FileName.GetFullName());
        return false;
    }
	return true;
}



bool vrLayerRasterGDAL::_HasStat() {

	if (m_OneBandMin == 0 ||
		m_OneBandMax == 0) {
		return false;
	}

	return true;
}



bool vrLayerRasterGDAL::_HasExtent() {

	if (m_Dataset == NULL) {
		wxLogError("No layer opened");
		return false;
	}


	if (m_ImgExtent.IsOk() == false) {
		return false;
	}


	if (m_ImgPxSize == wxDefaultSize) {
		wxLogError("Image size (pixels) isn't defined correctly");
		return false;
	}

	return true;
}



bool vrLayerRasterGDAL::_ComputeExtent() {
	if (m_Dataset == NULL) {
		wxLogError("No layer opened");
		return false;
	}

	wxASSERT(m_Dataset);
	double        adfGeoTransform[6];
	if( m_Dataset->GetGeoTransform( adfGeoTransform ) != CE_None ){
		wxLogError("Error getting geometric informations for layer : %s",
				   m_FileName.GetFullName());
		return false;
	}

	if (adfGeoTransform[5] > 0) {
		wxLogError("vertical pixel size is Wrong (should be negative) : %.2f",
				   adfGeoTransform[5]);
	}


	// top left corner
	wxPoint2DDouble mytopleft;
	mytopleft.m_x = adfGeoTransform[0] + adfGeoTransform[1] * 0.0 + adfGeoTransform[2] * 0.0;
	mytopleft.m_y = adfGeoTransform[3] + adfGeoTransform[4] * 0.0 + adfGeoTransform[5] * 0.0;

	// bottom left corner
	wxPoint2DDouble mybottomleft;
	mybottomleft.m_x = adfGeoTransform[0] + adfGeoTransform[1] * 0.0 + adfGeoTransform[2] * m_Dataset->GetRasterYSize();
	mybottomleft.m_y = adfGeoTransform[3] + adfGeoTransform[4] * 0.0 + adfGeoTransform[5] * m_Dataset->GetRasterYSize();


	// top right corner
	wxPoint2DDouble mytopright;
	mytopright.m_x = adfGeoTransform[0] + adfGeoTransform[1] * m_Dataset->GetRasterXSize() + adfGeoTransform[2] * 0.0;
	mytopright.m_y = adfGeoTransform[3] + adfGeoTransform[4] * m_Dataset->GetRasterXSize() + adfGeoTransform[5] * 0.0;

	// bottom right corner
	wxPoint2DDouble mybottomright;
	mybottomright.m_x = adfGeoTransform[0] + adfGeoTransform[1] * m_Dataset->GetRasterXSize() + adfGeoTransform[2] * m_Dataset->GetRasterYSize();
	mybottomright.m_y = adfGeoTransform[3] + adfGeoTransform[4] * m_Dataset->GetRasterXSize() + adfGeoTransform[5] * m_Dataset->GetRasterYSize();

	// compute extent
	// left
	double myleft = mytopleft.m_x;
	if (mybottomleft.m_x < mytopleft.m_x) {
		myleft = mybottomleft.m_x;
	}
	m_ImgExtent.SetLeft(myleft);

	// right
	double myright = mytopright.m_x;
	if (mybottomright.m_x > mytopright.m_x) {
		myright = mybottomright.m_x;
	}
	m_ImgExtent.SetRight(myright);

	// top
	double mytop = mytopleft.m_y;
	if (mytopright.m_y > mytopleft.m_y) {
		mytop = mytopright.m_y;
	}
	m_ImgExtent.SetTop(mytop);

	// bottom
	double mybottom = mybottomleft.m_y;
	if (mybottomright.m_y < mybottomleft.m_y) {
		mybottom = mybottomright.m_y;
	}
	m_ImgExtent.SetBottom(mybottom);


	m_ImgPxSize = wxSize(m_Dataset->GetRasterXSize(),
						 m_Dataset->GetRasterYSize());


	if (adfGeoTransform[2] != 0 || adfGeoTransform[4] != 0) {
		wxLogWarning("Layer %s contain following rotation informations (%.4f, %.4f). It may not be displayed correctly",
					 m_FileName.GetFullName(),
					 adfGeoTransform[2],
					 adfGeoTransform[4]);
	}

	return true;
}







vrLayerRasterGDAL::vrLayerRasterGDAL() {
}



vrLayerRasterGDAL::~vrLayerRasterGDAL() {
	_Close();
}



bool vrLayerRasterGDAL::Create(const wxFileName & filename) {
	return false;
}



bool vrLayerRasterGDAL::Open(const wxFileName & filename, bool readwrite) {
	// try to close
	_Close();
	wxASSERT(m_Dataset == NULL);

	// init filename and type
	m_FileName = filename;
	vrDrivers myDriver;
	m_DriverType = myDriver.GetType(filename.GetExt());

	GDALAccess myAcces = GA_ReadOnly;
	if (readwrite)
		myAcces = GA_Update;

	m_Dataset = (GDALDataset *) GDALOpen( filename.GetFullPath(), GA_ReadOnly );
	if(m_Dataset == NULL){
		wxLogError("Unable to open %s, maybe driver not registered -  GDALAllRegister()\nGDAL Error: '%s'",
				   filename.GetFullName(),
				   wxString(CPLGetLastErrorMsg()));
		
		return false;
	}

	return true;
}


bool vrLayerRasterGDAL::GetExtent(vrRealRect & rect) {

	// if extent exist : return it. otherwise, compute it
	if (_HasExtent() == true) {
		rect = m_ImgExtent;
		return true;
	}

	if (_ComputeExtent() == true) {
		rect = m_ImgExtent;
		return true;
	}

	wxLogError("Unable to compute the layer %s extent", m_FileName.GetFullName());
	return false;
}




bool vrLayerRasterGDAL::GetDataThread(wxImage * bmp, const vrRealRect & coord,  double pxsize,
									  const vrRender * render, const vrLabel * label) {

	// get extent of raster
	vrRealRect myImgExtent;
	if (GetExtent(myImgExtent) == false) {
		return false;
	}


	// compute visible part, position for raster
	wxRect myImgInfo;
	wxRect myImgPos;

	if(_ComputeDisplayPosSize(m_ImgPxSize, myImgExtent, coord,
							  pxsize, myImgInfo, myImgPos)==false){
		wxLogMessage("Raster %s invalid. Maybe outside the display", m_FileName.GetFullName());
		return false;
	}

	// raster inside display
	unsigned char * myimgdata = NULL;
	if (_GetRasterData(&myimgdata, wxSize(myImgPos.GetWidth(), myImgPos.GetHeight()),
					   myImgInfo, render) == false) {
		wxASSERT(myimgdata == NULL);
		return false;
	}

	wxImage myImg (myImgPos.GetWidth(), myImgPos.GetHeight());
	myImg.SetData(myimgdata, false);
	if (myImg.IsOk() == false) {
		wxLogError("Creating raster failed");
		return false;
	}

	unsigned char * mynodata = NULL;
	if (_GetRasterNoData(&mynodata, wxSize(myImgPos.GetWidth(), myImgPos.GetHeight()),
						 myImgInfo, render) != false) {
		wxASSERT(mynodata != NULL);
		myImg.SetAlpha(mynodata, false);
	}

	// user transparency
	int myUserTransparency = 255 - (render->GetTransparency() * 255 / 100);
	if (myUserTransparency != 255) {
		wxImagePixelData data(myImg);
		wxImagePixelData::Iterator row (data);
		for (int y = 0; y < myImgPos.GetHeight();y++){
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
	wxMemoryDC dc (myBmp);
	//dc.Clear();

	dc.DrawBitmap(myImg, myImgPos.GetX(), myImgPos.GetY(), true);
	dc.SelectObject(wxNullBitmap);
	*bmp = myBmp.ConvertToImage();

//  transparency problem under linux
#if __LINUX__
	// set image transparency. Image is fully transparent, except for pixels where bitmap
	// was drawn. Those are set the the user defined transparency
	char myBackgroundTransparency = 0;
	//int myUserTransparency = 255 - (render->GetTransparency() * 255 / 100);
	unsigned char * alphachar = NULL;

	unsigned int myimglen = bmp->GetWidth() * bmp->GetHeight();
	alphachar= (unsigned char*)malloc(myimglen);
	if (alphachar == NULL)
	{
		wxLogError(_T("Error creating translucency"));
		return false;
	}


	for ( unsigned int i = 0; i< myimglen; i++) {
		*(alphachar + i) =  myBackgroundTransparency;
	}
	if (bmp->HasAlpha() == false){
		bmp->InitAlpha();
		wxLogMessage("Initing alpha");
	}
	bmp->SetAlpha(alphachar, false);


	if (myImgPos.GetHeight() + myImgPos.GetY() > bmp->GetHeight()) {
		myImgPos.height = bmp->GetHeight() - bmp->GetHeight();
		//wxLogMessage("Height wasn't correct... saving situation");
	}

	if (myImgPos.GetWidth() + myImgPos.GetX() > bmp->GetWidth()) {
		myImgPos.width = bmp->GetWidth() - bmp->GetWidth();
		//wxLogMessage("Width wasn't correct... saving situation");
	}


	wxASSERT(myImgPos.GetWidth() + myImgPos.GetX() <= bmp->GetWidth());
	wxASSERT(myImgPos.GetHeight() + myImgPos.GetY() <= bmp->GetHeight());

	for (int x = myImgPos.GetX(); x< myImgPos.GetWidth() + myImgPos.GetX() ; x++) {
		for (int y = myImgPos.y; y < myImgPos.GetHeight() + myImgPos.y ; y++) {
			bmp->SetAlpha(x, y, (char) myUserTransparency);
		}
	}
#endif

	return true;
}



bool vrLayerRasterGDAL::GetData(wxBitmap * bmp, const vrRealRect & coord, double pxsize, 
							   const vrRender * render, const vrLabel * label) {
	return false;
}



bool vrLayerRasterGDAL::GetPixelValue(double coordx, double coordy, wxArrayDouble & values) {

	values.Clear();

	// is coordinate inside raster
	vrRealRect myRasterExtent;
	if (GetExtent(myRasterExtent)==false) {
		return false;
	}
	wxPoint2DDouble myPoint (coordx, coordy);
	if (myRasterExtent.Contains(myPoint) == false) {
		return false;
	}


	// convert real coord to pixel coord
	double myMin = wxMin (m_ImgExtent.m_y, m_ImgExtent.m_y + m_ImgExtent.m_height);
	int pxcoordx = wxRound((coordx - m_ImgExtent.m_x) / fabs(m_ImgExtent.m_width / m_ImgPxSize.x));
	int pxcoordy = wxRound((coordy - myMin) / fabs(m_ImgExtent.m_height / m_ImgPxSize.y));
	// invert y axis
	pxcoordy = m_ImgPxSize.y - pxcoordy;

	// get data for all bands
	wxASSERT(m_Dataset);
	int myRasterCount = m_Dataset->GetRasterCount();
	void * pData = CPLMalloc((GDALGetDataTypeSize(GDT_Float32) / 8) * myRasterCount);

	if (m_Dataset->RasterIO(GF_Read,
							pxcoordx, pxcoordy,
							1, 1,
							pData,
							1, 1,
							GDT_Float32,
							myRasterCount, NULL, 0,0,0) != CE_None) {
		wxLogMessage("Error reading value at pixel (%d, %d) in %s",
				   pxcoordx, pxcoordy, m_FileName.GetFullName());
		if (pData != NULL) {
			CPLFree(pData);
			pData = NULL;
		}
		return false;
	}
	double myVal = 0.0;
	for (int i = 0; i<myRasterCount; i++) {
		myVal = _ReadGDALValueToDouble(pData, GDT_Float32, i);
        if (wxIsSameDouble(myVal, m_OneBandNoData)) {
            //wxLogMessage("Reading no data !");
            CPLFree(pData);
            values.Clear();
            return false;
        }
		values.Add(myVal);
	}
	CPLFree(pData);
	return true;
}




bool vrLayerRasterGDAL::BuildOverviews(const wxArrayInt & factors,
									   vrOVERVIEW_TYPE type,
									   GDALProgressFunc progress,
									   void * pfProgressData) {
	wxASSERT(m_Dataset);
	int * myInts = new int[factors.GetCount()];
	for (unsigned int i = 0; i<factors.GetCount(); i++) {
		myInts[i] = factors.Item(i);
	}

	wxString myConstRRD = "USE_RRD";
	wxString myConstRRDValue = "YES";

	CPLSetConfigOption(myConstRRD.mb_str(), myConstRRDValue.mb_str());
	if( m_Dataset->BuildOverviews(vrOVERVIEW_TYPE_NAME[type].mb_str(),
								  factors.GetCount(),
								  myInts, 0, NULL,
								  progress, pfProgressData) == CE_None){
		wxDELETEA(myInts);
		return true;
	}

	wxDELETEA(myInts);
	wxLogError("%s", CPLGetLastErrorMsg());
	return false;
}


bool vrLayerRasterGDAL::HasOverviews() {
	wxASSERT(m_Dataset);

	GDALRasterBand * myBand = m_Dataset->GetRasterBand(1);
	wxASSERT(myBand);
	if (myBand->GetOverviewCount() > 0) {
		return true;
	}
	return false;
}



vrLayerRasterEGRID::vrLayerRasterEGRID() {
}

vrLayerRasterEGRID::~vrLayerRasterEGRID() {

}

wxFileName vrLayerRasterEGRID::GetDisplayName() {
	wxFileName myName (GetFileName().GetPath());
	return myName;
}



