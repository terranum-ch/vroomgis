/***************************************************************************
 test_vroperationvector
 -------------------
 copyright            : (C) 2013 CREALP Lucien Schreiber
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

#include "gtest/gtest.h"

#include <wx/filename.h>

#include "test_param.h"
#include "vroperationvectorprofiler.h"
#include "vrlayervector.h"
#include "vrlayerraster.h"
#include "vrlayermanager.h"

class OperationVector : public ::testing::Test {
private:
    vrLayerRasterGDAL *myRaster;
    vrLayerVectorOGR *myVector;


public:
    void setUp() {
        OGRRegisterAll();
        GDALAllRegister();

        wxFileName myRasterName(g_TestPath, g_TestFileMNT);
        myRaster = new vrLayerRasterGDAL();
        myRaster->Open(myRasterName);

        wxFileName myVectorName(g_TestPath, g_TestProfile);
        myVector = new vrLayerVectorOGR();
        myVector->Open(myVectorName);
    }

    void tearDown() {
        wxDELETE(myRaster);
        wxDELETE(myVector);
    }
};

TEST_F(OperationVector, CreateProfileObject()
{
    EXPECT_TRUE(myRaster->IsOK());
    EXPECT_TRUE(myVector->IsOK());

    OGRFeature *myFeat = myVector->GetNextFeature(true);
    vrOperationVectorProfiler myProfiler(myFeat->GetGeometryRef(), myRaster);
    EXPECT_TRUE(myProfiler.IsOk());

    OGRFeature::DestroyFeature(myFeat);

}


TEST_F(OperationVector, DoProfile()
{
    OGRFeature *myFeat = myVector->GetNextFeature(true);
    vrOperationVectorProfiler myProfiler(myFeat->GetGeometryRef(), myRaster);
    EXPECT_TRUE(myProfiler.DoProfile());
    EXPECT_TRUE(myProfiler.GetResultRef()->GetCount() > 0);
    OGRFeature::DestroyFeature(myFeat);
}


TEST_F(OperationVector, ProfileOut()
{
    vrLayerManager myManager;
    wxFileName myOutPtName(g_TestPath, g_TestProfileOutPt);
    myManager.Erase(myOutPtName);

    wxFileName myOutLineName(g_TestPath, g_TestProfileOutLine);
    myManager.Erase(myOutLineName);

    vrLayerVectorOGR myOutPt;
    EXPECT_TRUE(myOutPt.Create(myOutPtName, wkbPoint));


    OGRFeature *myFeat = myVector->GetNextFeature(true);
    vrOperationVectorProfiler myProfiler(myFeat->GetGeometryRef(), myRaster);
    EXPECT_TRUE(myProfiler.DoProfile());
    int myNumResult = myProfiler.GetResultRef()->GetCount();

    for (unsigned int i = 0; i < myNumResult; i++) {
        OGRPoint myPt;
        EXPECT_TRUE(myProfiler.GetResultPoint(i, &myPt));
        EXPECT_TRUE(myOutPt.AddFeature(&myPt) != wxNOT_FOUND);
    }


    vrLayerVectorOGR myOutLine;
    EXPECT_TRUE(myOutLine.Create(myOutLineName, wkbLineString));
    OGRLineString myLine;
    EXPECT_TRUE(myProfiler.GetResultLine(&myLine));
    EXPECT_TRUE(myOutLine.AddFeature(&myLine) != wxNOT_FOUND);

}


TEST_F(OperationVector, ProfileGeometries()
{
    OGRFeature *myFeat = myVector->GetNextFeature(true);
    vrOperationVectorProfiler myProfiler(myFeat->GetGeometryRef(), myRaster);
    EXPECT_TRUE(myProfiler.DoProfile());
    //int myNumResult = myProfiler.GetResultRef()->GetCount();

    OGRPoint myPt0;
    OGRPoint myPtEnd;
    OGRLineString *myLine = static_cast<OGRLineString *> (myFeat->GetGeometryRef());

    EXPECT_TRUE(myProfiler.GetResultPoint(0, &myPt0));
    EXPECT_TRUE(myPt0.getX() == myLine->getX(0));
    EXPECT_TRUE(myPt0.getY() == myLine->getY(0));

    EXPECT_TRUE(myProfiler.GetIncrementDistance() > 0);

    OGRFeature::DestroyFeature(myFeat);
}
