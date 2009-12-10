/***************************************************************************
								test_vrrender.h
							Rendering main class for data
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

#ifndef _TEST_VR_RENDER_H_
#define _TEST_VR_RENDER_H_


#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

//#include "test_param.h"	//for test parameters
#include "vrrender.h"


class TEST_vrRender : public CxxTest::TestSuite
{
private:
	
public:
	void testRenderBasic()
	{
		vrRender myRender;
		vrRenderVector myRenderVect;
		vrRenderRaster myRenderRast;
		vrRenderRasterColtop myRenderRastc2d;
		TS_ASSERT_EQUALS(myRender.GetType(), vrRENDER_UNKNOWN);
		TS_ASSERT_EQUALS(myRenderVect.GetType(), vrRENDER_VECTOR);
		TS_ASSERT_EQUALS(myRenderRast.GetType(), vrRENDER_RASTER);
		TS_ASSERT_EQUALS(myRenderRastc2d.GetType(), vrRENDER_RASTER_C2D);
		
	}
	
	void testRenderPointer()
	{
		vrRender * mypRender = NULL;
		vrRenderVector myRenderVect;
		vrRenderRaster myRenderRast;
		vrRenderRasterColtop myRenderRastc2d;
		
		mypRender = & myRenderVect;
		TS_ASSERT_EQUALS(mypRender->GetType(), vrRENDER_VECTOR);
		
		mypRender = &myRenderRast;
		TS_ASSERT_EQUALS(mypRender->GetType(), vrRENDER_RASTER);
		
		mypRender = &myRenderRastc2d;
		TS_ASSERT_EQUALS(mypRender->GetType(), vrRENDER_RASTER_C2D);
		
	}
	
};







#endif
