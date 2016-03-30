/***************************************************************************
 vrdisplaytool.cpp

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


#include "vrdisplaytool.h"
#include "vrviewerdisplay.h"
#include "vrrubberband.h"
#include "vrevent.h"
#include "../art/vroomgis_bmp.h"
#include "vrcoordinate.h"



/***************************************************************************//**
@brief Used for passing message from tool to application
@author Lucien Schreiber (c) CREALP 2010
@date 20 avril 2010
*******************************************************************************/
void vrDisplayToolMessage::_InitMembers()
{
    m_evtType = wxEVT_NULL;
    m_rect = wxRect();
    wxASSERT(m_rect.IsEmpty());
    m_position = wxDefaultPosition;
    m_parentManager = NULL;
    m_mouseStatus = vrMOUSE_UNKNOWN;
    m_longData = wxNOT_FOUND;
}

vrDisplayToolMessage::vrDisplayToolMessage(const wxEventType &eventtype, vrViewerDisplay *parent, const wxRect &rect,
                                           vrMOUSE_STATUS mousestat)
{
    _InitMembers();
    m_evtType = eventtype;
    m_rect = rect;
    m_parentManager = parent->GetViewerLayerManager();
    m_mouseStatus = mousestat;
}

vrDisplayToolMessage::vrDisplayToolMessage(const wxEventType &eventtype, vrViewerDisplay *parent, const wxPoint &pos,
                                           vrMOUSE_STATUS mousestat)
{
    _InitMembers();
    m_evtType = eventtype;
    m_position = pos;
    m_parentManager = parent->GetViewerLayerManager();
    m_mouseStatus = mousestat;

}

vrDisplayToolMessage::~vrDisplayToolMessage()
{
}







/***************************************************************************//**
@brief Base tool class
@author Lucien Schreiber (c) CREALP 2010
@date 20 avril 2010
*******************************************************************************/
vrDisplayTool::vrDisplayTool()
{
    m_iD = wxNOT_FOUND;
    m_name = wxEmptyString;
    m_rubber = NULL;
}


vrDisplayTool::vrDisplayTool(vrViewerDisplay *display, int id, wxString name, wxCursor cursor)
{
    Create(display, id, name, cursor);
}


void vrDisplayTool::Create(vrViewerDisplay *display, int id, wxString name, wxCursor cursor)
{
    wxASSERT(id != wxNOT_FOUND);
    wxASSERT(!name.IsEmpty());
    wxASSERT(display != NULL);

    m_iD = id;
    m_name = name;
    m_cursor = cursor;
    m_display = display;
    m_rubber = NULL;
}


vrDisplayTool::~vrDisplayTool()
{
    wxDELETE(m_rubber);
}


bool vrDisplayTool::MouseDown(const wxMouseEvent &event)
{
    // don't use this class directly but use the child instead
    wxFAIL;
    return false;
}

bool vrDisplayTool::MouseUp(const wxMouseEvent &event)
{
    // don't use this class directly but use the child instead
    wxFAIL;
    return false;
}

bool vrDisplayTool::MouseMove(const wxMouseEvent &event)
{
    // don't use this class directly but use the child instead
    wxFAIL;
    return false;
}


bool vrDisplayTool::MouseDClickLeft(const wxMouseEvent &event)
{
    return false;
}



/***************************************************************************//**
@brief Send a message to the parent
@param void
@param vrDisplayTool::SendMessage
@param vrDisplayToolMessage
@param message valid pointer, will be sent to parent throught SetClientData.
 Should be deleted when intercepting event
@author Lucien Schreiber (c) CREALP 2010
@date 20 avril 2010
 *******************************************************************************/
void vrDisplayTool::SendMessage(vrDisplayToolMessage *message)
{
    wxASSERT(m_display);
    wxASSERT(message);
    wxASSERT(message->m_evtType != wxEVT_NULL);

    wxCommandEvent myEvt(message->m_evtType);
    myEvt.SetClientData(message);

    // send message to the top level window if found
    wxWindow *myTopWin = wxGetTopLevelParent(m_display);
    if (myTopWin != NULL) {
        myTopWin->ProcessWindowEvent(myEvt);
        return;
    }

    // send message to the display if not found
    m_display->ProcessWindowEvent(myEvt);
}


vrDisplayToolDefault::vrDisplayToolDefault(vrViewerDisplay *display)
{
    Create(display, wxID_DEFAULT, "Select", wxCursor(wxCURSOR_ARROW));
}

