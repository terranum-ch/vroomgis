/***************************************************************************
								test_vrlayervectorGDAL.h
								Test the GDAL Layers
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

#ifndef _TEST_VR_LAYERVECTORGDAL_H_
#define _TEST_VR_LAYERVECTORGDAL_H_


#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "test_param.h"	//for test parameters
#include "vrlayerraster.h"
#include "vrlayermanager.h"



class TEST_vrLayerRasterGDAL : public CxxTest::TestSuite
{
public:
   	void testOpenLayerRasterGDAL()
	{
		//init lib.
		vrLayerManager myManager;		
		
		vrLayerRasterGDAL myLayer;
		TS_ASSERT(myLayer.IsOK()==false);
		TS_ASSERT_EQUALS(myLayer.Open(wxFileName(g_TestPath, g_TestFileTIF), true),true);
		TS_ASSERT_EQUALS(myLayer.GetType(), vrDRIVER_RASTER_TIFF);
		TS_ASSERT(myLayer.IsOK());
		TS_ASSERT_EQUALS(myLayer.Open(wxFileName(g_TestPath, g_TestFileTIF), true),true);
	}
	
	
	void testGetExtentRasterGDAL(){
		wxLogMessage("Testing getting extent for GDAL extent");
		vrRealRect myExtent;
		TS_ASSERT(myExtent.IsEmpty()==true);
		
		// extent failed, layer not opened
		vrLayerRasterGDAL myLayer;
		TS_ASSERT(myLayer.GetExtent(myExtent)==false);
		
		// extent ok for layer GDAL
		TS_ASSERT_EQUALS(myLayer.Open(wxFileName(g_TestPath, g_TestFileTIF), false),true);
		TS_ASSERT_EQUALS(myLayer.IsOK(),true);
		TS_ASSERT(myLayer.GetExtent(myExtent)==true);
		TS_ASSERT_EQUALS((int) myExtent.GetLeft(), 598000);
		TS_ASSERT_EQUALS((int) myExtent.GetTop(), 116000);
		TS_ASSERT_EQUALS((int) myExtent.GetRight(), 599000);
		TS_ASSERT_EQUALS((int) myExtent.GetBottom(), 114000);
		
		
		
	}
	
	
	void testGettingExtentGDAL2(){
		
		// GETTING EXTENT FOR ROTATED RASTERS RETURN MAX EXTENT
		
		vrRealRect myExtent;
		TS_ASSERT(myExtent.IsEmpty()==true);
		
		// extent failed, layer not opened
		vrLayerRasterGDAL myLayer;
		TS_ASSERT(myLayer.GetExtent(myExtent)==false);
		
		// extent ok for layer GDAL
		TS_ASSERT_EQUALS(myLayer.Open(wxFileName(g_TestPath, g_TestFileJPEG), false),true);
		TS_ASSERT_EQUALS(myLayer.IsOK(),true);
		TS_ASSERT(myLayer.GetExtent(myExtent)==true);
		TS_ASSERT_DELTA(myExtent.GetLeft(), 575147.743, 0.001);
		TS_ASSERT_DELTA(myExtent.GetTop(), 169147.080, 0.001);
		TS_ASSERT_DELTA(myExtent.GetRight(), 613503.148, 0.001);
		TS_ASSERT_DELTA(myExtent.GetBottom(), 141898.483, 0.001);
		
		wxLogMessage("Windows extent is :\nleft : \t%.3f\nright : \t%.3f\ntop : \t%.3f\nbottom : \t%.3f",
					 myExtent.GetLeft(),
					 myExtent.GetRight(),
					 myExtent.GetTop(),
					 myExtent.GetBottom());
		
	}
	
	bool _ComputeDisplayPosSize(const wxSize & pximgsize, 
								const vrRealRect & imgextent, 
								const vrRealRect & wndextent,
								double pxsize,
								wxRect & pximginfo, wxPoint & pximgpos){
		
		// get intersection between display and img
		vrRealRect myWndExtentTemp = wndextent;
		wxASSERT(myWndExtentTemp == wndextent);
	
		vrRealRect myIntersect = myWndExtentTemp.Intersect(imgextent);
		if (myIntersect.IsOk() == false) {
			wxLogMessage("Image out of the dislay, intersection is null");
			return false;
		}
		
		
		// width of image to display (in pixels)
		int pxWidthVisible = myIntersect.m_width * pximgsize.GetWidth() / imgextent.m_width;
		int pxHeightVisible = myIntersect.m_height * pximgsize.GetHeight() / imgextent.m_height;
		
		// starting position from where we get image data (px)
		int ximg = (myIntersect.GetLeft() - imgextent.GetLeft())  * pximgsize.GetWidth() / imgextent.m_width;
		int yimg = (myIntersect.GetTop() - imgextent.GetTop()) * pximgsize.GetHeight() / imgextent.m_height;
		
		// position for displaying the bitmap (in pixels)
		int vx = (myIntersect.GetLeft() - wndextent.GetLeft()) / pxsize;
		int vy = (wndextent.GetTop()- myIntersect.GetTop()) / pxsize;
		
		// returning values
		pximginfo.SetTopLeft(wxPoint(ximg, yimg));
		pximginfo.width = pxWidthVisible;
		pximginfo.height = pxHeightVisible;
		
		pximgpos.x = vx;
		pximgpos.y = vy;
		
		if (pximginfo.IsEmpty()) {
			wxLogMessage("Image is outside the display.");
			return false;
			
		}
		
		return true;
	}
	
	void testForRasterIO1(){
		// this code is for testing how to get data ready 
		// for RasterIO function
		
		//Param
		vrRealRect myWndExtent (0, 1000, 4000, -1000);
		vrRealRect myImgExtent (1000, 800, 2000, -600);
		
		wxSize myImgPxSize (200, 60);
		double pixelsize = 10;
		
		
		// tests
		wxRect myImgInfo;
		wxPoint myImgPos;
		TS_ASSERT(_ComputeDisplayPosSize(myImgPxSize, myImgExtent, myWndExtent, pixelsize,
										 myImgInfo, myImgPos)==true);
		TS_ASSERT_EQUALS(myImgInfo.GetX(), 0);
		TS_ASSERT_EQUALS(myImgInfo.GetY(), 0);
		TS_ASSERT_EQUALS(myImgInfo.GetWidth(), 200);
		TS_ASSERT_EQUALS(myImgInfo.GetHeight(), 60);
		
		TS_ASSERT_EQUALS(myImgPos.x, 100);
		TS_ASSERT_EQUALS(myImgPos.y, 20);
	}
	
	void testForRasterIO2(){
		// this code is for testing how to get data ready 
		// for RasterIO function
		
		//Param
		vrRealRect myWndExtent (0, 1000, 4000, -1000);
		vrRealRect myImgExtent (2000, 1500, 4000, -1000);
		
		wxSize myImgPxSize (200, 100);
		double pixelsize = 10;
		
		
		// tests
		wxRect myImgInfo;
		wxPoint myImgPos;
		TS_ASSERT(_ComputeDisplayPosSize(myImgPxSize, myImgExtent, myWndExtent, pixelsize,
										 myImgInfo, myImgPos)==true);
		TS_ASSERT_EQUALS(myImgInfo.GetX(), 0);
		TS_ASSERT_EQUALS(myImgInfo.GetY(), 50);
		TS_ASSERT_EQUALS(myImgInfo.GetWidth(), 100);
		TS_ASSERT_EQUALS(myImgInfo.GetHeight(), 50);
		
		TS_ASSERT_EQUALS(myImgPos.x, 200);
		TS_ASSERT_EQUALS(myImgPos.y, 0);
	}
	
	void testForRasterIO3(){
		// this code is for testing how to get data ready 
		// for RasterIO function
		
		//Param
		vrRealRect myWndExtent (1000, 1500, 4000, -1000);
		vrRealRect myImgExtent (0, 1000, 2000, -1000);
		
		wxSize myImgPxSize (200, 100);
		double pixelsize = 10;
		
		
		// tests
		wxRect myImgInfo;
		wxPoint myImgPos;
		TS_ASSERT(_ComputeDisplayPosSize(myImgPxSize, myImgExtent, myWndExtent, pixelsize,
										 myImgInfo, myImgPos)==true);
		TS_ASSERT_EQUALS(myImgInfo.GetX(), 100);
		TS_ASSERT_EQUALS(myImgInfo.GetY(), 0);
		TS_ASSERT_EQUALS(myImgInfo.GetWidth(), 100);
		TS_ASSERT_EQUALS(myImgInfo.GetHeight(), 50);
		
		TS_ASSERT_EQUALS(myImgPos.x, 0);
		TS_ASSERT_EQUALS(myImgPos.y, 50);
	}
	
	
};







#endif
