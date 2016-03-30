/***************************************************************************
 vrevent.h
 Event definition for vroomgis

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

#ifndef _VREVENT_H_
#define _VREVENT_H_

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
// Include wxWidgets' headers
#ifndef WX_PRECOMP

#include <wx/wx.h>

#endif

// Event definition.
wxDECLARE_EVENT(vrEVT_VLM_RELOAD, wxCommandEvent);
wxDECLARE_EVENT(vrEVT_TOOL_ZOOM, wxCommandEvent);
wxDECLARE_EVENT(vrEVT_TOOL_ZOOMOUT, wxCommandEvent);
wxDECLARE_EVENT(vrEVT_TOOL_SELECT, wxCommandEvent);
wxDECLARE_EVENT(vrEVT_TOOL_PAN, wxCommandEvent);
wxDECLARE_EVENT(vrEVT_TOOL_SIGHT, wxCommandEvent);
wxDECLARE_EVENT(vrEVT_TOOL_EDIT, wxCommandEvent);
wxDECLARE_EVENT(vrEVT_TOOL_EDIT_FINISHED, wxCommandEvent);
wxDECLARE_EVENT(vrEVT_TOOL_MODIFY, wxCommandEvent);
wxDECLARE_EVENT(vrEVT_TOOL_MODIFY_FINISHED, wxCommandEvent);

#endif