vrDisplayToolDefault::~vrDisplayToolDefault()
{
}

bool vrDisplayToolDefault::MouseDown(const wxMouseEvent &event)
{

    vrDisplayToolMessage *myMsG = new vrDisplayToolMessage(vrEVT_TOOL_SELECT, GetDisplay(), event.GetPosition());
    SendMessage(myMsG);
    return true;
}

bool vrDisplayToolDefault::MouseUp(const wxMouseEvent &event)
{

    return true;
}

bool vrDisplayToolDefault::MouseMove(const wxMouseEvent &event)
{
    if (event.Dragging()) {
        //wxLogMessage("Dragging with selection");
    }
    return true;
}


vrDisplayToolSelect::vrDisplayToolSelect(vrViewerDisplay *display)
{
    Create(display, wxID_DEFAULT, "Select", wxCursor(wxCURSOR_ARROW));
}


vrDisplayToolSelect::~vrDisplayToolSelect()
{
}


bool vrDisplayToolSelect::MouseDown(const wxMouseEvent &event)
{
    wxASSERT(m_rubber == NULL);
    m_rubber = new vrRubberBand(GetDisplay());
    wxASSERT(m_rubber);
    m_rubber->SetPointFirst(event.GetPosition());
    return false;
}


bool vrDisplayToolSelect::MouseUp(const wxMouseEvent &event)
{
    if (m_rubber == NULL) {
        return true;
    }

    m_rubber->SetPointLast(event.GetPosition());

    wxRect myRect;
    myRect.SetLeftTop(event.GetPosition());
    myRect.SetRightBottom(event.GetPosition());
    if (m_rubber->IsValid()) {
        myRect = m_rubber->GetRect();
    }
    wxDELETE(m_rubber);

    /*wxLogMessage("Selection rect is %d, %d, %d, %d",
                 myRect.GetLeft(),
                 myRect.GetTop(),
                 myRect.GetWidth(),
                 myRect.GetHeight());*/

    vrDisplayToolMessage *myMessage = new vrDisplayToolMessage(vrEVT_TOOL_SELECT, GetDisplay(), myRect);
    wxASSERT(myMessage);
    SendMessage(myMessage);
    return true;

}

bool vrDisplayToolSelect::MouseMove(const wxMouseEvent &event)
{
    if (event.Dragging() && m_rubber != NULL) {
        m_rubber->SetPointLast(event.GetPosition());
        m_rubber->Update();
    }
    return false;
}


vrDisplayToolZoom::vrDisplayToolZoom(vrViewerDisplay *display)
{
    Create(display, wxID_ZOOM_IN, "Zoom", wxCursor(_img_cursor_zoomin->ConvertToImage()));
}

vrDisplayToolZoom::~vrDisplayToolZoom()
{

}

bool vrDisplayToolZoom::MouseDown(const wxMouseEvent &event)
{
    wxASSERT(m_rubber == NULL);
    m_rubber = new vrRubberBand(GetDisplay());
    wxASSERT(m_rubber);
    m_rubber->SetPointFirst(event.GetPosition());
    return false;
}


bool vrDisplayToolZoom::MouseUp(const wxMouseEvent &event)
{

    wxRect myRect;

    if (!m_rubber) {
        return false;
    }

    m_rubber->SetPointLast(event.GetPosition());
    if (!m_rubber->IsValid()) { // e.g. if user only clicks
        wxDELETE(m_rubber);

        // Set a rectangle centered with a size 1.5 times smaller
        myRect = wxRect(event.GetPosition().x / 3, event.GetPosition().y / 3, GetDisplay()->GetSize().GetWidth() / 1.5,
                        GetDisplay()->GetSize().GetHeight() / 1.5);
    } else {
        myRect = m_rubber->GetRect();
        wxDELETE(m_rubber);

        // Check that no dimension is too small
        if (myRect.GetWidth() < 5) {
            myRect.SetWidth(5);
        }
        if (myRect.GetHeight() < 5) {
            myRect.SetHeight(5);
        }

        /*wxLogMessage("Selection rect is %d, %d, %d, %d",
                     myRect.GetLeft(),
                     myRect.GetTop(),
                     myRect.GetWidth(),
                     myRect.GetHeight());*/
    }

    vrDisplayToolMessage *myMessage = new vrDisplayToolMessage(vrEVT_TOOL_ZOOM, GetDisplay(), myRect);
    wxASSERT(myMessage);
    SendMessage(myMessage);
    return true;
}


