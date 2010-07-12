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

#ifndef _TEST_VRCOORDINATE_H_
#define _TEST_VRCOORDINATE_H_


#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif


#include "test_param.h"	//for test parameters
#include "vroomgis.h"
#include "vrcoordinate.h"
#include "fake_vrviewerdisplay.h"
#include "fake_vrviewerdisplay.cpp"



class TEST_vrCoordinate : public CxxTest::TestSuite
{
public:
	void testCreateCoordinate(){

		FakevrViewerDisplay myDisplay;
		TS_ASSERT(myDisplay.GetSize() == wxSize(600, 400));
		vrCoordinate myCoord (&myDisplay);

		wxLogMessage("Initial coordinate extent is %.2f %.2f, %.2f, %.2f",
					 myCoord.GetExtent().GetLeft(),
					 myCoord.GetExtent().GetTop(),
					 myCoord.GetExtent().GetRight(),
					 myCoord.GetExtent().GetBottom());
		TS_ASSERT_DELTA(myCoord.GetExtent().GetLeft(), 0, 0.000001);
		TS_ASSERT_DELTA(myCoord.GetExtent().GetTop(), 0, 0.000001);
		TS_ASSERT_DELTA(myCoord.GetExtent().GetRight(), 0, 0.000001);
		TS_ASSERT_DELTA(myCoord.GetExtent().GetBottom(), 0, 0.000001);
	}



	void testComputePixelSizeFailed(){
		FakevrViewerDisplay myNewDisplay;
		vrCoordinate myCoord (&myNewDisplay);

		TS_ASSERT(myCoord.GetPixelSize() == wxNOT_FOUND);
	}



	void testComputeGetExtent(){
		FakevrViewerDisplay myNewDisplay;
		vrCoordinate myCoord (&myNewDisplay);

		// computing full extent without adding extent return false
		TS_ASSERT(myCoord.ComputeFullExtent()==false);


		// creating false extent
		vrRealRect myTestExtent (598000, 116000, 1000, -2000);
		myCoord.AddLayersExtent(myTestExtent);
		TS_ASSERT(myCoord.ComputeFullExtent()==true);

		wxLogMessage("Extent is %.2f %.2f, %.2f, %.2f",
					 myCoord.GetExtent().GetLeft(),
					 myCoord.GetExtent().GetTop(),
					 myCoord.GetExtent().GetRight(),
					 myCoord.GetExtent().GetBottom());

	}



	void testExtentAndPixSize(){
		FakevrViewerDisplay myNewDisplay (wxSize(610, 410)); // to take into accout the margin
		vrCoordinate myCoord (&myNewDisplay);

		vrRealRect myTestExtent (0, 4000, 6000, -4000);
		myCoord.AddLayersExtent(myTestExtent);
		TS_ASSERT(myCoord.ComputeFullExtent()==true);
		TS_ASSERT_DELTA(myCoord.GetExtent().GetLeft(), -50, 0.0001);
		TS_ASSERT_DELTA(myCoord.GetExtent().GetTop(), 4050, 0.0001);
		TS_ASSERT_DELTA(myCoord.GetExtent().GetRight(), 6050, 0.0001);
		TS_ASSERT_DELTA(myCoord.GetExtent().GetBottom(), -50, 0.0001);


		TS_ASSERT_DELTA(myCoord.GetPixelSize(), 10, 0.000001);
	}


	void testUpdateExtent(){
		FakevrViewerDisplay myNewDisplay (wxSize(610, 410));
		vrCoordinate myCoord (&myNewDisplay);

		TS_ASSERT(myCoord.UpdateExtent() == false);

		vrRealRect myTestExtent (0, 4000, 6000, -4000);
		myCoord.AddLayersExtent(myTestExtent);
		TS_ASSERT(myCoord.ComputeFullExtent()==true);
		TS_ASSERT_DELTA(myCoord.GetPixelSize(), 10, 0.00000001);


		// this is the real test, before was just initialisation
		TS_ASSERT(myCoord.UpdateExtent() == true);
		TS_ASSERT_DELTA(myCoord.GetPixelSize(), 10,0.00000001);


		// viewer size increase but px size doesn't change
		myNewDisplay.SetFakeSize(wxSize(1220, 820));
		TS_ASSERT(myCoord.UpdateExtent() == true);
		TS_ASSERT_DELTA(myCoord.GetPixelSize(), 10,0.00000001);
		wxLogMessage("---- pixel size is %.f", myCoord.GetPixelSize());


		// left and top doesn't change too
		TS_ASSERT_DELTA(myCoord.GetExtent().GetLeft(), -50, 0.0001);
		TS_ASSERT_DELTA(myCoord.GetExtent().GetTop(), 4050, 0.0001);

		TS_ASSERT_DELTA(myCoord.GetExtent().GetRight(), 12150, 0.0001);
		TS_ASSERT_DELTA(myCoord.GetExtent().GetBottom(), -4150, 0.0001);




	}

