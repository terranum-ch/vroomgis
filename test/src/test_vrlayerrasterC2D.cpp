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

#include <gtest/gtest.h>

#include "test_param.h"  //for test parameters
#include "vrlayer.h"
#include "vrlayermanager.h"
#include "vrlayerraster.h"
#include "vrlayerrasterc2d.h"
#include "vrlayervector.h"

class LayerRasterC2D : public ::testing::Test {
    friend class vrLayerRasterC2D;
};

TEST_F(LayerRasterC2D, OpenLayerRasterC2D) {
    GTEST_SKIP() << "Skipping C2D raster test, as it requires GDAL and C2D support.";
    // init lib.
    vrLayerManager myManager;

    vrLayerRasterGDAL myLayer;
    EXPECT_FALSE(myLayer.IsOK());
    EXPECT_EQ(myLayer.Open(wxFileName(g_TestPath, g_TestFileC2D), true), true);
    EXPECT_EQ(myLayer.GetType(), vrDRIVER_RASTER_C2D);
    EXPECT_TRUE(myLayer.IsOK());
}

TEST_F(LayerRasterC2D, GetPixelValueC2D) {
    GTEST_SKIP() << "Skipping C2D raster pixel value test, as it requires GDAL and C2D support.";
    wxLogMessage("Testing getting pixel value for C2D raster");
    vrRealRect myExtent;
    EXPECT_TRUE(myExtent.IsEmpty());

    // extent failed, layer not opened
    vrLayerRasterGDAL myLayer;
    EXPECT_FALSE(myLayer.GetExtent(myExtent));

    // extent ok for layer GDAL
    EXPECT_EQ(myLayer.Open(wxFileName(g_TestPath, g_TestFileC2D), false), true);
    EXPECT_EQ(myLayer.IsOK(), true);
    wxArrayDouble myValues;
    EXPECT_TRUE(myLayer.GetPixelValue(598100, 115000, myValues));
    EXPECT_TRUE(myValues.GetCount() == 3);

    wxString myTxtValues = "Value of px 598100 - 115000 = ";
    for (unsigned int i = 0; i < myValues.GetCount(); i++) {
        myTxtValues.Append(wxString::Format(" %.2f -", myValues.Item(i)));
    }
    myTxtValues.RemoveLast(1);
    wxLogMessage(myTxtValues);
}
