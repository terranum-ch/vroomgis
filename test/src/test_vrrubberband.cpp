/***************************************************************************
 test_vrrubberband.h
 Testing the rubber band (only the computation part)
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

#include "test_param.h"
#include "vrrubberband.h"


TEST(RubberBand, GetRect1)
{
    wxPoint p1(10, 10);
    wxPoint p2(50, 50);

    vrRubberBand myRubber(NULL);
    myRubber.SetPointFirst(p1);
    myRubber.SetPointLast(p2);
    wxRect myRect = myRubber.GetRect();
    EXPECT_TRUE(myRect.GetTopLeft() == p1);
    EXPECT_TRUE(myRect.GetBottomRight() == p2);
}

TEST(RubberBand, GetRectInvert)
{
    wxPoint p2(10, 10);
    wxPoint p1(50, 50);

    vrRubberBand myRubber(NULL);
    myRubber.SetPointFirst(p1);
    myRubber.SetPointLast(p2);
    wxRect myRect = myRubber.GetRect();
    EXPECT_TRUE(myRect.GetTopLeft() == p2);
    EXPECT_TRUE(myRect.GetBottomRight() == p1);
}

TEST(RubberBand, IsPositive)
{
    wxPoint p1(10, 10);
    wxPoint p2(25, 25);
    wxPoint p3(50, 50);

    vrRubberBand myRubber(NULL);
    myRubber.SetPointFirst(p2);
    myRubber.SetPointLast(p3);
    EXPECT_TRUE(myRubber.IsPositive());
    myRubber.SetPointLast(p1);
    EXPECT_FALSE(myRubber.IsPositive());
    EXPECT_FALSE(myRubber.IsPositive());
    myRubber.SetPointLast(p2);
    EXPECT_TRUE(myRubber.IsPositive());
}


TEST(RubberBand, ZeroRect)
{
    wxPoint p1(10, 10);

    vrRubberBand myRubber(NULL);
    myRubber.SetPointFirst(p1);
    myRubber.SetPointLast(p1);
    EXPECT_TRUE(myRubber.IsPositive());

    wxRect myRect = myRubber.GetRect();
    EXPECT_TRUE(myRect.GetLeft() == myRect.GetRight());
    EXPECT_TRUE(myRect.GetTop() == myRect.GetBottom());
    EXPECT_TRUE(myRect.GetSize() == wxSize(1, 1));
}

TEST(RubberBand, ValidRubber)
{
    wxPoint p1(10, 10);
    wxPoint p2(20, 10);
    wxPoint p3(20, 20);

    vrRubberBand myRubber(NULL);
    myRubber.SetPointFirst(p1);
    EXPECT_FALSE(myRubber.IsValid());// not fully inited

    myRubber.SetPointLast(p1);
    EXPECT_FALSE(myRubber.IsValid()); // two same points

    myRubber.SetPointLast(p2);
    EXPECT_FALSE(myRubber.IsValid()); // no height

    myRubber.SetPointLast(p3);
    EXPECT_TRUE(myRubber.IsValid()); // valid
}
