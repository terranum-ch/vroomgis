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

#ifndef _TEST_VR_RENDER_VECTOR_DIPS_H_
#define _TEST_VR_RENDER_VECTOR_DIPS_H_

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif


//#include "test_param.h"	//for test parameters
#include "vrrendervectorc2p.h"



class TEST_vrRenderVectorC2PDips : public CxxTest::TestSuite
{
private:
	
public:
	void testRenderBasic()
	{
		vrRenderVectorC2PDips myRenderDips;
		TS_ASSERT_EQUALS(myRenderDips.GetType(), vrRENDER_VECTOR_C2P_DIPS);
	}
	
	void testRenderDefaultColour(){
		vrRenderVectorC2PDips myRenderDips(*wxRED, 2);
		TS_ASSERT(myRenderDips.GetDipColour(10) == *wxRED);
		TS_ASSERT(myRenderDips.AddDipColour(*wxBLUE, 10));
		TS_ASSERT(myRenderDips.GetDipColour(10) == *wxBLUE);
		TS_ASSERT(myRenderDips.GetDipColour(10) == *wxBLUE);
	}
	
};







#endif
