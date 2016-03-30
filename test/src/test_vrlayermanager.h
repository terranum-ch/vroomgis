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

#ifndef _TEST_VR_LAYERMANAGER_H_
#define _TEST_VR_LAYERMANAGER_H_


#include "wx/wxprec.h"

#ifndef WX_PRECOMP

#include <wx/wx.h>

#endif


#include "test_param.h"	//for test parameters
#include "vrlayermanager.h"


class TEST_vrLayerManager
        : public CxxTest::TestSuite
{
private:
    wxFileName m_JpegName;
    wxFileName m_TiffName;

public:
    void setUp()
    {
        m_JpegName = wxFileName(g_TestPath, g_TestFileJPEG);
        m_TiffName = wxFileName(g_TestPath, g_TestFileTIF);
    }

    void testCreateLayerManager()
    {
        wxLogMessage("*************************");
        wxLogMessage("  VRLAYER MANAGER TESTS  ");
        wxLogMessage("*************************");


        vrLayerManager myLayerManager;
        TS_ASSERT(!myLayerManager.Open(wxFileName(g_TestPath, g_TestFileMisc)));
        TS_ASSERT(myLayerManager.Open(wxFileName(g_TestPath, g_TestFileSHP)));
        TS_ASSERT_EQUALS(myLayerManager.GetCount(), 1);
        // opening two times the same file
        TS_ASSERT(myLayerManager.Open(wxFileName(g_TestPath, g_TestFileSHP)));
        TS_ASSERT_EQUALS(myLayerManager.GetCount(), 1);
    }

    void testAddingRaster()
    {
        vrLayerManager myLayerManager;

        TS_ASSERT(myLayerManager.Open(m_JpegName));
        TS_ASSERT_EQUALS(myLayerManager.GetCount(), 1);


        TS_ASSERT(myLayerManager.Open(m_TiffName));
        TS_ASSERT_EQUALS(myLayerManager.GetCount(), 2);

        // opening two times the same file
        TS_ASSERT(myLayerManager.Open(m_JpegName));
        TS_ASSERT_EQUALS(myLayerManager.GetCount(), 2);
        TS_ASSERT(myLayerManager.Open(m_TiffName));
        TS_ASSERT_EQUALS(myLayerManager.GetCount(), 2);


    }

    void testGetType()
    {
        vrLayerManager myLayerManager;
        TS_ASSERT(myLayerManager.Open(m_JpegName));
        TS_ASSERT(myLayerManager.Open(m_TiffName));
        TS_ASSERT_EQUALS(myLayerManager.GetCount(), 2);

        // testing type
        TS_ASSERT_EQUALS(myLayerManager.GetLayer(m_JpegName)->GetType(), vrDRIVER_RASTER_JPEG);
        TS_ASSERT_EQUALS(myLayerManager.GetLayer(m_TiffName)->GetType(), vrDRIVER_RASTER_TIFF);
    }

    void testRemoveLayer1()
    {
        vrLayerManager myLayerManager;
        TS_ASSERT(myLayerManager.Open(m_JpegName));
        TS_ASSERT(myLayerManager.Open(m_TiffName));
        TS_ASSERT_EQUALS(myLayerManager.GetCount(), 2);

        // removing layer
        vrLayer *myLayer = myLayerManager.GetLayer(m_JpegName);
        TS_ASSERT(myLayer != NULL);

        TS_ASSERT(myLayerManager.Close(myLayer));

        // myLayer isn't valid anymore dont test again !
        //TS_ASSERT(myLayerManager.Close(myLayer)==false);
    }


};


#endif
