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

#ifndef _TEST_VRRUBBERBAND_H_
#define _TEST_VRRUBBERBAND_H_


#include "wx/wxprec.h"

#ifndef WX_PRECOMP

#include <wx/wx.h>

#endif


#include "test_param.h"	//for test parameters
#include "vrrubberband.h"


class TEST_vrRubberBand
        : public CxxTest::TestSuite
{
public:
    void testShowTestName()
    {
        wxLogMessage("******************************");
        wxLogMessage(" TESTING vrRubberBand CLASS  ");
        wxLogMessage("******************************");
    }

    void testGetRect1()
    {
        wxPoint p1(10, 10);
        wxPoint p2(50, 50);

        vrRubberBand myRubber(NULL);
        myRubber.SetPointFirst(p1);
        myRubber.SetPointLast(p2);
        wxRect myRect = myRubber.GetRect();
        TS_ASSERT(myRect.GetTopLeft() == p1);
        TS_ASSERT(myRect.GetBottomRight() == p2);
    }

    void testGetRectInvert()
    {
        wxPoint p2(10, 10);
        wxPoint p1(50, 50);

        vrRubberBand myRubber(NULL);
        myRubber.SetPointFirst(p1);
        myRubber.SetPointLast(p2);
        wxRect myRect = myRubber.GetRect();
        TS_ASSERT(myRect.GetTopLeft() == p2);
        TS_ASSERT(myRect.GetBottomRight() == p1);
    }

    void testIsPositive()
    {
        wxPoint p1(10, 10);
        wxPoint p2(25, 25);
        wxPoint p3(50, 50);

        vrRubberBand myRubber(NULL);
        myRubber.SetPointFirst(p2);
        myRubber.SetPointLast(p3);
        TS_ASSERT(myRubber.IsPositive());
        myRubber.SetPointLast(p1);
        TS_ASSERT(!myRubber.IsPositive());
        TS_ASSERT(!myRubber.IsPositive());
        myRubber.SetPointLast(p2);
        TS_ASSERT(myRubber.IsPositive());
    }


    void testZeroRect()
    {
        wxPoint p1(10, 10);

        vrRubberBand myRubber(NULL);
        myRubber.SetPointFirst(p1);
        myRubber.SetPointLast(p1);
        TS_ASSERT(myRubber.IsPositive());

        wxRect myRect = myRubber.GetRect();
        TS_ASSERT(myRect.GetLeft() == myRect.GetRight());
        TS_ASSERT(myRect.GetTop() == myRect.GetBottom());
        TS_ASSERT(myRect.GetSize() == wxSize(1, 1));
    }

    void testValidRubber()
    {
        wxPoint p1(10, 10);
        wxPoint p2(20, 10);
        wxPoint p3(20, 20);

        vrRubberBand myRubber(NULL);
        myRubber.SetPointFirst(p1);
        TS_ASSERT(!myRubber.IsValid());// not fully inited

        myRubber.SetPointLast(p1);
        TS_ASSERT(!myRubber.IsValid()); // two same points

        myRubber.SetPointLast(p2);
        TS_ASSERT(!myRubber.IsValid()); // no height

        myRubber.SetPointLast(p3);
        TS_ASSERT(myRubber.IsValid()); // valid
    }


    void testRubberFinished()
    {
        wxLogMessage("TESTING RUBBER CLASS FINISHED");
    }


};


#endif
