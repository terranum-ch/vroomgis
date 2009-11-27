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

#ifndef _VR_LAYERMANAGER_H_
#define _VR_LAYERMANAGER_H_


#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif





class vrLayerManager 
{
	
private:
   // vrViewerLayerManager * m_ViewerLayerManager;
   // vrLayer * m_Layers;
	
public:
    //bool Create(const wxFileName & filename);
	
    //bool Open(const wxFileName & filename);
	
    //bool Close(vrLayer * layer);
	
    wxArrayString GetDriversExtension();
    wxString GetDriversWildcards();
	
    //bool AddViewerLayerManager(vrViewerTOC * toc = NULL, vrViewer * viewer = NULL);
	

	
};








#endif
