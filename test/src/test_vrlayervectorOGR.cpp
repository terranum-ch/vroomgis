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

#include <gtest/gtest.h>

#include "test_param.h"    //for test parameters
#include "vrlayervector.h"
#include "vrlayermanager.h"


TEST(LayerVectorOGR, OpenLayerVectorOGR) {
    //init lib.
    vrLayerManager myManager;

    vrLayerVectorOGR myLayer;
    EXPECT_EQ(myLayer.Open(wxFileName(g_TestPath, g_TestFileSHP), true), true);
    EXPECT_EQ(myLayer.GetType(), vrDRIVER_VECTOR_SHP);
    EXPECT_TRUE(myLayer.IsOK());
    EXPECT_EQ(myLayer.Open(wxFileName(g_TestPath, g_TestFileSHP), true), true);
}

TEST(LayerVectorOGR, OpenLayerVectorOGRFailled) {
    vrLayerVectorOGR myLayer;
    EXPECT_EQ(myLayer.Open(wxFileName(g_TestPath, g_TestFileMisc), false), false);
    EXPECT_EQ(myLayer.IsOK(), false);

}

TEST(LayerVectorOGR, GetExtentVectorOGR) {

    vrRealRect myExtent;
    EXPECT_TRUE(myExtent.IsEmpty());

    // extent failed, layer not opened
    vrLayerVectorOGR myLayer;
    EXPECT_FALSE(myLayer.GetExtent(myExtent));

    // extent ok for layer ogr
    EXPECT_EQ(myLayer.Open(wxFileName(g_TestPath, g_TestFileSHP), false), true);
    EXPECT_EQ(myLayer.IsOK(), true);
    EXPECT_TRUE(myLayer.GetExtent(myExtent));
    EXPECT_EQ((int) myExtent.GetLeft(), 598000);
    EXPECT_EQ((int) myExtent.GetTop(), 117200);
    EXPECT_EQ((int) myExtent.GetRight(), 598662);
    EXPECT_EQ((int) myExtent.GetBottom(), 114173);
}

TEST(LayerVectorOGR, GettingGeometry) {
    vrLayerVectorOGR myLayer;

    // layer not opened
    OGRFeature *myFeat = myLayer.GetFeature(0);
    EXPECT_TRUE(myFeat == NULL);

    EXPECT_EQ(myLayer.Open(wxFileName(g_TestPath, g_TestFileSHP2), false), true);
    EXPECT_EQ(myLayer.IsOK(), true);

    OGRFeature *myGeom = NULL;
    myGeom = myLayer.GetFeature(0);
    EXPECT_TRUE(myGeom != NULL);

    OGRLineString *myLine = (OGRLineString *) myGeom->GetGeometryRef();
    wxLogMessage("line returned start here : %.4f | %.4f and contain %d vertex", myLine->getX(0), myLine->getY(0),
                 myLine->getNumPoints());

    OGRFeature::DestroyFeature(myGeom);

}

TEST(LayerVectorOGR, GettingGeometryType) {
    vrLayerVectorOGR myLayer;

    // layer not opened
    EXPECT_TRUE(myLayer.GetGeometryType() == wkbUnknown);

    EXPECT_EQ(myLayer.Open(wxFileName(g_TestPath, g_TestFileSHP2), false), true);
    EXPECT_EQ(myLayer.IsOK(), true);
    EXPECT_EQ(myLayer.GetGeometryType(), wkbLineString);

    EXPECT_EQ(myLayer.Open(wxFileName(g_TestPath, g_TestFileSHP), false), true);
    EXPECT_EQ(myLayer.GetGeometryType(), wkbPolygon);
}

TEST(LayerVectorOGR, GettingNextGeometry) {
    wxLogMessage("Testing getting next data for layer");

    vrLayerVectorOGR myLayer;
    EXPECT_EQ(myLayer.Open(wxFileName(g_TestPath, g_TestFileSHP2), false), true);
    EXPECT_EQ(myLayer.IsOK(), true);

    bool restart = true;
    OGRFeature *myFeat = NULL;
    int iCount = 0;

    while (1) {
        myFeat = myLayer.GetNextFeature(restart);
        restart = false;
        if (myFeat == NULL) {
            break;
        }

        wxLogMessage("oid : %ld contain %d vertex", myFeat->GetFID(),
                     (((OGRLineString *) myFeat->GetGeometryRef())->getNumPoints()));
        iCount++;
        OGRFeature::DestroyFeature(myFeat);

    }
    wxLogMessage("%d feature readed", iCount);
    EXPECT_EQ(iCount, 45);
}
