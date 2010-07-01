/***************************************************************************
				lsversion_core.cpp
                    
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

#include "lsversion_core.h"

wxString lsVersion::GetSoftName()
{
	wxString myName = wxEmptyString;
#ifdef lsVERSION_SOFT_NAME
	myName = lsVERSION_SOFT_NAME;
#endif
	return myName;
}

wxString lsVersion::GetSoftSVN()
{
	wxString mySVN = wxEmptyString;
#ifdef lsVERSION_SOFT_VERSION
	mySVN = lsVERSION_SOFT_VERSION;
#endif
	return mySVN;
}

wxString lsVersion::GetwxWidgetsNumber()
{
	wxString mywxVersion = wxString::Format("%d.%d.%d",
							wxMAJOR_VERSION,
							wxMINOR_VERSION,
							wxRELEASE_NUMBER);
	if (wxSUBRELEASE_NUMBER != 0) {
		mywxVersion.Append(wxString::Format(".%d",wxSUBRELEASE_NUMBER));
	}
	
	return mywxVersion; 
}

wxString lsVersion::GetwxWidgetsSVN()
{ 
	wxString mySVN = wxEmptyString;
#ifdef lsVERSION_WXWIDGETS_SVN
	mySVN = lsVERSION_WXWIDGETS_SVN;
#endif
	return mySVN;
}

wxString lsVersion::GetGDALNumber()
{
	wxString myGDAL = wxEmptyString;
#ifdef lsVERSION_HAS_GDAL
	myGDAL = GDAL_RELEASE_NAME;
#endif
	return myGDAL;
}

wxString lsVersion::GetGEOSNumber()
{
	wxString myGEOS = wxEmptyString;
#ifdef lsVERSION_HAS_GEOS
	myGEOS = GEOS_VERSION;
#endif
	return myGEOS;
}

wxString lsVersion::GetAllModules()
{
	wxString myModules = _T("wxWidgets : ") + GetwxWidgetsNumber();
#ifdef lsVERSION_WXWIDGETS_SVN
	myModules.Append(wxString::Format(" (%s)", GetwxWidgetsSVN()));
#endif
	myModules.Append(_T("\n"));
	
	// GDAL
#ifdef lsVERSION_HAS_GDAL
	myModules.Append(_T("GDAL : ") + GetGDALNumber() + _T("\n"));
#endif

#ifdef lsVERSION_HAS_GEOS
	myModules.Append(_T("GEOS : ") + GetGEOSNumber() + _T("\n"));
#endif	

	myModules.Append(wxGetOsDescription());	
	return myModules;
}

