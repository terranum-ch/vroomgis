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

#ifndef _TEST_VR_LAYERVECTORC2D_H_
#define _TEST_VR_LAYERVECTORC2D_H_


#include "wx/wxprec.h"

#ifndef WX_PRECOMP

#include <wx/wx.h>

#endif


#include "vrlayer.h"
#include "vrlayerraster.h"
#include "vrlayervector.h"
#include "test_param.h"	//for test parameters
#include "vrlayerrasterc2d.h"
#include "vrlayermanager.h"


class TEST_vrLayerRasterC2D : public CxxTest::TestSuite
{
    friend class vrLayerRasterC2D;

public:
    void testOpenLayerRasterC2D()
    {
        //init lib.
        vrLayerManager myManager;

        vrLayerRasterGDAL myLayer;
        TS_ASSERT(!myLayer.IsOK());
        TS_ASSERT_EQUALS(myLayer.Open(wxFileName(g_TestPath, g_TestFileC2D), true), true);
        TS_ASSERT_EQUALS(myLayer.GetType(), vrDRIVER_RASTER_C2D);
        TS_ASSERT(myLayer.IsOK());
    }


    void testGetPixelValueC2D()
    {
        wxLogMessage("Testing getting pixel value for C2D raster");
        vrRealRect myExtent;
        TS_ASSERT(myExtent.IsEmpty());

        // extent failed, layer not opened
        vrLayerRasterGDAL myLayer;
        TS_ASSERT(!myLayer.GetExtent(myExtent));

        // extent ok for layer GDAL
        TS_ASSERT_EQUALS(myLayer.Open(wxFileName(g_TestPath, g_TestFileC2D), false), true);
        TS_ASSERT_EQUALS(myLayer.IsOK(), true);
        wxArrayDouble myValues;
        TS_ASSERT(myLayer.GetPixelValue(598100, 115000, myValues));
        TS_ASSERT(myValues.GetCount() == 3);

        wxString myTxtValues = "Value of px 598100 - 115000 = ";
        for (unsigned int i = 0; i < myValues.GetCount(); i++) {
            myTxtValues.Append(wxString::Format(" %.2f -", myValues.Item(i)));
        }
        myTxtValues.RemoveLast(1);
        wxLogMessage(myTxtValues);

    }


};


#endif
