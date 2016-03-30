/***************************************************************************
								test_vrdriver.h
							Testing vrDrivers class
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

#ifndef _TEST_VR_DRIVER_H_
#define _TEST_VR_DRIVER_H_


#include "wx/wxprec.h"

#ifndef WX_PRECOMP

#include <wx/wx.h>

#endif


//#include "test_param.h"	//for test parameters
#include "vrdriver.h"


class TEST_vrDrivers
        : public CxxTest::TestSuite
{
private:
    vrDrivers m_Driver;
public:

    void testDriverGetWildcards()
    {

        TS_ASSERT(m_Driver.GetWildcards().IsEmpty() == false);
        wxLogDebug(m_Driver.GetWildcards());
    }

    void testDriverIsSupported()
    {
        TS_ASSERT(m_Driver.IsSupported(wxEmptyString) == false);
        TS_ASSERT(m_Driver.IsSupported("shp") == true);
        TS_ASSERT(m_Driver.IsSupported("sh") == false);
        TS_ASSERT_EQUALS(m_Driver.IsSupported("shpp"), false);
    }

    void testDriverGetType()
    {
        TS_ASSERT_EQUALS(m_Driver.GetType("shp"), vrDRIVER_VECTOR_SHP);
        TS_ASSERT_EQUALS(m_Driver.GetType("tiff"), vrDRIVER_RASTER_TIFF);
        TS_ASSERT_EQUALS(m_Driver.GetType(""), vrDRIVER_UNKNOWN);
        TS_ASSERT_EQUALS(m_Driver.GetType("jpg"), vrDRIVER_RASTER_JPEG);
        TS_ASSERT_EQUALS(m_Driver.GetType("JPEG"), vrDRIVER_RASTER_JPEG);
        TS_ASSERT_EQUALS(m_Driver.GetType("c2d"), vrDRIVER_RASTER_C2D);
        TS_ASSERT_EQUALS(m_Driver.GetType("adf"), vrDRIVER_RASTER_ESRIGRID);
    }


};


#endif
