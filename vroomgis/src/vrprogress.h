/***************************************************************************
				vrprogress.h
                    
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

#ifndef _VRPROGRESS_H_
#define _VRPROGRESS_H_

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
// Include wxWidgets' headers
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <wx/progdlg.h>
#include "tmpercent.h"


class vrProgress {
public:
    vrProgress();
    virtual ~vrProgress();
	
};


class vrProgressSimple : public vrProgress {
private:
    static wxProgressDialog * m_ProgressWnd;
    static wxWindow * m_Parent;
    static wxString m_Title;
    static wxString m_Message;
	static tmPercent * m_Percent;
	static bool m_Continue;
	
public:
    static void Init(wxWindow * parent, const wxString & title, const wxString & message);
    static int GDALUpdateSimple(double dfComplete, const char * pszMessage, void * pProgressArg);
	static void End();

};
#endif

