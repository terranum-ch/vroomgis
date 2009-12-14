/***************************************************************************
								test_vrlayervectorOGR.h
								Test the OGR Layers
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

#ifndef _TEST_VR_LAYERVECTOROGR_H_
#define _TEST_VR_LAYERVECTOROGR_H_


#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "test_param.h"	//for test parameters
#include "vrlayervector.h"
#include "vrlayermanager.h"



class TEST_vrLayerVectorOGR : public CxxTest::TestSuite
{
public:
   	void testOpenLayerVectorOGR()
	{
		//init lib.
		vrLayerManager myManager;		

		vrLayerVectorOGR myLayer;
		TS_ASSERT_EQUALS(myLayer.Open(wxFileName(g_TestPath, g_TestFileSHP), true),true);
		TS_ASSERT_EQUALS(myLayer.GetType(), vrDRIVER_VECTOR_SHP);
		TS_ASSERT(myLayer.IsOK());
		TS_ASSERT_EQUALS(myLayer.Open(wxFileName(g_TestPath, g_TestFileSHP), true),true);
	}
	
	void testOpenLayerVectorOGRFailled()
	{
		vrLayerVectorOGR myLayer;
		TS_ASSERT_EQUALS(myLayer.Open(wxFileName(g_TestPath, g_TestFileMisc), false),false);
		TS_ASSERT_EQUALS(myLayer.IsOK(), false);
						
	}
	
	
	
};







#endif
