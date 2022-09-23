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

#include "test_param.h"
#include "vroomgis.h"
#include "vrcoordinate.h"
#include "fake_vrviewerdisplay.h"
#include "fake_vrviewerdisplay.cpp"


TEST(Coordinates, CreateCoordinate)
{
    FakevrViewerDisplay myDisplay;
    EXPECT_TRUE(myDisplay.GetSize() == wxSize(600, 400));
    vrCoordinate myCoord(&myDisplay);

    wxLogMessage("Initial coordinate extent is %.2f %.2f, %.2f, %.2f", myCoord.GetExtent().GetLeft(),
                 myCoord.GetExtent().GetTop(), myCoord.GetExtent().GetRight(), myCoord.GetExtent().GetBottom());
    EXPECT_NEAR(myCoord.GetExtent().GetLeft(), 0, 0.000001);
    EXPECT_NEAR(myCoord.GetExtent().GetTop(), 0, 0.000001);
    EXPECT_NEAR(myCoord.GetExtent().GetRight(), 0, 0.000001);
    EXPECT_NEAR(myCoord.GetExtent().GetBottom(), 0, 0.000001);
}

TEST(Coordinates, ComputePixelSizeFailed)
{
    FakevrViewerDisplay myNewDisplay;
    vrCoordinate myCoord(&myNewDisplay);

    EXPECT_TRUE(myCoord.GetPixelSize() == wxNOT_FOUND);
}

TEST(Coordinates, ComputeGetExtent)
{
    FakevrViewerDisplay myNewDisplay;
    vrCoordinate myCoord(&myNewDisplay);

    // computing full extent without adding extent return false
    EXPECT_TRUE(!myCoord.ComputeFullExtent());


    // creating false extent
    vrRealRect myTestExtent(598000, 116000, 1000, -2000);
    myCoord.AddLayersExtent(myTestExtent);
    EXPECT_TRUE(myCoord.ComputeFullExtent());

    wxLogMessage("Extent is %.2f %.2f, %.2f, %.2f", myCoord.GetExtent().GetLeft(), myCoord.GetExtent().GetTop(),
                 myCoord.GetExtent().GetRight(), myCoord.GetExtent().GetBottom());

}

TEST(Coordinates, ExtentAndPixSize)
{
    FakevrViewerDisplay myNewDisplay(wxSize(610, 410)); // to take into accout the margin
    vrCoordinate myCoord(&myNewDisplay);

    vrRealRect myTestExtent(0, 4000, 6000, -4000);
    myCoord.AddLayersExtent(myTestExtent);
    EXPECT_TRUE(myCoord.ComputeFullExtent());
    EXPECT_NEAR(myCoord.GetExtent().GetLeft(), -50, 0.0001);
    EXPECT_NEAR(myCoord.GetExtent().GetTop(), 4050, 0.0001);
    EXPECT_NEAR(myCoord.GetExtent().GetRight(), 6050, 0.0001);
    EXPECT_NEAR(myCoord.GetExtent().GetBottom(), -50, 0.0001);


    EXPECT_NEAR(myCoord.GetPixelSize(), 10, 0.000001);
}

TEST(Coordinates, UpdateExtent)
{
    FakevrViewerDisplay myNewDisplay(wxSize(610, 410));
    vrCoordinate myCoord(&myNewDisplay);

    EXPECT_TRUE(!myCoord.UpdateExtent());

    vrRealRect myTestExtent(0, 4000, 6000, -4000);
    myCoord.AddLayersExtent(myTestExtent);
    EXPECT_TRUE(myCoord.ComputeFullExtent());
    EXPECT_NEAR(myCoord.GetPixelSize(), 10, 0.00000001);


    // this is the real test, before was just initialisation
    EXPECT_TRUE(myCoord.UpdateExtent());
    EXPECT_NEAR(myCoord.GetPixelSize(), 10, 0.00000001);


    // viewer size increase but px size doesn't change
    myNewDisplay.SetFakeSize(wxSize(1220, 820));
    EXPECT_TRUE(myCoord.UpdateExtent());
    EXPECT_NEAR(myCoord.GetPixelSize(), 10, 0.00000001);
    wxLogMessage("---- pixel size is %.f", myCoord.GetPixelSize());


    // left and top doesn't change too
    EXPECT_NEAR(myCoord.GetExtent().GetLeft(), -50, 0.0001);
    EXPECT_NEAR(myCoord.GetExtent().GetTop(), 4050, 0.0001);

    EXPECT_NEAR(myCoord.GetExtent().GetRight(), 12150, 0.0001);
    EXPECT_NEAR(myCoord.GetExtent().GetBottom(), -4150, 0.0001);


}

