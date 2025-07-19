/***************************************************************************
 vroomgis.h
                    Main include for VROOMGIS engine
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

#ifndef _VROOMGIS_H_
#define _VROOMGIS_H_

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
// Include wxWidgets' headers
#ifndef WX_PRECOMP

#include <wx/wx.h>

#endif

#include "vrcoordinate.h"
#include "vrdisplaytool.h"
#include "vrevent.h"
#include "vrlayermanager.h"
#include "vrviewerdisplay.h"
#include "vrviewertoc.h"
#include "vrversion.h"

// \class vrVersion
// \brief Provides versioning information for the VROOMGIS engine.
class vrVersion {
        public:
        static wxString GetVersion() {
                return wxString::Format(_T("%s.%s.%s"), vroomgis_MAJOR_VERSION, vroomgis_MINOR_VERSION, GIT_NUMBER);
        }
        static wxString GetGitHash() { return wxString(GIT_REV); }
};

#endif
