/***************************************************************************
								vrlayermanager.h
				Manage the layers. Keep a list of all opened layers
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

#ifndef _VR_LAYERMANAGER_TEST_H_
#define _VR_LAYERMANAGER_TEST_H_


#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif


#include "vrlayermanager.h"


class vrLayerManagerTEST : public CxxTest::TestSuite
{
public:
    void testDifferString()
    {
		wxString myTest1 = "coucou";
		wxString myTest2 = "salut";
		
        TS_ASSERT_DIFFERS(myTest1, myTest2);
    }
	
	void testCreateLayerManager()
	{
		vrLayerManager myLayerManager;
		TS_ASSERT_EQUALS(myLayerManager.Open(wxFileName("/Users/lucien/DATA/PRJ/COLTOP-GIS/trunk_vroomgis/test/src/SimpleTest.h")), true);
	}
	
};






#endif
