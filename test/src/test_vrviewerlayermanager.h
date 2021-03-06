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
#include "vroomgis.h"


class TEST_vrViewerLayerManager : public CxxTest::TestSuite
{
private:
    vrLayerManager *m_LayerManager;
    vrViewerLayerManager *m_ViewManager;

public:
    void setUp()
    {
        m_LayerManager = new vrLayerManager();
        m_ViewManager = NULL;
        vrViewerDisplay *myDisplay = NULL;
        // will be destroyed by layermanager.
        m_ViewManager = new vrViewerLayerManager(m_LayerManager, NULL, myDisplay);

        // try to add the viewer. But was allready added by ctor.
        TS_ASSERT_EQUALS(m_LayerManager->AddViewerLayerManager(m_ViewManager), false);
    }

    void tearDown()
    {
        delete m_LayerManager;
    }

    void testAddViewerLayerManager()
    {
        // adding two times the same viewermanager is not permitted
        TS_ASSERT(!m_LayerManager->AddViewerLayerManager(m_ViewManager));
        vrViewerLayerManager *myManager2 = new vrViewerLayerManager(m_LayerManager, NULL, NULL);

        // allready added by ctor
        TS_ASSERT(!m_LayerManager->AddViewerLayerManager(myManager2));
    }

    void testOpenGISDataViewerManager()
    {


        // open data
        TS_ASSERT(m_LayerManager->Open(wxFileName(g_TestPath, g_TestFileSHP)));
        vrLayer *myTestSHPLayer = NULL;
        myTestSHPLayer = m_LayerManager->GetLayer(wxFileName(g_TestPath, g_TestFileSHP));
        TS_ASSERT(myTestSHPLayer != NULL);
        TS_ASSERT_EQUALS(myTestSHPLayer->GetType(), vrDRIVER_VECTOR_SHP);

        // open data 2
        TS_ASSERT(m_LayerManager->Open(wxFileName(g_TestPath, g_TestFileJPEG)));
        vrLayer *myTestJpegLayer = NULL;
        myTestJpegLayer = m_LayerManager->GetLayer(wxFileName(g_TestPath, g_TestFileJPEG));
        TS_ASSERT(myTestJpegLayer != NULL);
        TS_ASSERT_EQUALS(myTestJpegLayer->GetType(), vrDRIVER_RASTER_JPEG);


        // add data to the viewermanager
        TS_ASSERT(m_ViewManager->Add(-1, myTestSHPLayer));
        TS_ASSERT(m_ViewManager->Add(0, myTestJpegLayer));

    }

    void testGetRenderer()
    {


        // open data
        TS_ASSERT(m_LayerManager->Open(wxFileName(g_TestPath, g_TestFileSHP)));
        vrLayer *myTestSHPLayer = NULL;
        myTestSHPLayer = m_LayerManager->GetLayer(wxFileName(g_TestPath, g_TestFileSHP));
        TS_ASSERT(myTestSHPLayer != NULL);
        TS_ASSERT_EQUALS(myTestSHPLayer->GetType(), vrDRIVER_VECTOR_SHP);

        // add data to the viewermanager
        TS_ASSERT(m_ViewManager->Add(-1, myTestSHPLayer));

        // renderer should be accessible
        vrRenderer *myRenderer = m_ViewManager->GetRenderer(0);
        TS_ASSERT(myRenderer != NULL);
        TS_ASSERT_EQUALS(myRenderer->GetLayer(), myTestSHPLayer);

        // when adding, status should be visible
        TS_ASSERT(myRenderer->GetVisible());


        // trying to get out bounds renderer
        myRenderer = m_ViewManager->GetRenderer(12);
        TS_ASSERT(myRenderer == NULL);

    }

    void testRemove()
    {

        // open data
        TS_ASSERT(m_LayerManager->Open(wxFileName(g_TestPath, g_TestFileSHP)));
        vrLayer *myTestSHPLayer = NULL;
        myTestSHPLayer = m_LayerManager->GetLayer(wxFileName(g_TestPath, g_TestFileSHP));
        TS_ASSERT(myTestSHPLayer != NULL);
        TS_ASSERT_EQUALS(myTestSHPLayer->GetType(), vrDRIVER_VECTOR_SHP);

        // add data to the viewermanager
        TS_ASSERT(m_ViewManager->Add(-1, myTestSHPLayer));

        // renderer should be accessible
        vrRenderer *myRenderer = m_ViewManager->GetRenderer(0);
        TS_ASSERT(myRenderer != NULL);
        TS_ASSERT_EQUALS(myRenderer->GetLayer(), myTestSHPLayer);

        // testing deleting
        TS_ASSERT_EQUALS(m_ViewManager->Remove(myRenderer), true);
        TS_ASSERT_EQUALS(m_ViewManager->Remove(myRenderer), false);
    }


    void testMoveLayer()
    {
        wxLogMessage("Testing moving layer");
        TS_ASSERT(m_LayerManager->Open(wxFileName(g_TestPath, g_TestFileSHP)));
        TS_ASSERT(m_LayerManager->Open(wxFileName(g_TestPath, g_TestFileTIF)));
        TS_ASSERT(m_LayerManager->Open(wxFileName(g_TestPath, g_TestFileJPEG)));
        TS_ASSERT_EQUALS(m_LayerManager->GetCount(), 3);

        vrLayer *myLayer1 = m_LayerManager->GetLayer(wxFileName(g_TestPath, g_TestFileJPEG));
        vrLayer *myLayer2 = m_LayerManager->GetLayer(wxFileName(g_TestPath, g_TestFileTIF));
        vrLayer *myLayer3 = m_LayerManager->GetLayer(wxFileName(g_TestPath, g_TestFileSHP));

        TS_ASSERT(m_ViewManager->Add(-1, myLayer3));
        TS_ASSERT(m_ViewManager->Add(-1, myLayer2));
        TS_ASSERT(m_ViewManager->Add(-1, myLayer1));




        // Moving code
        wxString myJpegName = m_ViewManager->GetRenderer(0)->GetLayer()->GetDisplayName().GetFullName();
        //wxLogMessage (myJpegName + " - " + g_TestFileJPEG);
        TS_ASSERT(myJpegName == g_TestFileJPEG);
        TS_ASSERT(m_ViewManager->Move(2, 0));
        TS_ASSERT(m_ViewManager->GetRenderer(0)->GetLayer()->GetDisplayName().GetFullName() == g_TestFileSHP);
        TS_ASSERT(m_ViewManager->Move(0, 2));
        TS_ASSERT(m_ViewManager->GetRenderer(0)->GetLayer()->GetDisplayName().GetFullName() == g_TestFileJPEG);
    }


};


#endif
