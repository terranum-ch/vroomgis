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
	#include <wx/filename.h>
#endif


#include "vrlayer.h"
#include "vrlayervector.h"
#include "vrlayerraster.h"
#include "vrviewerlayermanager.h"


class vrLayerManager 
{
	
private:
	vrArrayViewerLayerManager m_ViewerManagers;
	vrArrayLayer m_Layers;
	
public:
	vrLayerManager();
    virtual ~vrLayerManager();
	
    bool Open(const wxFileName & filename);	
	bool Close(vrLayer * layer);
	int GetCount();
	vrLayer * GetLayer(const wxFileName & filename);
	
	bool AddViewerLayerManager(vrViewerLayerManager * manager);//, vrViewerTOC * toc);
	
};








#endif
