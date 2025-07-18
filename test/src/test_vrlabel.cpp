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

#include <gtest/gtest.h>

#include "vrlabel.h"

class Label : public ::testing::Test {
  protected:
    wxFont m_Fontt;

    void SetUp() override {
        m_Fontt = wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    }
};

TEST_F(Label, LabelCreate) {
    // using *wxNORMAL_FONT crash !!!
    vrLabel myLabel;

    // getting font is strange...
    // lot of mac alert ???, to be tested on windows
//#ifndef __WXMAC__
    EXPECT_EQ(myLabel.GetFont(), m_Fontt);
//#endif
    EXPECT_EQ(myLabel.GetColor(), *wxBLACK);
}

TEST_F(Label, LabelCreate2) {
    wxFont myFont2 = m_Fontt;
    myFont2.SetUnderlined(true);

    vrLabel myLabel(myFont2, wxColour(255, 0, 0));
    EXPECT_EQ(myLabel.GetColor(), *wxRED);
#ifndef __WXMAC__
    EXPECT_TRUE(myLabel.GetFont() != m_Fontt);
    EXPECT_EQ(myLabel.GetFont(), myFont2);
#endif
}

TEST_F(Label, SerializeLabel) {
    // save label
    vrLabel myLabel;
    vrSerialize mySerialize;
    EXPECT_TRUE(myLabel.Serialize(mySerialize));
    EXPECT_TRUE(mySerialize.GetString() != wxEmptyString);
    wxLogMessage(mySerialize.GetString());

    // change label colour
    wxColour myColour(255, 0, 0);
    myLabel.SetColor(myColour);

    // deserialize
    vrSerialize mySerialize2(mySerialize.GetString());
    EXPECT_TRUE(myLabel.Serialize(mySerialize2));
    EXPECT_TRUE(myLabel.GetColor() != myColour);
}
