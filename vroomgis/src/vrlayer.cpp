/***************************************************************************
								vrlayer.cpp
					Abstact base class for layers
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


#include "vrlayer.h"

vrLayer::vrLayer()
{
    m_driverType = vrDRIVER_UNKNOWN;
    m_fileName.Clear();
}


vrLayer::~vrLayer()
{
}


wxFileName vrLayer::GetFileName()
{
    return m_fileName;
}


wxFileName vrLayer::GetDisplayName()
{
    return m_fileName;
}


vrDRIVERS_TYPE vrLayer::GetType()
{
    return m_driverType;
}


#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(vrArrayLayer);