	void testIsOk(){
		FakevrViewerDisplay myNewDisplay (wxSize(610, 410));
		vrCoordinate myCoord (&myNewDisplay);

		TS_ASSERT(myCoord.IsOk() == false);

		vrRealRect myTestExtent (0, 4000, 6000, -4000);
		myCoord.AddLayersExtent(myTestExtent);
		TS_ASSERT(myCoord.ComputeFullExtent()==true);
		TS_ASSERT_DELTA(myCoord.GetPixelSize(), 10, 0.00000001);

		TS_ASSERT(myCoord.IsOk() == true);
	}

	
	void testGetFittedRectangle(){
		FakevrViewerDisplay myDisplay;
		TS_ASSERT(myDisplay.GetSize() == wxSize(600, 400));
		vrCoordinate myCoord (&myDisplay);
		
		vrRealRect myExtent (0,2000,1000,-1000);
		myCoord.SetExtent(myExtent);
		
		// not valid rectangle
		vrRealRect mySelRect1 (0,10,0,0);
		TS_ASSERT(myCoord.GetRectFitted(mySelRect1).IsOk() == false);
		
		// perfect ratio (smaller)
		vrRealRect mySelRect (0,200,100,-100);
		TS_ASSERT(myCoord.GetRectFitted(mySelRect) == mySelRect);
		
		// perfect ratio (bigger)
		vrRealRect mySelRect2 (0,20000,10000,-10000);
		TS_ASSERT(myCoord.GetRectFitted(mySelRect2) == mySelRect2);
		
		// smaller 1/2
		vrRealRect mySelRect3 (0,200,10,-100);
		vrRealRect myResult3 (0,155,10,-10);
		TS_ASSERT(myCoord.GetRectFitted(mySelRect3) == myResult3);
	}
	
	void testGetPixelSize(){
		FakevrViewerDisplay myDisplay (wxSize(200,100));
		TS_ASSERT(myDisplay.GetSize() == wxSize(200, 100));
		vrCoordinate myCoord (&myDisplay);
		
		vrRealRect myExtent (0,2000,1000,-1000);
		myCoord.SetExtent(myExtent);
		double myPx = myCoord.GetPixelSize();
		TS_ASSERT_DELTA(myPx, 5, 0.000001);
	
		vrRealRect myExtent2 (0,200,100,-100);
		myCoord.SetExtent(myExtent2);
		double myPx2 = myCoord.GetPixelSize();
		TS_ASSERT_DELTA(myPx2, 0.5, 0.000001);
		
		
	}
	
	
	void testConvertFromPixels(){
		// init display
		FakevrViewerDisplay myDisplay (wxSize(200,100));
		TS_ASSERT(myDisplay.GetSize() == wxSize(200, 100));
		vrCoordinate myCoord (&myDisplay);
		vrRealRect myExtent (0,2000,1000,-1000);
		myCoord.SetExtent(myExtent);
		double myPx = myCoord.GetPixelSize();
		TS_ASSERT_DELTA(myPx, 5, 0.000001);
		
		// test
		wxPoint2DDouble myConvertedPt;
		TS_ASSERT(myCoord.ConvertFromPixels(wxPoint(100,50),myConvertedPt)==true);
		TS_ASSERT(myConvertedPt == wxPoint2DDouble(500,1750));
		
		TS_ASSERT(myCoord.ConvertFromPixels(wxPoint(10,0),myConvertedPt)==true);
		TS_ASSERT(myConvertedPt == wxPoint2DDouble(50,2000));

		TS_ASSERT(myCoord.ConvertFromPixels(wxPoint(0,0),myConvertedPt)==true);
		TS_ASSERT(myConvertedPt == wxPoint2DDouble(0,2000));

	}
	
	void testConvertFromPixels2(){
		// init display
		FakevrViewerDisplay myDisplay (wxSize(200,100));
		TS_ASSERT(myDisplay.GetSize() == wxSize(200, 100));
		vrCoordinate myCoord (&myDisplay);
		vrRealRect myExtent (0,2000,1000,-1000);
		myCoord.SetExtent(myExtent);
		double myPx = myCoord.GetPixelSize();
		TS_ASSERT_DELTA(myPx, 5, 0.000001);
		
		// test
		vrRealRect myConvertedRect;
		TS_ASSERT(myCoord.ConvertFromPixels(wxRect(0,0, 50, 100),myConvertedRect)==true);
		TS_ASSERT(myConvertedRect == vrRealRect(0,2000, 245,-495));
		
	}
	

};







#endif
