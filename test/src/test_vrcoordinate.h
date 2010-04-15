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
#include "../../src/vroomgis.h"
#include "../../src/vrcoordinate.h"
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


};







#endif
