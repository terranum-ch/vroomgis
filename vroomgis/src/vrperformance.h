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
#include <mach/mach_init.h>
#include <mach/task.h>
#endif

#ifdef __LINUX__

#include <sys/sysinfo.h>

#endif

#ifdef __WXMSW__
#include <psapi.h>
#include <windows.h>
#else

#include <sys/resource.h>

#endif

class vrPerformance {
  private:
    wxStopWatch m_stopWatch;
    wxFileName m_filePath;
    wxFile m_file;

    size_t _GetUsedMemory(bool resident = false);

  public:
    vrPerformance(wxString file, wxString header = _T(""));

    virtual ~vrPerformance();

    void StopWork(wxString text = wxEmptyString);
};

#endif
