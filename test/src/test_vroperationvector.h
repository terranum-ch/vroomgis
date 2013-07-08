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

#ifndef _TEST_VR_OPERATION_VECTOR_H_
#define _TEST_VR_OPERATION_VECTOR_H_

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <wx/filename.h>

#include "test_param.h"
#include "vroperationvectorprofiler.h"
#include "vrlayervector.h"
#include "vrlayerraster.h"
#include "vrlayermanager.h"

class TEST_vrOperationVector : public CxxTest::TestSuite
{
private:
    vrLayerRasterGDAL * myRaster;
    vrLayerVectorOGR * myVector;

    
public:
   	void setUp() {
        OGRRegisterAll();
        GDALAllRegister();
        
        wxFileName myRasterName (g_TestPath, g_TestFileMNT);
        myRaster = new vrLayerRasterGDAL();
        myRaster->Open(myRasterName);
        
        wxFileName myVectorName (g_TestPath, g_TestProfile);
        myVector = new vrLayerVectorOGR();
        myVector->Open(myVectorName);
    }
    
    void tearDown() {
        wxDELETE(myRaster);
        wxDELETE(myVector);
    }
    
    
    void testvrOperationVector(){
		wxLogMessage("*************************");
		wxLogMessage("TESTING VECTOR OPERATION");
		wxLogMessage("*************************");
    }
    
    
    void testCreateProfileObject() {
        TS_ASSERT(myRaster->IsOK());
        TS_ASSERT(myVector->IsOK());
        
        OGRFeature * myFeat = myVector->GetNextFeature(true);
        vrOperationVectorProfiler myProfiler ( myFeat->GetGeometryRef() , myRaster);
        TS_ASSERT(myProfiler.IsOk());
        
        OGRFeature::DestroyFeature(myFeat);
        
    }
    
    
    
    void testDoProfile () {
        OGRFeature * myFeat = myVector->GetNextFeature(true);
        vrOperationVectorProfiler myProfiler ( myFeat->GetGeometryRef() , myRaster);
        TS_ASSERT(myProfiler.DoProfile());
        TS_ASSERT(myProfiler.GetResultRef()->GetCount() > 0);
        OGRFeature::DestroyFeature(myFeat);
    }
    
    
    
    void testProfileOut () {
        vrLayerManager myManager;
        wxFileName myOutPtName (g_TestPath, g_TestProfileOutPt);
        myManager.Erase(myOutPtName);
        
        wxFileName myOutLineName (g_TestPath, g_TestProfileOutLine);
        myManager.Erase(myOutLineName);
        
        vrLayerVectorOGR myOutPt;
        TS_ASSERT(myOutPt.Create(myOutPtName, wkbPoint));
        
        
        OGRFeature * myFeat = myVector->GetNextFeature(true);
        vrOperationVectorProfiler myProfiler ( myFeat->GetGeometryRef() , myRaster);
        TS_ASSERT(myProfiler.DoProfile());
        int myNumResult = myProfiler.GetResultRef()->GetCount();
        
        for (unsigned int i = 0; i< myNumResult; i++) {
            OGRPoint myPt;
            TS_ASSERT(myProfiler.GetResultPoint(i, &myPt));
            TS_ASSERT(myOutPt.AddFeature(&myPt) != wxNOT_FOUND);
        }
    
    
    vrLayerVectorOGR myOutLine;
    TS_ASSERT(myOutLine.Create(myOutLineName, wkbLineString));
    OGRLineString myLine;
    TS_ASSERT(myProfiler.GetResultLine(&myLine));
    TS_ASSERT(myOutLine.AddFeature(&myLine) != wxNOT_FOUND);
    
    }
    
    
    void testProfileGeometries () {
        OGRFeature * myFeat = myVector->GetNextFeature(true);
        vrOperationVectorProfiler myProfiler ( myFeat->GetGeometryRef() , myRaster);
        TS_ASSERT(myProfiler.DoProfile());
        //int myNumResult = myProfiler.GetResultRef()->GetCount();

        OGRPoint myPt0;
        OGRPoint myPtEnd;
        OGRLineString * myLine = static_cast<OGRLineString*> (myFeat->GetGeometryRef());

        TS_ASSERT(myProfiler.GetResultPoint(0,&myPt0));
        TS_ASSERT(myPt0.getX() == myLine->getX(0));
        TS_ASSERT(myPt0.getY() == myLine->getY(0));
        
        TS_ASSERT(myProfiler.GetIncrementDistance() > 0);
              
        OGRFeature::DestroyFeature(myFeat);
    }
    
};





#endif
