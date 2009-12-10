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

#ifndef _TEST_VR_RENDERER_H_
#define _TEST_VR_RENDERER_H_


#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "test_param.h"	//for test parameters
#include "vrrenderer.h"


class TEST_vrRenderer : public CxxTest::TestSuite
{
private:
	vrLayerManager * m_Manager;
	vrLayer * m_LayerShp;
	
public:
	void setUp()
	{
		m_LayerShp = NULL;
		m_Manager = new vrLayerManager;
		TS_ASSERT(m_Manager->Open(wxFileName(g_TestPath, g_TestFileSHP)));
		m_LayerShp = m_Manager->GetLayer(wxFileName(g_TestPath, g_TestFileSHP));
		TS_ASSERT(m_LayerShp != NULL);
	}
	
	void tearDown()
	{
		delete m_Manager;
	}
	
	
	void testRendererCreate()
	{
		TS_ASSERT(m_Manager != NULL);
		TS_ASSERT(m_LayerShp != NULL);
		
		vrRenderer myRenderer(m_LayerShp);
		TS_ASSERT_EQUALS(myRenderer.GetLayer()->GetType(), vrDRIVER_VECTOR_SHP);
		
		TS_ASSERT_EQUALS(myRenderer.GetRender()->GetType(), vrRENDER_VECTOR);
		TS_ASSERT_EQUALS(myRenderer.GetLabel()->GetColor(), *wxBLACK);
		
	}
	
	void testRenderCreate()
	{
		
		vrRenderRaster * myRenderRaster = new vrRenderRaster();
		vrRenderVector * myRenderVector = new vrRenderVector();
		
		// crash
		//vrRenderer myRenderer(m_LayerShp, myRenderRaster);
		
		// ok
		vrRenderer myRenderer (m_LayerShp, myRenderVector);
		
	}
	
	
};







#endif
