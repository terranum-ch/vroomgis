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
#include "../../src/vroomgis.h"




class TEST_vrViewerLayerManager : public CxxTest::TestSuite
{
private:
	vrLayerManager * m_LayerManager;
	vrViewerLayerManager * m_ViewManager;
	
public:
	void setUp()
	{
		m_LayerManager = new vrLayerManager();
		m_ViewManager = NULL;
		vrViewerDisplay * myDisplay = NULL;
		// will be destroyed by layermanager.
		m_ViewManager = new vrViewerLayerManager(m_LayerManager, myDisplay);
		TS_ASSERT_EQUALS(m_LayerManager->AddViewerLayerManager(m_ViewManager),true);
	}
	
	void tearDown()
	{
		delete m_LayerManager;
	}
	
    void testAddViewerLayerManager()
	{
		// adding two times the same viewermanager is not permitted
		TS_ASSERT(m_LayerManager->AddViewerLayerManager(m_ViewManager)==false);
		vrViewerLayerManager * myManager2 = new vrViewerLayerManager(m_LayerManager, NULL);
		TS_ASSERT(m_LayerManager->AddViewerLayerManager(myManager2)==true);
	}
	
	void testOpenGISDataViewerManager()
	{
		
		
		// open data
		TS_ASSERT(m_LayerManager->Open(wxFileName(g_TestPath, g_TestFileSHP)));
		vrLayer * myTestSHPLayer = NULL;
		myTestSHPLayer = m_LayerManager->GetLayer(wxFileName(g_TestPath, g_TestFileSHP));
		TS_ASSERT(myTestSHPLayer != NULL);
		TS_ASSERT_EQUALS(myTestSHPLayer->GetType(), vrDRIVER_VECTOR_SHP);
		
		// open data 2
		TS_ASSERT(m_LayerManager->Open(wxFileName(g_TestPath, g_TestFileJPEG)));
		vrLayer * myTestJpegLayer = NULL;
		myTestJpegLayer = m_LayerManager->GetLayer(wxFileName(g_TestPath, g_TestFileJPEG));
		TS_ASSERT(myTestJpegLayer != NULL);
		TS_ASSERT_EQUALS(myTestJpegLayer->GetType(), vrDRIVER_RASTER_JPEG);
		
			
		// add data to the viewermanager
		TS_ASSERT(m_ViewManager->Add(-1, myTestSHPLayer)==true);
		TS_ASSERT(m_ViewManager->Add(0, myTestJpegLayer)==true);
			
	}
	
	void testGetRenderer() {
		wxLogMessage("Testing getting renderer");
		
		// open data
		TS_ASSERT(m_LayerManager->Open(wxFileName(g_TestPath, g_TestFileSHP)));
		vrLayer * myTestSHPLayer = NULL;
		myTestSHPLayer = m_LayerManager->GetLayer(wxFileName(g_TestPath, g_TestFileSHP));
		TS_ASSERT(myTestSHPLayer != NULL);
		TS_ASSERT_EQUALS(myTestSHPLayer->GetType(), vrDRIVER_VECTOR_SHP);
		
		// add data to the viewermanager
		TS_ASSERT(m_ViewManager->Add(-1, myTestSHPLayer)==true);
		
		// renderer should be accessible
		vrRenderer * myRenderer = m_ViewManager->GetRenderer(0);
		TS_ASSERT(myRenderer != NULL);
		TS_ASSERT_EQUALS(myRenderer->GetLayer(), myTestSHPLayer);
		
		// when adding, status should be visible
		TS_ASSERT(myRenderer->GetVisible());
		
		
		// trying to get out bounds renderer
		myRenderer = m_ViewManager->GetRenderer(12);
		TS_ASSERT(myRenderer == NULL);
		
	}
	


};







#endif
