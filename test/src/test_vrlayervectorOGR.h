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

#ifndef _TEST_VR_LAYERVECTOROGR_H_
#define _TEST_VR_LAYERVECTOROGR_H_


#include "wx/wxprec.h"

#ifndef WX_PRECOMP

#include <wx/wx.h>

#endif


#include "test_param.h"	//for test parameters
#include "vrlayervector.h"
#include "vrlayermanager.h"


class TEST_vrLayerVectorOGR
        : public CxxTest::TestSuite
{
public:
    void testOpenLayerVectorOGR()
    {
        //init lib.
        vrLayerManager myManager;

        vrLayerVectorOGR myLayer;
        TS_ASSERT_EQUALS(myLayer.Open(wxFileName(g_TestPath, g_TestFileSHP), true), true);
        TS_ASSERT_EQUALS(myLayer.GetType(), vrDRIVER_VECTOR_SHP);
        TS_ASSERT(myLayer.IsOK());
        TS_ASSERT_EQUALS(myLayer.Open(wxFileName(g_TestPath, g_TestFileSHP), true), true);
    }

    void testOpenLayerVectorOGRFailled()
    {
        vrLayerVectorOGR myLayer;
        TS_ASSERT_EQUALS(myLayer.Open(wxFileName(g_TestPath, g_TestFileMisc), false), false);
        TS_ASSERT_EQUALS(myLayer.IsOK(), false);

    }

    void testGetExtentVectorOGR()
    {

        vrRealRect myExtent;
        TS_ASSERT(myExtent.IsEmpty());

        // extent failed, layer not opened
        vrLayerVectorOGR myLayer;
        TS_ASSERT(!myLayer.GetExtent(myExtent));

        // extent ok for layer ogr
        TS_ASSERT_EQUALS(myLayer.Open(wxFileName(g_TestPath, g_TestFileSHP), false), true);
        TS_ASSERT_EQUALS(myLayer.IsOK(), true);
        TS_ASSERT(myLayer.GetExtent(myExtent));
        TS_ASSERT_EQUALS((int) myExtent.GetLeft(), 598000);
        TS_ASSERT_EQUALS((int) myExtent.GetTop(), 117200);
        TS_ASSERT_EQUALS((int) myExtent.GetRight(), 598662);
        TS_ASSERT_EQUALS((int) myExtent.GetBottom(), 114173);
    }


    void testGettingGeometry()
    {
        vrLayerVectorOGR myLayer;

        // layer not opened
        OGRFeature *myFeat = myLayer.GetFeature(0);
        TS_ASSERT(myFeat == NULL);


        TS_ASSERT_EQUALS(myLayer.Open(wxFileName(g_TestPath, g_TestFileSHP2), false), true);
        TS_ASSERT_EQUALS(myLayer.IsOK(), true);

        OGRFeature *myGeom = NULL;
        myGeom = myLayer.GetFeature(0);
        TS_ASSERT(myGeom != NULL);

        OGRLineString *myLine = (OGRLineString *) myGeom->GetGeometryRef();
        wxLogMessage("line returned start here : %.4f | %.4f and contain %d vertex", myLine->getX(0), myLine->getY(0),
                     myLine->getNumPoints());

        OGRFeature::DestroyFeature(myGeom);

    }


    void testGettingGeometryType()
    {
        vrLayerVectorOGR myLayer;

        // layer not opened
        TS_ASSERT(myLayer.GetGeometryType() == wkbUnknown);


        TS_ASSERT_EQUALS(myLayer.Open(wxFileName(g_TestPath, g_TestFileSHP2), false), true);
        TS_ASSERT_EQUALS(myLayer.IsOK(), true);
        TS_ASSERT_EQUALS(myLayer.GetGeometryType(), wkbLineString);


        TS_ASSERT_EQUALS(myLayer.Open(wxFileName(g_TestPath, g_TestFileSHP), false), true);
        TS_ASSERT_EQUALS(myLayer.GetGeometryType(), wkbPolygon);
    }


    void testGettingNextGeometry()
    {
        wxLogMessage("Testing getting next data for layer");

        vrLayerVectorOGR myLayer;
        TS_ASSERT_EQUALS(myLayer.Open(wxFileName(g_TestPath, g_TestFileSHP2), false), true);
        TS_ASSERT_EQUALS(myLayer.IsOK(), true);

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
        TS_ASSERT_EQUALS(iCount, 45);


    }


};


#endif