bool vrDisplayToolZoom::MouseMove(const wxMouseEvent &event)
{
    if (!m_rubber) {
        return false;
    }

    if (event.Dragging()) {
        m_rubber->SetPointLast(event.GetPosition());
        m_rubber->Update();
    }
    return false;
}


vrDisplayToolZoomOut::vrDisplayToolZoomOut(vrViewerDisplay *display)
        : vrDisplayToolZoom(display)
{
    Create(display, wxID_ZOOM_OUT, "Zoom out", wxCursor(_img_cursor_zoomout->ConvertToImage()));
}

vrDisplayToolZoomOut::~vrDisplayToolZoomOut()
{

}


bool vrDisplayToolZoomOut::MouseUp(const wxMouseEvent &event)
{

    wxRect myRect;

    if (!m_rubber) {
        return false;
    }

    m_rubber->SetPointLast(event.GetPosition());
    if (!m_rubber->IsValid()) { // e.g. if user only clicks
        wxDELETE(m_rubber);

        // Set a rectangle centered with a size 1.5 times smaller
        myRect = wxRect(event.GetPosition().x - GetDisplay()->GetSize().GetWidth() / 3,
                        event.GetPosition().y - GetDisplay()->GetSize().GetHeight() / 3,
                        GetDisplay()->GetSize().GetWidth() / 1.5, GetDisplay()->GetSize().GetHeight() / 1.5);
    } else {
        myRect = m_rubber->GetRect();
        wxDELETE(m_rubber);

        // Check that no dimension is too small
        if (myRect.GetWidth() < 5) {
            myRect.SetWidth(5);
        }
        if (myRect.GetHeight() < 5) {
            myRect.SetHeight(5);
        }

        /*wxLogMessage("Selection rect is %d, %d, %d, %d",
                     myRect.GetLeft(),
                     myRect.GetTop(),
                     myRect.GetWidth(),
                     myRect.GetHeight());*/
    }

    vrDisplayToolMessage *myMessage = new vrDisplayToolMessage(vrEVT_TOOL_ZOOMOUT, GetDisplay(), myRect);
    wxASSERT(myMessage);
    SendMessage(myMessage);
    return true;
}




/***************************************************************************//**
@brief Default pan tool
@author Lucien Schreiber (c) CREALP 2010
@date 28 avril 2010
 *******************************************************************************/
vrDisplayToolPan::vrDisplayToolPan(vrViewerDisplay *display)
{
    Create(display, wxID_DEFAULT, "Pan", wxCursor(wxCURSOR_HAND));
    m_point = wxDefaultPosition;
    m_panBitmap = NULL;
}


vrDisplayToolPan::~vrDisplayToolPan()
{
}


bool vrDisplayToolPan::MouseDown(const wxMouseEvent &event)
{
    wxASSERT(m_point == wxDefaultPosition);
    m_point = event.GetPosition();

    wxASSERT(m_panBitmap == NULL);
    wxBitmap *myDisplayBmpRef = GetDisplay()->GetBitmapRef();
    wxASSERT(myDisplayBmpRef);

    m_panBitmap = new wxBitmap(
            myDisplayBmpRef->GetSubBitmap(wxRect(0, 0, myDisplayBmpRef->GetWidth(), myDisplayBmpRef->GetHeight())));
    return true;
}


bool vrDisplayToolPan::MouseUp(const wxMouseEvent &event)
{
    if (m_panBitmap == NULL) {
        return false;
    }

    // compute the new raster origin
    wxPoint myNewPos(m_point.x - event.GetPosition().x, m_point.y - event.GetPosition().y);


    vrDisplayToolMessage *myMessage = new vrDisplayToolMessage(vrEVT_TOOL_PAN, GetDisplay(), myNewPos);
    wxASSERT(myMessage);
    SendMessage(myMessage);

    wxDELETE(m_panBitmap);
    m_point = wxDefaultPosition;
    return true;
}


bool vrDisplayToolPan::MouseMove(const wxMouseEvent &event)
{
    if (!event.Dragging() || m_panBitmap == NULL) {
        return false;
    }

    // compute the new raster origin
    wxPoint myNewPos(event.GetPosition().x - m_point.x, event.GetPosition().y - m_point.y);

    if (myNewPos.x == 0 && myNewPos.y == 0) {
        return false;
    }

    wxMemoryDC memdc;
    wxBitmap *myMovedBmp = new wxBitmap(m_panBitmap->GetSize());
    wxASSERT(myMovedBmp);
    memdc.SelectObject(*myMovedBmp);
    memdc.SetBrush(GetDisplay()->GetBackgroundColour());
    memdc.SetPen(GetDisplay()->GetBackgroundColour());
    memdc.DrawRectangle(0, 0, m_panBitmap->GetSize().x, m_panBitmap->GetSize().y);
    memdc.DrawBitmap(*m_panBitmap, myNewPos.x, myNewPos.y);
    memdc.SelectObject(wxNullBitmap);

    GetDisplay()->SetBitmap(myMovedBmp);
    wxDELETE(myMovedBmp);
    return true;
}


