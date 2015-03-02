/***************************************************************************
				vrlayerrasterc2d.cpp
                    
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

#include "vrlayerrasterc2d.h"
#include "vrrealrect.h"
#include "vrrendercoltop.h"
#include "vrlabel.h"


vrLayerRasterC2D::vrLayerRasterC2D() {
}

vrLayerRasterC2D::~vrLayerRasterC2D() {
}

bool vrLayerRasterC2D::_GetRasterData(unsigned char ** imgdata, const wxSize & outimgpxsize,
									  const wxRect & readimgpxinfo, const vrRender * render) {
	
	wxASSERT(m_dataset);
	m_dataset->FlushCache();
    int myRasterCount  = m_dataset->GetRasterCount();
	
	if (myRasterCount != 3) {
		wxLogError("Corrupted C2D file, contain %d band instead of 3", myRasterCount);
		return false;
	}
	
	// create array for image data (RGBRGBRGB...)
	unsigned int myimgRGBLen = outimgpxsize.GetWidth() * outimgpxsize.GetHeight() * 3;
	*imgdata = (unsigned char *) malloc(myimgRGBLen);
	if (*imgdata == NULL) {
		wxLogError("Image creation failed, out of memory");
		return false;
	}
	
	
	// read band 2 (slope)
	GDALRasterBand *band = m_dataset->GetRasterBand(2);
	int myDataSize = GDALGetDataTypeSize(GDT_Float32) / 8;
	void * mySlopeData = CPLMalloc(myDataSize * outimgpxsize.GetWidth() * outimgpxsize.GetHeight());
	if (band->RasterIO(GF_Read,
					   readimgpxinfo.GetX(), readimgpxinfo.GetY(),
					   readimgpxinfo.GetWidth(), readimgpxinfo.GetHeight(), 
					   mySlopeData,
					   outimgpxsize.GetWidth(), outimgpxsize.GetHeight(),
					   GDT_Float32, 0, 0) != CE_None) {
		wxLogError("Error gettign C2D slope, maybe out of memory");
		if (mySlopeData != NULL) {
			CPLFree (mySlopeData);
			mySlopeData = NULL;
		}
		return false;
	}

	
	// read band 3 (aspect)
	band = m_dataset->GetRasterBand(3);
	void * myAspectData = CPLMalloc(myDataSize * outimgpxsize.GetWidth() * outimgpxsize.GetHeight());
	if (band->RasterIO(GF_Read,
					   readimgpxinfo.GetX(), readimgpxinfo.GetY(),
					   readimgpxinfo.GetWidth(), readimgpxinfo.GetHeight(), 
					   myAspectData,
					   outimgpxsize.GetWidth(), outimgpxsize.GetHeight(),
					   GDT_Float32, 0, 0) != CE_None) {
		wxLogError("Error gettign C2D aspect, maybe out of memory");
		if (myAspectData != NULL) {
			CPLFree (myAspectData);
			myAspectData = NULL;
		}
		return false;
	}
	
	vrRenderRasterColtop * myColtopRender = (vrRenderRasterColtop*) render;
	wxASSERT(myColtopRender);
	wxASSERT(myColtopRender->GetType() == vrRENDER_RASTER_C2D);
	
	// convert to RGB
	for (unsigned int i = 0; i< myimgRGBLen; i += 3) {
		double mySlpDouble = _ReadGDALValueToDouble(mySlopeData, GDT_Float32, i/3);
		double myAspDouble = _ReadGDALValueToDouble(myAspectData, GDT_Float32, i/3);
		
		wxImage::RGBValue myRGBValue = myColtopRender->GetColorFromDipDir(mySlpDouble, myAspDouble);
		
		// fill buffer
		*(*imgdata + i)	= myRGBValue.red;
		*(*imgdata + i + 1) = myRGBValue.green;
		*(*imgdata + i + 2) = myRGBValue.blue;
	}
	
	CPLFree(myAspectData);
	myAspectData = NULL;
	CPLFree(mySlopeData);
	mySlopeData = NULL;
	return true;
}


bool vrLayerRasterC2D::GetRasterData(unsigned char ** imgdata, const wxSize & outimgpxsize,
									 const wxRect & readimgpxinfo, const vrRender * render) {
	return _GetRasterData(imgdata, outimgpxsize, readimgpxinfo, render);
}


// NOT used anymore, using wxImage::HSVtoRGB instead
void vrLayerRasterC2D::_HSVtoRGB (int *r,int *g,int *b, int h, int s, int v){
	long p, q, t;
	int f;
	
	if( s == 0 ){
		*r = *g = *b = v;
		return;
	}
	
	f = ((h%60)*255)/60;
	h /= 60;
	
	p = (v * (256 - s)) / 256;
	q = (v * (256 - (s * f) / 256))/256;
	t = (v * (256 - (s * (256 - f)) / 256)) / 256;
	
	switch( h ) {
		case 0:
			*r = v;
			*g = t;
			*b = p;
			break;
		case 1:
			*r = q;
			*g = v;
			*b = p;
			break;
		case 2:
			*r = p;
			*g = v;
			*b = t;
			break;
		case 3:
			*r = p;
			*g = q;
			*b = v;
			break;
		case 4:
			*r = t;
			*g = p;
			*b = v;
			break;
		default:
			*r = v;
			*g = p;
			*b = q;
			break;
	}
}
	
