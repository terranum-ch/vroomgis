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

#include <gtest/gtest.h>

#include "test_param.h"
#include "vroomgis.h"

class ViewerLayerManager : public ::testing::Test {
  protected:
    vrLayerManager* m_LayerManager;
    vrViewerLayerManager* m_ViewManager;

    virtual void SetUp() {
        m_LayerManager = new vrLayerManager();
        m_ViewManager = NULL;
        vrViewerDisplay* myDisplay = NULL;
        // will be destroyed by layermanager.
        m_ViewManager = new vrViewerLayerManager(m_LayerManager, NULL, myDisplay);

        // try to add the viewer. But was already added by ctor.
        EXPECT_EQ(m_LayerManager->AddViewerLayerManager(m_ViewManager), false);
    }

    virtual void tearDown() {
        delete m_LayerManager;
    }
};

TEST_F(ViewerLayerManager, AddViewerLayerManager) {
    // adding two times the same viewermanager is not permitted
    EXPECT_FALSE(m_LayerManager->AddViewerLayerManager(m_ViewManager));
    vrViewerLayerManager* myManager2 = new vrViewerLayerManager(m_LayerManager, NULL, NULL);

    // allready added by ctor
    EXPECT_FALSE(m_LayerManager->AddViewerLayerManager(myManager2));
}

TEST_F(ViewerLayerManager, OpenGISDataViewerManager) {
    // open data
    EXPECT_TRUE(m_LayerManager->Open(wxFileName(g_TestPath, g_TestFileSHP)));
    vrLayer* myTestSHPLayer = NULL;
    myTestSHPLayer = m_LayerManager->GetLayer(wxFileName(g_TestPath, g_TestFileSHP));
    EXPECT_TRUE(myTestSHPLayer != NULL);
    EXPECT_EQ(myTestSHPLayer->GetType(), vrDRIVER_VECTOR_SHP);

    // open data 2
    EXPECT_TRUE(m_LayerManager->Open(wxFileName(g_TestPath, g_TestFileJPEG)));
    vrLayer* myTestJpegLayer = NULL;
    myTestJpegLayer = m_LayerManager->GetLayer(wxFileName(g_TestPath, g_TestFileJPEG));
    EXPECT_TRUE(myTestJpegLayer != NULL);
    EXPECT_EQ(myTestJpegLayer->GetType(), vrDRIVER_RASTER_JPEG);

    // add data to the viewermanager
    EXPECT_TRUE(m_ViewManager->Add(-1, myTestSHPLayer));
    EXPECT_TRUE(m_ViewManager->Add(0, myTestJpegLayer));
}

TEST_F(ViewerLayerManager, GetRenderer) {
    // open data
    EXPECT_TRUE(m_LayerManager->Open(wxFileName(g_TestPath, g_TestFileSHP)));
    vrLayer* myTestSHPLayer = NULL;
    myTestSHPLayer = m_LayerManager->GetLayer(wxFileName(g_TestPath, g_TestFileSHP));
    EXPECT_TRUE(myTestSHPLayer != NULL);
    EXPECT_EQ(myTestSHPLayer->GetType(), vrDRIVER_VECTOR_SHP);

    // add data to the viewermanager
    EXPECT_TRUE(m_ViewManager->Add(-1, myTestSHPLayer));

    // renderer should be accessible
    vrRenderer* myRenderer = m_ViewManager->GetRenderer(0);
    EXPECT_TRUE(myRenderer != NULL);
    EXPECT_EQ(myRenderer->GetLayer(), myTestSHPLayer);

    // when adding, status should be visible
    EXPECT_TRUE(myRenderer->GetVisible());

    // trying to get out bounds renderer
    myRenderer = m_ViewManager->GetRenderer(12);
    EXPECT_TRUE(myRenderer == NULL);
}

TEST_F(ViewerLayerManager, Remove) {
    // open data
    EXPECT_TRUE(m_LayerManager->Open(wxFileName(g_TestPath, g_TestFileSHP)));
    vrLayer* myTestSHPLayer = NULL;
    myTestSHPLayer = m_LayerManager->GetLayer(wxFileName(g_TestPath, g_TestFileSHP));
    EXPECT_TRUE(myTestSHPLayer != NULL);
    EXPECT_EQ(myTestSHPLayer->GetType(), vrDRIVER_VECTOR_SHP);

    // add data to the viewermanager
    EXPECT_TRUE(m_ViewManager->Add(-1, myTestSHPLayer));

    // renderer should be accessible
    vrRenderer* myRenderer = m_ViewManager->GetRenderer(0);
    EXPECT_TRUE(myRenderer != NULL);
    EXPECT_EQ(myRenderer->GetLayer(), myTestSHPLayer);

    // testing deleting
    EXPECT_EQ(m_ViewManager->Remove(myRenderer), true);
    EXPECT_EQ(m_ViewManager->Remove(myRenderer), false);
}

TEST_F(ViewerLayerManager, MoveLayer) {
    wxLogMessage("Testing moving layer");
    EXPECT_TRUE(m_LayerManager->Open(wxFileName(g_TestPath, g_TestFileSHP)));
    EXPECT_TRUE(m_LayerManager->Open(wxFileName(g_TestPath, g_TestFileMNT)));
    EXPECT_TRUE(m_LayerManager->Open(wxFileName(g_TestPath, g_TestFileJPEG)));
    EXPECT_EQ(m_LayerManager->GetCount(), 3);

    vrLayer* myLayer1 = m_LayerManager->GetLayer(wxFileName(g_TestPath, g_TestFileJPEG));
    vrLayer* myLayer2 = m_LayerManager->GetLayer(wxFileName(g_TestPath, g_TestFileMNT));
    vrLayer* myLayer3 = m_LayerManager->GetLayer(wxFileName(g_TestPath, g_TestFileSHP));

    EXPECT_TRUE(m_ViewManager->Add(-1, myLayer3));
    EXPECT_TRUE(m_ViewManager->Add(-1, myLayer2));
    EXPECT_TRUE(m_ViewManager->Add(-1, myLayer1));

    // Moving code
    wxString myJpegName = m_ViewManager->GetRenderer(0)->GetLayer()->GetDisplayName().GetFullName();
    // wxLogMessage (myJpegName + " - " + g_TestFileJPEG);
    EXPECT_TRUE(myJpegName == g_TestFileJPEG);
    EXPECT_TRUE(m_ViewManager->Move(2, 0));
    EXPECT_TRUE(m_ViewManager->GetRenderer(0)->GetLayer()->GetDisplayName().GetFullName() == g_TestFileSHP);
    EXPECT_TRUE(m_ViewManager->Move(0, 2));
    EXPECT_TRUE(m_ViewManager->GetRenderer(0)->GetLayer()->GetDisplayName().GetFullName() == g_TestFileJPEG);
}
