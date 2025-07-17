/***************************************************************************
 test_setup.h
 Setup tests
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
// #ifdef __LINUX__
//         #include <gtk/gtk.h>
// #endif
#include "vrlayer.h"

class Setup : public ::testing::Test {
  protected:
    virtual void SetUp() {
        wxApp::CheckBuildOptions(WX_BUILD_OPTIONS_SIGNATURE, "program");
        wxInitializer initializer;
        if (!initializer) {
            fprintf(stderr, "Failed to initialize the wxWidgets library, aborting.");
            ASSERT_TRUE("Unable to init the wxWigets library");
        }

// #ifdef __LINUX__
//          EXPECT_TRUE(gtk_init_check(NULL, NULL));
// #endif
    }
};

TEST_F(Setup, LogIsWorking) {
    // setting output to the std err (otherwise log into windows)
    wxLog::SetActiveTarget(new wxLogStderr());
    wxLogMessage("Setup file processed ");
};
