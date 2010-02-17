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
		wxRect2DDouble myExtent;
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
		
		wxRect2DDouble myExtent;
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
	
	
};







#endif
