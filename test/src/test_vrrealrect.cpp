/***************************************************************************
test_vrcoordinate.h
Testing the coordinate system
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

#include "test_param.h"	//for test parameters
#include "vrrealrect.h"


TEST(RealRect, CreateRealRect)
{
    vrRealRect myRect;
    EXPECT_TRUE(!myRect.IsOk());
}


TEST(RealRect, Intersect1)
{
    vrRealRect myRectWnd(0, 1000, 4000, -1000);
    vrRealRect myRectImg(1000, 800, 2000, -600);

    EXPECT_TRUE(myRectWnd.IsOk());
    EXPECT_TRUE(myRectImg.IsOk());

    vrRealRect myIntersect = myRectWnd.Intersect(myRectImg);
    wxLogMessage("Intersection computed : %.2f / %.2f / %.2f / %.2f", myIntersect.GetLeft(), myIntersect.GetTop(),
                 myIntersect.GetRight(), myIntersect.GetBottom());

    EXPECT_TRUE(myIntersect == myRectImg);
}

TEST(RealRect, Intersect2)
{
    vrRealRect myRectWnd(0, 1000, 4000, -1000);
    vrRealRect myRectImg(2000, 2000, 3000, -1500);

    vrRealRect myResult(2000, 1000, 2000, -500);

    EXPECT_TRUE(myRectWnd.IsOk());
    EXPECT_TRUE(myRectImg.IsOk());

    vrRealRect myIntersect = myRectWnd.Intersect(myRectImg);
    wxLogMessage("Intersection computed : %.2f / %.2f / %.2f / %.2f", myIntersect.GetLeft(), myIntersect.GetTop(),
                 myIntersect.GetRight(), myIntersect.GetBottom());

    EXPECT_TRUE(myIntersect == myResult);

}


TEST(RealRect, Intersect3)
{
    vrRealRect myRectWnd(1000, 1000, 4000, -1000);
    vrRealRect myRectImg(0, 500, 2000, -1500);

    vrRealRect myResult(1000, 500, 1000, -500);

    EXPECT_TRUE(myRectWnd.IsOk());
    EXPECT_TRUE(myRectImg.IsOk());

    vrRealRect myIntersect = myRectWnd.Intersect(myRectImg);
    wxLogMessage("Intersection computed : %.2f / %.2f / %.2f / %.2f", myIntersect.GetLeft(), myIntersect.GetTop(),
                 myIntersect.GetRight(), myIntersect.GetBottom());

    EXPECT_TRUE(myIntersect == myResult);

}


TEST(RealRect, Equality)
{
    vrRealRect myRect(10, 10, 10, 10);
    vrRealRect myRect2;
    EXPECT_TRUE(!myRect2.IsOk());

    myRect2 = myRect;
    EXPECT_TRUE(myRect2.IsOk());
    EXPECT_TRUE(myRect2 == myRect);

    vrRealRect myRect3 = myRect;
    EXPECT_TRUE(myRect3.IsOk());
    EXPECT_TRUE(myRect3 == myRect2);
}


TEST(RealRect, OutsideIntersectRight)
{
    vrRealRect myRectWnd(0, 10, 10, -10);
    vrRealRect myRectImg(11, 10, 10, -10);

    vrRealRect myResult = myRectWnd.Intersect(myRectImg);
    EXPECT_TRUE(!myResult.IsOk());
}

TEST(RealRect, OutsideIntersectTop)
{
    vrRealRect myRectWnd(0, 10, 10, -10);
    vrRealRect myRectImg(5, 20, 20, -9);

    vrRealRect myResult = myRectWnd.Intersect(myRectImg);
    EXPECT_TRUE(!myResult.IsOk());
}

TEST(RealRect, OutsideIntersectLeft)
{
    vrRealRect myRectWnd(10, 10, 10, -10);
    vrRealRect myRectImg(0, 10, 5, -10);

    vrRealRect myResult = myRectWnd.Intersect(myRectImg);
    EXPECT_TRUE(!myResult.IsOk());
}

TEST(RealRect, OutsideIntersectBottom)
{
    vrRealRect myRectWnd(0, 20, 20, -10);
    vrRealRect myRectImg(0, 9, 9, -9);

    vrRealRect myResult = myRectWnd.Intersect(myRectImg);
    EXPECT_TRUE(!myResult.IsOk());
}
