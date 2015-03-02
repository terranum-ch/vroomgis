/***************************************************************************
 vrperformance.cpp
 -------------------
 copyright            : (C) 2011 CREALP Lucien Schreiber
 email                : lucien.schreiber at crealp dot vs dot ch
 ***************************************************************************/


#include "vrperformance.h"

size_t vrPerformance::_GetUsedMemory (bool resident){
#if defined(__LINUX__)
    // Ugh, getrusage doesn't work well on Linux.  Try grabbing info
    // directly from the /proc pseudo-filesystem.  Reading from
    // /proc/self/statm gives info on your own process, as one line of
    // numbers that are: virtual mem program size, resident set size,
    // shared pages, text/code, data/stack, library, dirty pages.  The
    // mem sizes should all be multiplied by the page size.
    size_t size = 0;
    FILE *file = fopen("/proc/self/statm", "r");
    if (file) {
        unsigned long vm = 0;
        fscanf (file, "%lu", &vm);  // Just need the first num: vm size
        fclose (file);
		size = (size_t)vm * getpagesize();
    }
    return size;

#elif defined(__WXMAC__)
    // Inspired by:
    // http://miknight.blogspot.com/2005/11/resident-set-size-in-mac-os-x.html
    struct task_basic_info t_info;
    mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;
    task_info(current_task(), TASK_BASIC_INFO, (task_info_t)&t_info, &t_info_count);
    size_t size = (resident ? t_info.resident_size : t_info.virtual_size);
    return size;

#elif defined(__WXMSW__)
    // According to MSDN...
    PROCESS_MEMORY_COUNTERS counters;
    if (GetProcessMemoryInfo (GetCurrentProcess(), &counters, sizeof (counters)))
        return counters.PagefileUsage;
    else return 0;

#else
    // No idea what platform this is
    return 0;   // Punt
#endif
}



vrPerformance::vrPerformance(wxString file, wxString header) {
	wxASSERT(file != wxEmptyString);
	m_filePath = wxFileName(file);
	m_stopWatch.Start(0);
	m_file.Open(m_filePath.GetFullPath(), wxFile::write_append);
	if (m_file.Length() < 10) {
		m_file.Write(_("Date\tPlateform\tDebug\tElapsed time\tUsed Memory\t") + header + _T("\n"));
	}
}



vrPerformance::~vrPerformance() {
}



void vrPerformance::StopWork(wxString text) {
	long myUsedMem = _GetUsedMemory(true);
	wxString myDebug = _T("Undefined");
#ifndef NDEBUG
	myDebug = _("Yes");
#else
	myDebug = _("No");
#endif

	m_file.Write(wxString::Format(_T("%s\t%s\t%s\t%ld\t%ld\t"),
								  wxDateTime::Now().FormatISOCombined(),
								  wxGetOsDescription(),
								  myDebug,
								  m_stopWatch.Time(),
								  myUsedMem)
				 + text + _T("\n"));
}