bool vrDisplayToolPan::MouseDClickLeft(const wxMouseEvent &event)
{
    // compute middle pixel
    wxSize myDisplaySize = GetDisplay()->GetSize();
    if (!myDisplaySize.IsFullySpecified() || myDisplaySize == wxSize(0, 0)) {
        wxLogError("Error getting display size!");
        return false;
    }
    wxPoint myMiddlePoint(wxRound(myDisplaySize.GetWidth() / 2.0), wxRound(myDisplaySize.GetHeight() / 2.0));
    // computing moved point
    wxPoint myMovedPoint(event.GetPosition() - myMiddlePoint);

    vrDisplayToolMessage *myMessage = new vrDisplayToolMessage(vrEVT_TOOL_PAN, GetDisplay(), myMovedPoint);
    wxASSERT(myMessage);
    SendMessage(myMessage);
    return true;
}



/***************************************************************************//**
@brief Default viewfinder tool
@author Lucien Schreiber (c) CREALP 2010
@date 29 avril 2010
 *******************************************************************************/
vrDisplayToolSight::vrDisplayToolSight(vrViewerDisplay *display)
{
    Create(display, wxID_DEFAULT, "Sight", wxCursor(wxCURSOR_CROSS));
}


vrDisplayToolSight::~vrDisplayToolSight()
{
}

bool vrDisplayToolSight::MouseDown(const wxMouseEvent &event)
{
    wxClientDC myDC(GetDisplay());
    wxDCOverlay overlaydc(m_overlay, &myDC);
    overlaydc.Clear();
    myDC.SetPen(*wxRED_PEN);
    myDC.CrossHair(event.GetPosition());


    vrDisplayToolMessage *myMsg = new vrDisplayToolMessage(vrEVT_TOOL_SIGHT, GetDisplay(), event.GetPosition(),
                                                           vrMOUSE_DOWN);
    SendMessage(myMsg);
    return true;
}

bool vrDisplayToolSight::MouseUp(const wxMouseEvent &event)
{
    {
        wxClientDC myDC(GetDisplay());
        wxDCOverlay overlaydc(m_overlay, &myDC);
        overlaydc.Clear();
    }
    m_overlay.Reset();

    vrDisplayToolMessage *myMsg = new vrDisplayToolMessage(vrEVT_TOOL_SIGHT, GetDisplay(), event.GetPosition(),
                                                           vrMOUSE_UP);
    SendMessage(myMsg);

    return true;
}

bool vrDisplayToolSight::MouseMove(const wxMouseEvent &event)
{
    if (!event.Dragging()) {
        return false;
    }


    {
        wxClientDC myDC(GetDisplay());
        wxDCOverlay overlaydc(m_overlay, &myDC);
        overlaydc.Clear();
    }
    m_overlay.Reset();


    wxClientDC myDC(GetDisplay());
    wxDCOverlay overlaydc(m_overlay, &myDC);
    overlaydc.Clear();
    myDC.SetPen(*wxRED_PEN);
    myDC.CrossHair(event.GetPosition());


    vrDisplayToolMessage *myMsg = new vrDisplayToolMessage(vrEVT_TOOL_SIGHT, GetDisplay(), event.GetPosition(),
                                                           vrMOUSE_MOVE);
    SendMessage(myMsg);
    return true;
}





/*************************************************************************************//**
@brief Generic editing tool
@author Lucien Schreiber copyright CREALP
@date 30 mars 2012
*****************************************************************************************/
vrDisplayToolEdit::vrDisplayToolEdit(vrViewerDisplay *display, int toolid)
{
    wxImage myEditCursorImg = _img_cursor_editing->ConvertToImage();
    myEditCursorImg.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 8);
    myEditCursorImg.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 8);
    Create(display, toolid, "Editing", wxCursor(myEditCursorImg));
}


vrDisplayToolEdit::~vrDisplayToolEdit()
{
}