TEST(Coordinates, IsOk)
{
    FakevrViewerDisplay myNewDisplay(wxSize(610, 410));
    vrCoordinate myCoord(&myNewDisplay);

    EXPECT_TRUE(!myCoord.IsOk());

    vrRealRect myTestExtent(0, 4000, 6000, -4000);
    myCoord.AddLayersExtent(myTestExtent);
    EXPECT_TRUE(myCoord.ComputeFullExtent());
    EXPECT_NEAR(myCoord.GetPixelSize(), 10, 0.00000001);

    EXPECT_TRUE(myCoord.IsOk());
}

TEST(Coordinates, GetFittedRectangle)
{
    FakevrViewerDisplay myDisplay;
    EXPECT_TRUE(myDisplay.GetSize() == wxSize(600, 400));
    vrCoordinate myCoord(&myDisplay);

    vrRealRect myExtent(0, 2000, 1000, -1000);
    myCoord.SetExtent(myExtent);

    // not valid rectangle
    vrRealRect mySelRect1(0, 10, 0, 0);
    EXPECT_TRUE(!myCoord.GetRectFitted(mySelRect1).IsOk());

    // perfect ratio (smaller)
    vrRealRect mySelRect(0, 200, 100, -100);
    EXPECT_TRUE(myCoord.GetRectFitted(mySelRect) == mySelRect);

    // perfect ratio (bigger)
    vrRealRect mySelRect2(0, 20000, 10000, -10000);
    EXPECT_TRUE(myCoord.GetRectFitted(mySelRect2) == mySelRect2);

    // smaller 1/2
    vrRealRect mySelRect3(0, 200, 10, -100);
    vrRealRect myResult3(-45, 200, 100, -100);
    EXPECT_TRUE(myCoord.GetRectFitted(mySelRect3) == myResult3);
}

TEST(Coordinates, GetPixelSize)
{
    FakevrViewerDisplay myDisplay(wxSize(200, 100));
    EXPECT_TRUE(myDisplay.GetSize() == wxSize(200, 100));
    vrCoordinate myCoord(&myDisplay);

    vrRealRect myExtent(0, 2000, 1000, -1000);
    myCoord.SetExtent(myExtent);
    double myPx = myCoord.GetPixelSize();
    EXPECT_NEAR(myPx, 5, 0.000001);

    vrRealRect myExtent2(0, 200, 100, -100);
    myCoord.SetExtent(myExtent2);
    double myPx2 = myCoord.GetPixelSize();
    EXPECT_NEAR(myPx2, 0.5, 0.000001);


}

TEST(Coordinates, ConvertFromPixels)
{
    // init display
    FakevrViewerDisplay myDisplay(wxSize(200, 100));
    EXPECT_TRUE(myDisplay.GetSize() == wxSize(200, 100));
    vrCoordinate myCoord(&myDisplay);
    vrRealRect myExtent(0, 2000, 1000, -1000);
    myCoord.SetExtent(myExtent);
    double myPx = myCoord.GetPixelSize();
    EXPECT_NEAR(myPx, 5, 0.000001);

    // test
    wxPoint2DDouble myConvertedPt;
    EXPECT_TRUE(myCoord.ConvertFromPixels(wxPoint(100, 50), myConvertedPt));
    EXPECT_TRUE(myConvertedPt == wxPoint2DDouble(500, 1750));

    EXPECT_TRUE(myCoord.ConvertFromPixels(wxPoint(10, 0), myConvertedPt));
    EXPECT_TRUE(myConvertedPt == wxPoint2DDouble(50, 2000));

    EXPECT_TRUE(myCoord.ConvertFromPixels(wxPoint(0, 0), myConvertedPt));
    EXPECT_TRUE(myConvertedPt == wxPoint2DDouble(0, 2000));

}

TEST(Coordinates, ConvertFromPixels2)
{
    // init display
    FakevrViewerDisplay myDisplay(wxSize(200, 100));
    EXPECT_TRUE(myDisplay.GetSize() == wxSize(200, 100));
    vrCoordinate myCoord(&myDisplay);
    vrRealRect myExtent(0, 2000, 1000, -1000);
    myCoord.SetExtent(myExtent);
    double myPx = myCoord.GetPixelSize();
    EXPECT_NEAR(myPx, 5, 0.000001);

    // test
    vrRealRect myConvertedRect;
    EXPECT_TRUE(myCoord.ConvertFromPixels(wxRect(0, 0, 50, 100), myConvertedRect));
    EXPECT_TRUE(myConvertedRect == vrRealRect(0, 2000, 245, -495));

}
