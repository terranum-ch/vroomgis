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

#include <gtest/gtest.h>

#include "test_param.h"  //for test parameters
#include "vrlayermanager.h"

class LayerManager : public ::testing::Test {
  protected:
    wxFileName m_JpegName;
    wxFileName m_TiffName;

    virtual void SetUp() {
        m_JpegName = wxFileName(g_TestPath, g_TestFileJPEG);
        m_TiffName = wxFileName(g_TestPath, g_TestFileMNT);
    }
};

TEST_F(LayerManager, CreateLayerManager) {
    vrLayerManager myLayerManager;
    EXPECT_FALSE(myLayerManager.Open(wxFileName(g_TestPath, g_TestFileMisc)));
    EXPECT_TRUE(myLayerManager.Open(wxFileName(g_TestPath, g_TestFileSHP)));
    EXPECT_EQ(myLayerManager.GetCount(), 1);
    // opening two times the same file
    EXPECT_TRUE(myLayerManager.Open(wxFileName(g_TestPath, g_TestFileSHP)));
    EXPECT_EQ(myLayerManager.GetCount(), 1);
}

TEST_F(LayerManager, AddingRaster) {
    vrLayerManager myLayerManager;

    EXPECT_TRUE(wxFileExists(m_JpegName.GetFullPath()));
    EXPECT_TRUE(myLayerManager.Open(m_JpegName));
    EXPECT_EQ(myLayerManager.GetCount(), 1);

    EXPECT_TRUE(myLayerManager.Open(m_TiffName));
    EXPECT_EQ(myLayerManager.GetCount(), 2);

    // opening two times the same file
    EXPECT_TRUE(myLayerManager.Open(m_JpegName));
    EXPECT_EQ(myLayerManager.GetCount(), 2);
    EXPECT_TRUE(myLayerManager.Open(m_TiffName));
    EXPECT_EQ(myLayerManager.GetCount(), 2);
}

TEST_F(LayerManager, GetType) {
    vrLayerManager myLayerManager;
    EXPECT_TRUE(myLayerManager.Open(m_JpegName));
    EXPECT_TRUE(myLayerManager.Open(m_TiffName));
    EXPECT_EQ(myLayerManager.GetCount(), 2);

    // testing type
    EXPECT_EQ(myLayerManager.GetLayer(m_JpegName)->GetType(), vrDRIVER_RASTER_JPEG);
    EXPECT_EQ(myLayerManager.GetLayer(m_TiffName)->GetType(), vrDRIVER_RASTER_TIFF);
}

TEST_F(LayerManager, RemoveLayer1) {
    vrLayerManager myLayerManager;
    EXPECT_TRUE(myLayerManager.Open(m_JpegName));
    EXPECT_TRUE(myLayerManager.Open(m_TiffName));
    EXPECT_EQ(myLayerManager.GetCount(), 2);

    // removing layer
    vrLayer* myLayer = myLayerManager.GetLayer(m_JpegName);
    EXPECT_TRUE(myLayer != nullptr);

    EXPECT_TRUE(myLayerManager.Close(myLayer));

    // myLayer isn't valid anymore dont test again !
    // EXPECT_TRUE(myLayerManager.Close(myLayer)==false);
}
