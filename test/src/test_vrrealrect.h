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

#ifndef _TEST_VRREALRECT_H_
#define _TEST_VRREALRECT_H_


#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "test_param.h"	//for test parameters
#include "../../src/vroomgis.h"
#include "../../src/vrrealrect.h"




class TEST_vrRealRect : public CxxTest::TestSuite
{
public:
	void testShowTestName(){
		wxLogMessage("******************************");
		wxLogMessage(" TESTING vrRealRect CLASS     ");
		wxLogMessage("******************************");
	}
	
	void testCreateRealRect(){
		vrRealRect myRect;
		TS_ASSERT(myRect.IsOk() ==false);
	}
	
	
	
	
	void testIntersect1(){
		vrRealRect myRectWnd (0, 1000, 4000, -1000);
		vrRealRect myRectImg (1000, 800, 2000, -600);
		
		TS_ASSERT(myRectWnd.IsOk() == true);
		TS_ASSERT(myRectImg.IsOk() == true);
		
		vrRealRect myIntersect = myRectWnd.Intersect(myRectImg);
		wxLogMessage("Intersection computed : %.2f / %.2f / %.2f / %.2f",
					 myIntersect.GetLeft(),
					 myIntersect.GetTop(),
					 myIntersect.GetRight(),
					 myIntersect.GetBottom());
		
		TS_ASSERT(myIntersect == myRectImg);
	}
	
	void testIntersect2(){
		vrRealRect myRectWnd (0, 1000, 4000, -1000);
		vrRealRect myRectImg (2000, 2000, 3000, -1500);
		
		vrRealRect myResult (2000, 1000 ,2000, -500);
		
		TS_ASSERT(myRectWnd.IsOk() == true);
		TS_ASSERT(myRectImg.IsOk() == true);
		
		vrRealRect myIntersect = myRectWnd.Intersect(myRectImg);
		wxLogMessage("Intersection computed : %.2f / %.2f / %.2f / %.2f",
					 myIntersect.GetLeft(),
					 myIntersect.GetTop(),
					 myIntersect.GetRight(),
					 myIntersect.GetBottom());
		
		TS_ASSERT(myIntersect == myResult);
		
	}
	
	
	void testIntersect3(){
		vrRealRect myRectWnd (1000, 1000, 4000, -1000);
		vrRealRect myRectImg (0, 500, 2000, -1500);
		
		vrRealRect myResult (1000, 500 ,1000, -500);
		
		TS_ASSERT(myRectWnd.IsOk() == true);
		TS_ASSERT(myRectImg.IsOk() == true);
		
		vrRealRect myIntersect = myRectWnd.Intersect(myRectImg);
		wxLogMessage("Intersection computed : %.2f / %.2f / %.2f / %.2f",
					 myIntersect.GetLeft(),
					 myIntersect.GetTop(),
					 myIntersect.GetRight(),
					 myIntersect.GetBottom());
		
		TS_ASSERT(myIntersect == myResult);
		
	}
	
	
	void testEquality(){
		vrRealRect myRect(10,10,10,10);
		vrRealRect myRect2;
		TS_ASSERT(myRect2.IsOk() == false);
		
		myRect2 = myRect;
		TS_ASSERT(myRect2.IsOk() == true);
		TS_ASSERT(myRect2 == myRect);
		
		vrRealRect myRect3 = myRect;
		TS_ASSERT(myRect3.IsOk());
		TS_ASSERT(myRect3 == myRect2);
	}

};







#endif
