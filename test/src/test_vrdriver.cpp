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

#include <gtest/gtest.h>

#include "vrdriver.h"

class Drivers : public ::testing::Test {
  protected:
    vrDrivers m_Driver;
};

TEST_F(Drivers, DriverGetWildcards) {
    EXPECT_FALSE(m_Driver.GetWildcards().IsEmpty());
    wxLogDebug(m_Driver.GetWildcards());
}

TEST_F(Drivers, DriverIsSupported) {
    EXPECT_FALSE(m_Driver.IsSupported(wxEmptyString));
    EXPECT_TRUE(m_Driver.IsSupported("shp"));
    EXPECT_FALSE(m_Driver.IsSupported("sh"));
    EXPECT_FALSE(m_Driver.IsSupported("shpp"));
}

TEST_F(Drivers, DriverGetType) {
    EXPECT_EQ(m_Driver.GetType("shp"), vrDRIVER_VECTOR_SHP);
    EXPECT_EQ(m_Driver.GetType("tiff"), vrDRIVER_RASTER_TIFF);
    EXPECT_EQ(m_Driver.GetType(""), vrDRIVER_UNKNOWN);
    EXPECT_EQ(m_Driver.GetType("jpg"), vrDRIVER_RASTER_JPEG);
    EXPECT_EQ(m_Driver.GetType("JPEG"), vrDRIVER_RASTER_JPEG);
    EXPECT_EQ(m_Driver.GetType("c2d"), vrDRIVER_RASTER_C2D);
    EXPECT_EQ(m_Driver.GetType("adf"), vrDRIVER_RASTER_ESRIGRID);
}
