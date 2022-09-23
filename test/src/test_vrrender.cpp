/***************************************************************************
								test_vrrender.h
							Rendering main class for data
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

#include "gtest/gtest.h"

#include "vrrender.h"
#include "vrrendercoltop.h"


TEST(Render, RenderBasic)
{
    vrRender myRender;
    vrRenderVector myRenderVect;
    vrRenderRaster myRenderRast;
    vrRenderRasterColtop myRenderRastc2d;
    EXPECT_EQ(myRender.GetType(), vrRENDER_UNKNOWN);
    EXPECT_EQ(myRenderVect.GetType(), vrRENDER_VECTOR);
    EXPECT_EQ(myRenderRast.GetType(), vrRENDER_RASTER);
    EXPECT_EQ(myRenderRastc2d.GetType(), vrRENDER_RASTER_C2D);

}

TEST(Render, RenderPointer)
{
    vrRender *mypRender = NULL;
    vrRenderVector myRenderVect;
    vrRenderRaster myRenderRast;
    vrRenderRasterColtop myRenderRastc2d;

    mypRender = &myRenderVect;
    EXPECT_EQ(mypRender->GetType(), vrRENDER_VECTOR);

    mypRender = &myRenderRast;
    EXPECT_EQ(mypRender->GetType(), vrRENDER_RASTER);

    mypRender = &myRenderRastc2d;
    EXPECT_EQ(mypRender->GetType(), vrRENDER_RASTER_C2D);

}

TEST(Render, RenderGetTransparencyChar) {

#ifndef __WXMSW__
    vrRender *mypRender = NULL;
    vrRenderVector myRenderVect;

    mypRender = &myRenderVect;

    mypRender->SetTransparency(0); // opaque
    EXPECT_EQ(mypRender->GetTransparency(), 0);
    EXPECT_EQ(mypRender->GetTransparencyChar(), 255);

    mypRender->SetTransparency(100); // transparent
    EXPECT_EQ(mypRender->GetTransparency(), 100);
    EXPECT_EQ(mypRender->GetTransparencyChar(), 0);

    mypRender->SetTransparency(50); // medium tranparency
    EXPECT_EQ(mypRender->GetTransparency(), 50);
    EXPECT_EQ(mypRender->GetTransparencyChar(), 128);
#endif
}