bool vrDisplayToolEdit::MouseUp(const wxMouseEvent &event)
{
    vrDisplayToolMessage *myMessage = new vrDisplayToolMessage(vrEVT_TOOL_EDIT_FINISHED, GetDisplay(),
                                                               event.GetPosition());
    wxASSERT(myMessage);
    SendMessage(myMessage);
    return true;
}




/*************************************************************************************//**
@brief Line and polygons editing tool
@author Lucien Schreiber copyright CREALP
@date 30 mars 2012
*****************************************************************************************/
vrDisplayToolEditLine::vrDisplayToolEditLine(vrViewerDisplay *display, int toolid)
        : vrDisplayToolEdit(display, toolid)
{
    m_previousPoint = wxDefaultPosition;
    m_doubleClicked = false;
}


vrDisplayToolEditLine::~vrDisplayToolEditLine()
{
}


bool vrDisplayToolEditLine::MouseDown(const wxMouseEvent &event)
{
    return true;
}


bool vrDisplayToolEditLine::MouseUp(const wxMouseEvent &event)
{
    {
        wxClientDC myDC(GetDisplay());
        wxDCOverlay overlaydc(m_overlay, &myDC);
        overlaydc.Clear();
    }
    m_overlay.Reset();
    m_previousPoint = event.GetPosition();

    if (m_doubleClicked) {
        m_previousPoint = wxDefaultPosition;
        m_doubleClicked = false;
        vrDisplayToolMessage *myMessage = new vrDisplayToolMessage(vrEVT_TOOL_EDIT_FINISHED, GetDisplay(),
                                                                   event.GetPosition());
        wxASSERT(myMessage);
        SendMessage(myMessage);
        return true;
    }

    vrDisplayToolMessage *myMessage = new vrDisplayToolMessage(vrEVT_TOOL_EDIT, GetDisplay(), event.GetPosition());
    wxASSERT(myMessage);
    SendMessage(myMessage);
    return true;
}


bool vrDisplayToolEditLine::MouseMove(const wxMouseEvent &event)
{
    if (m_previousPoint == wxDefaultPosition) {
        return false;
    }

    {
        wxClientDC myDC(GetDisplay());
        wxDCOverlay overlaydc(m_overlay, &myDC);
        overlaydc.Clear();
    }
    m_overlay.Reset();

    wxClientDC myDC(GetDisplay());
    wxDCOverlay overlaydc(m_overlay, &myDC);
    overlaydc.Clear();
#ifdef __WXMAC__
    myDC.SetPen( *wxGREY_PEN );
#else
    myDC.SetPen(wxPen(*wxLIGHT_GREY, 2, wxSOLID));
#endif

    myDC.DrawLine(m_previousPoint, event.GetPosition());
    return true;
}


bool vrDisplayToolEditLine::MouseDClickLeft(const wxMouseEvent &event)
{
    m_doubleClicked = true;
    return true;
}




/***************************************************************************//**
@brief Points, lines, polygons, modification tool
@author Lucien Schreiber (c) CREALP 2012
@date 01 mai 2012
*******************************************************************************/
vrDisplayToolModify::vrDisplayToolModify(vrViewerDisplay *display)
{
    Create(display, wxID_DEFAULT, "Modify", wxCursor(wxCURSOR_CROSS));
    m_activeVertex = wxNOT_FOUND;
    m_geometryType = wkbUnknown;
}


vrDisplayToolModify::~vrDisplayToolModify()
{
}


void vrDisplayToolModify::ClearPoints()
{
    m_pointsX.Clear();
    m_pointsY.Clear();

    // clear overlay
    {
        wxClientDC myDC(GetDisplay());
        wxDCOverlay overlaydc(m_overlay, &myDC);
        overlaydc.Clear();
    }
    m_overlay.Reset();
}


void vrDisplayToolModify::AddToPoints(const wxPoint &pts)
{
    m_pointsX.Add(pts.x);
    m_pointsY.Add(pts.y);
}


