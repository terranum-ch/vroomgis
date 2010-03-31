/***************************************************************************
 vrdisplaytool.h

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
#ifndef _VRDISPLAYTOOL_H
#define _VRDISPLAYTOOL_H

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif


class vrDisplayTool {
  private:
    wxString m_Name;
    int m_ID;
    wxCursor m_Cursor;


  public:
    vrDisplayTool();
    vrDisplayTool(int id, wxString name, wxCursor cursor);
    void Create(int id, wxString name, wxCursor cursor);
    virtual ~vrDisplayTool();

    inline const wxString GetName() const;
    inline const int GetID() const;
    inline const wxCursor GetCursor() const;

    virtual bool MouseDown();
    virtual bool MouseUp();
    virtual bool MouseMove();

};
inline const wxString vrDisplayTool::GetName() const {
  return m_Name;
}

inline const int vrDisplayTool::GetID() const {
  return m_ID;
}

inline const wxCursor vrDisplayTool::GetCursor() const {
  return m_Cursor;
}





class vrDisplayToolDefault : public vrDisplayTool {
  public:
    vrDisplayToolDefault();
    virtual ~vrDisplayToolDefault();

    virtual bool MouseDown();
    virtual bool MouseUp();
    virtual bool MouseMove();

};




class vrDisplayToolZoom : public vrDisplayTool {
  public:
    vrDisplayToolZoom();
    virtual ~vrDisplayToolZoom();

    virtual bool MouseDown();
    virtual bool MouseUp();
    virtual bool MouseMove();

};
#endif
