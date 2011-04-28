/***************************************************************************
 vrperformance.h
 -------------------
 copyright            : (C) 2011 CREALP Lucien Schreiber 
 email                : lucien.schreiber at crealp dot vs dot ch
 ***************************************************************************/

#ifndef _VRPERFORMANCE_H_
#define _VRPERFORMANCE_H_

// wxWidgets include
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/filename.h>

#ifdef __WXMAC__
# include <mach/task.h>
# include <mach/mach_init.h>
#endif


#ifdef __LINUX__
# include <sys/sysinfo.h>
#endif


#ifdef _WXMSW__
# include <windows.h>
#else
# include <sys/resource.h>
#endif

class vrPerformance {
  private:
    wxStopWatch m_StopWatch;
    wxFileName m_FilePath;
	wxFile m_File;

	
	size_t _GetUsedMemory (bool resident=false);


  public:
    vrPerformance(wxString file, wxString header = _T(""));
    virtual ~vrPerformance();

    void StopWork(wxString text = wxEmptyString);

};
#endif
