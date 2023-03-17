/***************************************************************************
 vrviewerdisplay.h
 Default display
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
#ifndef _VRVIEWERDISPLAY_H
#define _VRVIEWERDISPLAY_H

#include "wx/wxprec.h"

#ifndef WX_PRECOMP

#include <wx/wx.h>

#endif

#include <wx/dcbuffer.h>

#include "vrvieweroverlay.h"

class vrCoordinate;

class vrDisplayTool;

class vrViewerLayerManager;

class vrViewerDisplay : public wxPanel {
  private:
    vrCoordinate* m_coordinate;
    wxBitmap* m_bmp;
    vrDisplayTool* m_tool;
    vrDisplayTool* m_toolSecondary;
    vrViewerLayerManager* m_viewerManager;
    wxStatusBar* m_status;
    int m_statusField;
    wxString m_statusErrText;
    vrViewerOverlayArray m_overlayArray;
    wxString m_noDataText;

    bool _DrawRoundedMessage(const wxString& text, const wxColour& colour = *wxLIGHT_GREY);

    void _InvalidateView(bool updatenow);

    void OnPaint(wxPaintEvent& event);

    void OnSizeChange(wxSizeEvent& event);

    void OnEraseBackground(wxPaintEvent& event);

    // mouse event functions
    void OnMouseDown(wxMouseEvent& event);

    void OnMouseUp(wxMouseEvent& event);

    void OnMouseMove(wxMouseEvent& event);

    void OnMouseDClickLeft(wxMouseEvent& event);

    void OnMouseCaptureLost(wxMouseEvent& event);

  public:
    vrViewerDisplay();

    vrViewerDisplay(wxWindow* parent, wxWindowID id = wxID_ANY, const wxColour& colour = *wxWHITE);

    virtual ~vrViewerDisplay();

    void SetViewerLayerManager(vrViewerLayerManager* value);

    vrViewerLayerManager* GetViewerLayerManager();

    void SetNoDataText(const wxString& text) {
        m_noDataText = text;
    }

    void SetBitmap(wxBitmap* bmp);

    wxBitmap* GetBitmapRef() {
        return m_bmp;
    }

    vrCoordinate* GetCoordinate() {
        return m_coordinate;
    }

    void SetStatusCoordinates(wxStatusBar* status, int field = 0, const wxString& errmsg = wxEmptyString);

    void SetToolDefault();

    void SetToolZoom();

    void SetToolZoomOut();

    void SetToolPan();

    void SetToolSight();

    void SetTool(vrDisplayTool* tool);

    void SetToolSecondary(vrDisplayTool* tool);

    inline const vrDisplayTool* GetTool() const;

    inline vrViewerOverlayArray* GetOverlayArrayRef();

    vrViewerOverlay* GetOverlayByName(const wxString& name);

    void ClearOverlayArray();
};

inline const vrDisplayTool* vrViewerDisplay::GetTool() const {
    return m_tool;
}

inline vrViewerOverlayArray* vrViewerDisplay::GetOverlayArrayRef() {
    return &m_overlayArray;
}

#endif
