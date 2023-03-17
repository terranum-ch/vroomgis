/***************************************************************************
 vrviewerdisplay.cpp
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

#include "vrviewerdisplay.h"

#include "vrcoordinate.h"
#include "vrdisplaytool.h"
#include "vrevent.h"
#include "vrviewerlayermanager.h"

bool vrViewerDisplay::_DrawRoundedMessage(const wxString& text, const wxColour& colour) {
    if (text.IsEmpty()) {
        wxLogError("No text specified, specify a text");
        return false;
    }

    wxClientDC dc(this);
    wxBufferedDC bdc(&dc, dc.GetSize());
    wxASSERT(bdc.IsOk());
    int width = 0;
    int height = 0;
    dc.GetSize(&width, &height);
    if (width < 20 || height < 20) {
        wxLogError("Windows size is too small for drawing");
        return false;
    }
    // erase screen
    bdc.Clear();
    bdc.SetBrush(wxBrush(GetBackgroundColour()));
    bdc.SetPen(wxPen(GetBackgroundColour()));

    // bdc.SetBrush(*wxWHITE_BRUSH);
    // bdc.SetPen(*wxWHITE_PEN);
    bdc.DrawRectangle(0, 0, width, height);

    // draw rectangle
    bdc.SetBrush(wxBrush(colour));
    wxRect myRect(width / 4, height / 3, width / 2, height / 3);
    bdc.DrawRoundedRectangle(myRect, height / 20);

    // draw text
    wxFont myFont(myRect.GetWidth() / text.Len(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    wxASSERT(myFont.IsOk());
    bdc.SetFont(myFont);
    bdc.SetTextForeground(*wxWHITE);

    wxSize myTextSize = bdc.GetTextExtent(text);
    wxRect myTextRectComputed(myTextSize);
    myTextRectComputed = myTextRectComputed.CenterIn(myRect, wxBOTH);
    bdc.DrawText(text, myTextRectComputed.GetX(), myTextRectComputed.GetY());
    return true;
}

void vrViewerDisplay::OnPaint(wxPaintEvent& event) {
    wxPaintDC dc(this);

    if (m_bmp == NULL) {
        _DrawRoundedMessage(m_noDataText);
        return;
    }

    dc.DrawBitmap(*m_bmp, 0, 0, true);

    // draw overlay
    for (unsigned int i = 0; i < m_overlayArray.GetCount(); i++) {
        vrViewerOverlay* myOverlay = m_overlayArray[i];
        if (myOverlay == NULL || !myOverlay->IsVisible()) {
            continue;
        }
        myOverlay->DrawOverlay(&dc);
    }
}

void vrViewerDisplay::OnSizeChange(wxSizeEvent& event) {
    if (event.GetSize().GetWidth() < 2 && event.GetSize().GetHeight() < 2) {
        event.Skip();
        return;
    }

    if (m_viewerManager == NULL) {
        return;
    }

    wxASSERT(m_coordinate);
    if (m_coordinate->IsOk()) {
        m_coordinate->UpdateExtent();
    }

    m_viewerManager->Reload();

    _InvalidateView(false);

    event.Skip();
}

void vrViewerDisplay::OnEraseBackground(wxPaintEvent& event) {
    // this is used to avoid flickering
}

void vrViewerDisplay::OnMouseDown(wxMouseEvent& event) {
    SetFocus();
    CaptureMouse();
    if (m_tool != NULL) {
        m_tool->MouseDown(event);
    }
}

void vrViewerDisplay::OnMouseUp(wxMouseEvent& event) {
    if (HasCapture()) {
        ReleaseMouse();
    }

    if (m_tool != NULL) {
        m_tool->MouseUp(event);
    }
}

void vrViewerDisplay::OnMouseMove(wxMouseEvent& event) {
    if (m_tool != NULL) {
        m_tool->MouseMove(event);
    }

    if (m_toolSecondary != NULL) {
        m_toolSecondary->MouseMove(event);
    }

    if (m_status != NULL) {
        wxPoint2DDouble myCoord;
        if (!GetCoordinate()->GetExtent().IsOk() || m_bmp == NULL) {
            m_status->SetStatusText(m_statusErrText, m_statusField);
            return;
        }
        GetCoordinate()->ConvertFromPixels(event.GetPosition(), myCoord);
        m_status->SetStatusText(wxString::Format("%.2f  %.2f", myCoord.m_x, myCoord.m_y), m_statusField);
    }
}

void vrViewerDisplay::OnMouseDClickLeft(wxMouseEvent& event) {
    SetFocus();
    if (m_tool != NULL) {
        m_tool->MouseDClickLeft(event);
    }
}

void vrViewerDisplay::OnMouseCaptureLost(wxMouseEvent& event) {
    // Only used under Windows. Mouse capture is lost when a dialog is displayed.
    if (HasCapture()) {
        ReleaseMouse();
    }
}

vrViewerDisplay::vrViewerDisplay() {
    m_coordinate = NULL;
    m_bmp = NULL;
    m_tool = NULL;
    m_toolSecondary = NULL;
    m_viewerManager = NULL;
    m_status = NULL;
    m_statusField = 0;
    m_statusErrText = wxEmptyString;
    m_noDataText = wxEmptyString;
    wxLogError("Don't use this vrViewerDisplay ctor, only here for tests");
}

vrViewerDisplay::vrViewerDisplay(wxWindow* parent, wxWindowID id, const wxColour& colour)
    : wxPanel(parent, id) {
    m_coordinate = new vrCoordinate(this);
    m_bmp = NULL;
    m_tool = NULL;
    m_toolSecondary = NULL;
    m_viewerManager = NULL;
    m_status = NULL;
    m_statusField = 0;
    m_statusErrText = wxEmptyString;
    m_noDataText = _("No GIS Data");
    SetBackgroundColour(colour);

    // connect event
    Connect(wxEVT_ERASE_BACKGROUND, wxPaintEventHandler(vrViewerDisplay::OnEraseBackground), NULL, this);
    Connect(wxEVT_SIZE, wxSizeEventHandler(vrViewerDisplay::OnSizeChange), NULL, this);
    Connect(wxEVT_PAINT, wxPaintEventHandler(vrViewerDisplay::OnPaint), NULL, this);

    // connect mouse event
    Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(vrViewerDisplay::OnMouseDown), NULL, this);
    Connect(wxEVT_LEFT_UP, wxMouseEventHandler(vrViewerDisplay::OnMouseUp), NULL, this);
    Connect(wxEVT_MOTION, wxMouseEventHandler(vrViewerDisplay::OnMouseMove), NULL, this);
    Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(vrViewerDisplay::OnMouseDClickLeft), NULL, this);
    Connect(wxEVT_MOUSE_CAPTURE_LOST, wxMouseEventHandler(vrViewerDisplay::OnMouseCaptureLost), NULL, this);
}

vrViewerDisplay::~vrViewerDisplay() {
    // disconnect event
    Disconnect(wxEVT_ERASE_BACKGROUND, wxPaintEventHandler(vrViewerDisplay::OnEraseBackground), NULL, this);
    Disconnect(wxEVT_SIZE, wxSizeEventHandler(vrViewerDisplay::OnSizeChange), NULL, this);
    Disconnect(wxEVT_PAINT, wxPaintEventHandler(vrViewerDisplay::OnPaint), NULL, this);

    // disconnect mouse event
    Disconnect(wxEVT_LEFT_DOWN, wxMouseEventHandler(vrViewerDisplay::OnMouseDown), NULL, this);
    Disconnect(wxEVT_LEFT_UP, wxMouseEventHandler(vrViewerDisplay::OnMouseUp), NULL, this);
    Disconnect(wxEVT_MOTION, wxMouseEventHandler(vrViewerDisplay::OnMouseMove), NULL, this);
    Disconnect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(vrViewerDisplay::OnMouseDClickLeft), NULL, this);
    Disconnect(wxEVT_MOUSE_CAPTURE_LOST, wxMouseEventHandler(vrViewerDisplay::OnMouseCaptureLost), NULL, this);

    wxDELETE(m_coordinate);
    wxDELETE(m_tool);
    wxDELETE(m_toolSecondary);
    wxDELETE(m_bmp);

    ClearOverlayArray();
}

void vrViewerDisplay::SetViewerLayerManager(vrViewerLayerManager* value) {
    wxASSERT(value);
    m_viewerManager = value;
}

vrViewerLayerManager* vrViewerDisplay::GetViewerLayerManager() {
    return m_viewerManager;
}

void vrViewerDisplay::SetBitmap(wxBitmap* bmp) {
    wxDELETE(m_bmp);

    // reference copy, this isn't expensive
    if (bmp != NULL) {
        m_bmp = new wxBitmap(*bmp);
    }
    _InvalidateView(true);
}

void vrViewerDisplay::SetStatusCoordinates(wxStatusBar* status, int field, const wxString& errmsg) {
    wxASSERT(status);
    m_status = status;
    m_statusField = field;
    m_statusErrText = errmsg;
    m_status->SetStatusText(m_statusErrText, m_statusField);
}

void vrViewerDisplay::_InvalidateView(bool updatenow) {
    this->Refresh();

    if (updatenow) {
        this->Update();
    }
}

void vrViewerDisplay::SetToolDefault() {
    SetTool(new vrDisplayToolDefault(this));
}

void vrViewerDisplay::SetToolZoom() {
    SetTool(new vrDisplayToolZoom(this));
}

void vrViewerDisplay::SetToolZoomOut() {
    SetTool(new vrDisplayToolZoomOut(this));
}

void vrViewerDisplay::SetToolPan() {
    SetTool(new vrDisplayToolPan(this));
}

void vrViewerDisplay::SetToolSight() {
    SetTool(new vrDisplayToolSight(this));
}

void vrViewerDisplay::SetTool(vrDisplayTool* tool) {
    wxDELETE(m_tool);

    wxASSERT(m_tool == NULL);
    if (tool == NULL) {
        return;
    }
    m_tool = tool;
    SetCursor(m_tool->GetCursor());
}

/*********************************************/ /**
 @brief Set a secondary Tool
 @details Cursor from the secondary tool is not
  used, he is mainly used for displaying live
  informations when mouse move for exemple
 @param tool the tool (derived from vrDisplayTool)
 @date 23 décembre 2010
 *************************************************/
void vrViewerDisplay::SetToolSecondary(vrDisplayTool* tool) {
    wxDELETE(m_toolSecondary);
    wxASSERT(m_toolSecondary == NULL);

    if (tool == NULL) {
        return;
    }

    m_toolSecondary = tool;
}

vrViewerOverlay* vrViewerDisplay::GetOverlayByName(const wxString& name) {
    for (unsigned int i = 0; i < m_overlayArray.GetCount(); i++) {
        vrViewerOverlay* myOverlay = m_overlayArray[i];
        if (myOverlay == NULL) {
            continue;
        }
        if (myOverlay->GetName() == name) {
            return myOverlay;
        }
    }
    return NULL;
}

void vrViewerDisplay::ClearOverlayArray() {
    unsigned int oCount = m_overlayArray.GetCount();
    for (unsigned int i = 0; i < oCount; i++) {
        vrViewerOverlay* myOverlay = m_overlayArray[0];
        wxDELETE(myOverlay);
        m_overlayArray.RemoveAt(0);
    }
    wxASSERT(m_overlayArray.GetCount() == 0);
}
