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

#include "vrlayer.h"
#include "vrlayerraster.h"
#include "vrlayervector.h"
#include "test_param.h"	//for test parameters
#include "vrlayerraster.h"
#include "vrlayermanager.h"


class LayerRasterGDAL : public ::testing::Test {
    friend class vrLayerRasterGDAL;
};

TEST_F(LayerRasterGDAL, OpenLayerRasterGDAL)
{
    //init lib.
    vrLayerManager myManager;

    vrLayerRasterGDAL myLayer;
    EXPECT_FALSE(myLayer.IsOK());
    EXPECT_EQ(myLayer.Open(wxFileName(g_TestPath, g_TestFileTIF), true), true);
    EXPECT_EQ(myLayer.GetType(), vrDRIVER_RASTER_TIFF);
    EXPECT_TRUE(myLayer.IsOK());
    EXPECT_EQ(myLayer.Open(wxFileName(g_TestPath, g_TestFileTIF), true), true);
}


TEST_F(LayerRasterGDAL, GetExtentRasterGDAL)
{
    wxLogMessage("Testing getting extent for GDAL extent");
    vrRealRect myExtent;
    EXPECT_TRUE(myExtent.IsEmpty());

    // extent failed, layer not opened
    vrLayerRasterGDAL myLayer;
    EXPECT_FALSE(myLayer.GetExtent(myExtent));

    // extent ok for layer GDAL
    EXPECT_EQ(myLayer.Open(wxFileName(g_TestPath, g_TestFileTIF), false), true);
    EXPECT_EQ(myLayer.IsOK(), true);
    EXPECT_TRUE(myLayer.GetExtent(myExtent));
    EXPECT_EQ((int) myExtent.GetLeft(), 598000);
    EXPECT_EQ((int) myExtent.GetTop(), 116000);
    EXPECT_EQ((int) myExtent.GetRight(), 599000);
    EXPECT_EQ((int) myExtent.GetBottom(), 114000);


}


TEST_F(LayerRasterGDAL, GettingExtentGDAL2)
{

    // GETTING EXTENT FOR ROTATED RASTERS RETURN MAX EXTENT
    vrRealRect myExtent;
    EXPECT_TRUE(myExtent.IsEmpty());

    // extent failed, layer not opened
    vrLayerRasterGDAL myLayer;
    EXPECT_FALSE(myLayer.GetExtent(myExtent));

    // extent ok for layer GDAL
    EXPECT_EQ(myLayer.Open(wxFileName(g_TestPath, g_TestFileJPEG), false), true);
    EXPECT_EQ(myLayer.IsOK(), true);
    EXPECT_TRUE(myLayer.GetExtent(myExtent));
    EXPECT_NEAR(myExtent.GetLeft(), 575147.743, 0.001);
    EXPECT_NEAR(myExtent.GetTop(), 169147.080, 0.001);
    EXPECT_NEAR(myExtent.GetRight(), 613503.148, 0.001);
    EXPECT_NEAR(myExtent.GetBottom(), 141898.483, 0.001);

    wxLogMessage("Windows extent is :\nleft : \t%.3f\nright : \t%.3f\ntop : \t%.3f\nbottom : \t%.3f",
                 myExtent.GetLeft(), myExtent.GetRight(), myExtent.GetTop(), myExtent.GetBottom());

}


TEST_F(LayerRasterGDAL, GetPixelValueGDAL)
{
    wxLogMessage("Testing getting pixel value for GDAL raster");
    vrRealRect myExtent;
    EXPECT_TRUE(myExtent.IsEmpty());

    // extent failed, layer not opened
    vrLayerRasterGDAL myLayer;
    EXPECT_FALSE(myLayer.GetExtent(myExtent));

    // extent ok for layer GDAL
    EXPECT_EQ(myLayer.Open(wxFileName(g_TestPath, g_TestFileMNT), false), true);
    EXPECT_EQ(myLayer.IsOK(), true);
    wxArrayDouble myValues;
    EXPECT_TRUE(myLayer.GetPixelValue(598100, 115000, myValues));
    EXPECT_TRUE(myValues.GetCount() != 0);

    EXPECT_NEAR(myValues.Item(0), 754.5999, 0.001);

    wxString myTxtValues = "Value of px 598100 - 115000 = ";
    for (unsigned int i = 0; i < myValues.GetCount(); i++) {
        myTxtValues.Append(wxString::Format(" %.2f -", myValues.Item(i)));
    }
    myTxtValues.RemoveLast(1);
    wxLogMessage(myTxtValues);

}
