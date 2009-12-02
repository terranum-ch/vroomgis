/***************************************************************************
								test_vrlayermanager.h
				Manage the layers. Keep a list of all opened layers
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

#ifndef _TEST_VR_LAYERMANAGER_H_
#define _TEST_VR_LAYERMANAGER_H_


#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "test_param.h"	//for test parameters
#include "vrlayermanager.h"



class TEST_vrLayerManager : public CxxTest::TestSuite
{
public:
   	void testCreateLayerManager()
	{
		vrLayerManager myLayerManager;
		TS_ASSERT(myLayerManager.Open(wxFileName(g_TestPath, g_TestFileMisc))==false);
		TS_ASSERT(myLayerManager.Open(wxFileName(g_TestPath, g_TestFileSHP))==true);
	}
	
};







#endif
