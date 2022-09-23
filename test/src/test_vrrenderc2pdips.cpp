/***************************************************************************
 TEST_vrRenderVectorC2PDipsc2pdips.h
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

#include "wx/wxprec.h"

#include "vrrendervectorc2p.h"


TEST(RenderVectorC2PDips, RenderBasic)
{
    vrRenderVectorC2PDips myRenderDips;
    EXPECT_EQ(myRenderDips.GetType(), vrRENDER_VECTOR_C2P_DIPS);
}

TEST(RenderVectorC2PDips, RenderDefaultColour)
{
    vrRenderVectorC2PDips myRenderDips(*wxRED, 2);
    EXPECT_TRUE(myRenderDips.GetDipColour(10) == *wxRED);
    EXPECT_TRUE(myRenderDips.AddDipColour(*wxBLUE, 10, true));
    EXPECT_TRUE(myRenderDips.GetDipColour(10) == *wxBLUE);
    EXPECT_TRUE(myRenderDips.GetDipColour(10) == *wxBLUE);
}
