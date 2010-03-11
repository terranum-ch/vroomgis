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
	
	void testComputePixelSize(){
		FakevrViewerDisplay myNewDisplay;
		vrCoordinate myCoord (&myNewDisplay);
		
		TS_ASSERT(myCoord.GetPixelSize() == wxNOT_FOUND);
	}
    	


};







#endif
