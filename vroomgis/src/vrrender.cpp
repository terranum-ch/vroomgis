/***************************************************************************
 vrrender.cpp
                    
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

#include "vrrender.h"

wxColour vrRender::m_defaultSelectionColour = *wxRED;

vrRender::vrRender() {
    m_type = vrRENDER_UNKNOWN;
    m_transparency = 0;
    m_selectionColour = m_defaultSelectionColour;
}

vrRender::~vrRender() {}

vrRENDER_TYPE vrRender::GetType() const {
    return m_type;
}

void vrRender::SetTransparency(int value) {
    wxASSERT(value >= 0 && value <= 100);
    m_transparency = value;
}

unsigned char vrRender::GetTransparencyChar() {
    int myValue = m_transparency * 255 / 100;
    return 255 - myValue;
}

void vrRender::SetSelectionColour(wxColour value) {
    m_selectionColour = value;
}

void vrRender::SetDefaultSelectionColour(wxColour value) {
    m_defaultSelectionColour = value;
}

wxColour vrRender::GetSelectionColour() {
    char myBlue = m_selectionColour.Blue();
    char myGreen = m_selectionColour.Green();
    char myRed = m_selectionColour.Red();
    return wxColour(myRed, myGreen, myBlue, GetTransparencyChar());
}

bool vrRender::Serialize(vrSerialize& serialobj) {
    serialobj.EnterObject();
    if (serialobj.IsStoring()) {
        serialobj << GetTransparency();
        // serialobj << GetSelectionColour();
    } else {
        serialobj >> m_transparency;
        // serialobj >> m_selectionColour;
    }
    serialobj.LeaveObject();
    return serialobj.IsOk();
}

vrRenderVector::vrRenderVector() {
    m_type = vrRENDER_VECTOR;
    m_size = 1;
    m_colorPen = *wxBLACK;
    m_colorBrush = *wxBLACK;
    m_brushStyle = wxBRUSHSTYLE_SOLID;
    m_useFastAndUglyDC = false;
}

vrRenderVector::~vrRenderVector() {}

void vrRenderVector::SetColorPen(const wxColour& color) {
    m_colorPen = color;
}

void vrRenderVector::SetColorBrush(const wxColour& color) {
    m_colorBrush = color;
}

void vrRenderVector::SetSize(int value) {
    m_size = value;
}

void vrRenderVector::SetUseFastAndUglyDC(bool value) {
    m_useFastAndUglyDC = value;
}

wxColour vrRenderVector::GetColorPen() {
    char myRed = m_colorPen.Red();
    char myGreen = m_colorPen.Green();
    char myBlue = m_colorPen.Blue();

    m_colorPen.Set(myRed, myGreen, myBlue, GetTransparencyChar());
    return m_colorPen;
}

wxColour vrRenderVector::GetColorBrush() {
    char myRed = m_colorBrush.Red();
    char myGreen = m_colorBrush.Green();
    char myBlue = m_colorBrush.Blue();

    m_colorBrush.Set(myRed, myGreen, myBlue, GetTransparencyChar());
    return m_colorBrush;
}

wxBrushStyle vrRenderVector::GetBrushStyle() {
    return m_brushStyle;
}

void vrRenderVector::SetBrushStyle(wxBrushStyle brushstyle) {
    m_brushStyle = brushstyle;
}

bool vrRenderVector::Serialize(vrSerialize& serialobj) {
    vrRender::Serialize(serialobj);
    serialobj.EnterObject();
    if (serialobj.IsStoring()) {
        serialobj << GetColorPen();
        serialobj << GetColorBrush();
        serialobj << GetSize();
        serialobj << GetBrushStyle();
    } else {
        serialobj >> m_colorPen;
        serialobj >> m_colorBrush;
        serialobj >> m_size;
        serialobj >> m_brushStyle;
    }
    serialobj.LeaveObject();
    return serialobj.IsOk();
}

vrRenderRaster::vrRenderRaster() {
    m_type = vrRENDER_RASTER;
}

vrRenderRaster::~vrRenderRaster() {}
