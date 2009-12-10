/***************************************************************************
							test_vrviewerlayermanager.h
							Manage a view for the viewer
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

#ifndef _TEST_VRVIEWERLAYERMANAGER_H_
#define _TEST_VRVIEWERLAYERMANAGER_H_


#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "test_param.h"	//for test parameters
#include "vrviewerlayermanager.h"



class TEST_vrViewerLayerManager : public CxxTest::TestSuite
{
private:
	
public:
	void setUp()
	{
	}
	
    void testCreatevrViewerLayerManager()
	{
		vrLayerManager myLayerManager;
		vrViewerDisplay * myDisplay = NULL;
		vrViewerLayerManager * myManager = new vrViewerLayerManager(&myLayerManager, myDisplay);
		TS_ASSERT(myLayerManager.AddViewerLayerManager(myManager)==true);
		// adding same manager is permitted only once
		TS_ASSERT(myLayerManager.AddViewerLayerManager(myManager)==false);
		vrViewerLayerManager * myManager2 = new vrViewerLayerManager(&myLayerManager, myDisplay);
		TS_ASSERT(myLayerManager.AddViewerLayerManager(myManager2)==true);
	}
	
	void testOpenGISDataViewerManager()
	{
		vrLayerManager myLayerManager;
		vrViewerLayerManager * myManager = new vrViewerLayerManager(&myLayerManager, NULL);
		TS_ASSERT(myLayerManager.AddViewerLayerManager(myManager)==true);
		
		// open data
		TS_ASSERT(myLayerManager.Open(wxFileName(g_TestPath, g_TestFileSHP)));
		vrLayer * myTestSHPLayer = NULL;
		myTestSHPLayer = myLayerManager.GetLayer(wxFileName(g_TestPath, g_TestFileSHP));
		TS_ASSERT(myTestSHPLayer != NULL);
		TS_ASSERT_EQUALS(myTestSHPLayer->GetType(), vrDRIVER_VECTOR_SHP);
		
		// add data to the viewermanager
		TS_ASSERT(myManager->Add(-1, myTestSHPLayer)==true);
	}
	
};







#endif
