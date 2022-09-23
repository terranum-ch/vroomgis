/***************************************************************************
								test_vrrender.h
					Rendering array storring vrLayer, vrRender, vrLabel
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

#include "test_param.h"
#include "vrrenderer.h"
#include "vrrender.h"
#include "vrlabel.h"
#include "vrlayermanager.h"


class Renderer : public ::testing::Test {
protected:
    vrLayerManager *m_Manager;
    vrLayer *m_LayerShp;

    virtual void setUp() {
        m_LayerShp = NULL;
        m_Manager = new vrLayerManager;
        EXPECT_TRUE(m_Manager->Open(wxFileName(g_TestPath, g_TestFileSHP)));
        m_LayerShp = m_Manager->GetLayer(wxFileName(g_TestPath, g_TestFileSHP));
        EXPECT_TRUE(m_LayerShp != NULL);
    }

    virtual void tearDown() {
        delete m_Manager;
    }
};

TEST_F(Renderer, RendererCreate) {
    EXPECT_TRUE(m_Manager != NULL);
    EXPECT_TRUE(m_LayerShp != NULL);

    vrRenderer myRenderer(m_LayerShp);
    EXPECT_EQ(myRenderer.GetLayer()->GetType(), vrDRIVER_VECTOR_SHP);

    EXPECT_EQ(myRenderer.GetRender()->GetType(), vrRENDER_VECTOR);
    EXPECT_EQ(myRenderer.GetLabel()->GetColor(), *wxBLACK);
}

TEST_F(Renderer, RenderCreate) {
    //vrRenderRaster * myRenderRaster = new vrRenderRaster();
    vrRenderVector *myRenderVector = new vrRenderVector();

    // crash
    //vrRenderer myRenderer(m_LayerShp, myRenderRaster);

    // ok
    vrRenderer myRenderer(m_LayerShp, myRenderVector);
}
