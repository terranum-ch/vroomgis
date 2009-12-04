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
		
		vrLayerRasterGDAL myLayer;
		TS_ASSERT(myLayer.IsOK()==false);
		TS_ASSERT_EQUALS(myLayer.Open(wxFileName(g_TestPath, g_TestFileTIF), true),true);
		TS_ASSERT_EQUALS(myLayer.GetType(), vrDRIVER_RASTER_TIFF);
		TS_ASSERT(myLayer.IsOK());
		TS_ASSERT_EQUALS(myLayer.Open(wxFileName(g_TestPath, g_TestFileTIF), true),true);
	}
	
		
	
};







#endif
