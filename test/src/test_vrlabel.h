/***************************************************************************
								test_vrlabel.h
							Labelling class for data
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

#ifndef _TEST_VR_LABEL_H_
#define _TEST_VR_LABEL_H_

#include <cxxtest/TestSuite.h>
#include "wx/wxprec.h"

#ifndef WX_PRECOMP

#include <wx/wx.h>

#endif

#include "vrlabel.h"


class TEST_vrLabel : public CxxTest::TestSuite
{
public:
    wxFont m_Fontt;

    void setUp()
    {
        m_Fontt = wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    }

    void testLabelCreate()
    {
        // using *wxNORMAL_FONT crash !!!
        vrLabel myLabel;

        // getting font is strange...
        // lot of mac alert ???, to be tested on windows
#ifndef __WXMAC__
        TS_ASSERT_EQUALS(myLabel.GetFont(), m_Fontt);
#endif
        TS_ASSERT_EQUALS(myLabel.GetColor(), *wxBLACK);
    }

    void testLabelCreate2()
    {
        wxFont myFont2 = m_Fontt;
        myFont2.SetUnderlined(true);

        vrLabel myLabel(myFont2, wxColour(255, 0, 0));
        TS_ASSERT_EQUALS(myLabel.GetColor(), *wxRED);
#ifndef __WXMAC__
        TS_ASSERT(myLabel.GetFont() != m_Fontt);
        TS_ASSERT_EQUALS(myLabel.GetFont(), myFont2);
#endif
    }

    void testSerializeLabel()
    {

        // save label
        vrLabel myLabel;
        vrSerialize mySerialize;
        TS_ASSERT(myLabel.Serialize(mySerialize));
        TS_ASSERT(mySerialize.GetString() != wxEmptyString);
        wxLogMessage(mySerialize.GetString());

        //change label colour
        wxColour myColour(255, 0, 0);
        myLabel.SetColor(myColour);


        // deserialize
        vrSerialize mySerialize2(mySerialize.GetString());
        TS_ASSERT(myLabel.Serialize(mySerialize2));
        TS_ASSERT(myLabel.GetColor() != myColour);
    }


};


#endif
