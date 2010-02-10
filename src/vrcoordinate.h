/***************************************************************************
 vrcoordinate.h

 -------------------
 copyright            : (C) 2010 CREALP Lucien Schreiber 
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
#ifndef _VRCOORDINATE_H
#define _VRCOORDINATE_H

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif


#include <wx/geometry.h>

class vrViewerDisplay;

class vrCoordinate {
  private:
    vrViewerDisplay * m_Viewer;
    wxRect2DDouble m_WndReal;


  public:
    vrCoordinate(vrViewerDisplay * viewer);
    ~vrCoordinate();

    wxRect2DDouble GetExtentReal();

};
#endif