bool vrDisplayToolModify::SetActiveGeometry(OGRGeometry *geometry, OGRwkbGeometryType geometrytype,
                                            vrCoordinate *coordinate)
{
    bool bReturn = true;
    m_geometryType = geometrytype;
    ClearPoints();
    switch (geometrytype) {
        case wkbLineString: {
            OGRLineString *myLine = (OGRLineString *) geometry;
            for (int i = 0; i < myLine->getNumPoints(); i++) {
                wxPoint myPixelPt;
                coordinate->ConvertToPixels(wxPoint2DDouble(myLine->getX(i), myLine->getY(i)), myPixelPt);
                AddToPoints(myPixelPt);
            }
        }
            break;


        case wkbPolygon: {
            OGRPolygon *myPoly = (OGRPolygon *) geometry;
            OGRLineString *myLine = (OGRLineString *) myPoly->getExteriorRing();
            for (int i = 0; i < myLine->getNumPoints(); i++) {
                wxPoint myPixelPt;
                coordinate->ConvertToPixels(wxPoint2DDouble(myLine->getX(i), myLine->getY(i)), myPixelPt);
                AddToPoints(myPixelPt);
            }
        }
            break;


        case wkbPoint: {
            OGRPoint *myPoint = (OGRPoint *) geometry;
            wxPoint myPixelPt;
            coordinate->ConvertToPixels(wxPoint2DDouble(myPoint->getX(), myPoint->getY()), myPixelPt);
            // add two points to display line when moving point.
            AddToPoints(myPixelPt);
            AddToPoints(myPixelPt);
        }
            break;

        default:
            wxLogError(_("Unsupported geometry type for modification! (%d)"), geometrytype);
            bReturn = false;
            break;
    }
    return bReturn;
}


bool vrDisplayToolModify::MouseDown(const wxMouseEvent &event)
{
    m_activeVertex = wxNOT_FOUND;
    if (m_pointsX.GetCount() == 0) {
        return true;
    }

    // search closest vertex
    wxPoint myClickedPt = event.GetPosition();
    wxRect myClickedRect(myClickedPt.x - 2, myClickedPt.y - 2, 5, 5);
    for (unsigned int i = 0; i < m_pointsX.GetCount(); i++) {
        wxPoint myPt(m_pointsX[i], m_pointsY[i]);
        if (myClickedRect.Contains(myPt)) {
            m_activeVertex = i;
            break;
        }
    }
    wxLogMessage(_T("Active vertex is : %d"), m_activeVertex);
    return true;
}


bool vrDisplayToolModify::MouseMove(const wxMouseEvent &event)
{
    if (m_activeVertex == wxNOT_FOUND) {
        return true;
    }

    // drawing modification lines
    {
        wxClientDC myDC(GetDisplay());
        wxDCOverlay overlaydc(m_overlay, &myDC);
        overlaydc.Clear();
    }
    m_overlay.Reset();

    wxClientDC myDC(GetDisplay());
    wxDCOverlay overlaydc(m_overlay, &myDC);
    overlaydc.Clear();
#ifdef __WXMAC__
    myDC.SetPen( *wxGREY_PEN );
#else
    myDC.SetPen(wxPen(*wxLIGHT_GREY, 2, wxSOLID));
#endif

    if (m_activeVertex != 0) {
        myDC.DrawLine(wxPoint(m_pointsX[m_activeVertex - 1], m_pointsY[m_activeVertex - 1]), event.GetPosition());
    } else if (m_geometryType == wkbPolygon) {
        // use last vertex as previous (polygon are closed)
        myDC.DrawLine(wxPoint(m_pointsX[m_pointsX.GetCount() - 2], m_pointsY[m_pointsY.GetCount() - 2]),
                      event.GetPosition());
    }

    if (m_activeVertex < (m_pointsX.GetCount() - 1)) {
        myDC.DrawLine(wxPoint(m_pointsX[m_activeVertex + 1], m_pointsY[m_activeVertex + 1]), event.GetPosition());
    } else if (m_geometryType == wkbPolygon) {
        // use first vertex as next one (polygon are closed)
        myDC.DrawLine(wxPoint(m_pointsX[1], m_pointsY[1]), event.GetPosition());
    }
    return true;
}


bool vrDisplayToolModify::MouseUp(const wxMouseEvent &event)
{
    if (m_activeVertex != wxNOT_FOUND) {
        vrDisplayToolMessage *myMessage = new vrDisplayToolMessage(vrEVT_TOOL_MODIFY_FINISHED, GetDisplay(),
                                                                   event.GetPosition());
        myMessage->m_longData = m_activeVertex;
        SendMessage(myMessage);
        m_activeVertex = wxNOT_FOUND;
        return true;
    }

    // search line
    ClearPoints();
    wxPoint myClickedPt = event.GetPosition();
    wxRect myClickedRect(myClickedPt.x - 2, myClickedPt.y - 2, 5, 5);
    vrDisplayToolMessage *myMessage = new vrDisplayToolMessage(vrEVT_TOOL_MODIFY, GetDisplay(), myClickedRect);
    SendMessage(myMessage);
    return true;
}







