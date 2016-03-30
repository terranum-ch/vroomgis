/***************************************************************************
				vrrendervectorc2p.cpp
                    
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

#include "vrrendervectorc2p.h"

vrDipColour::vrDipColour(const wxColour &colour, long familyid, bool visible)
{
    m_colour = colour;
    m_familyID = familyid;
    m_visible = visible;
}

vrDipColour::~vrDipColour()
{
}

#include <wx/arrimpl.cpp> // This is a magic incantation which must be done!
WX_DEFINE_OBJARRAY(vrArrayDipColours);


vrRenderVectorC2PDips::vrRenderVectorC2PDips(const wxColour &defaultcolour, int size, int dipwith)
{
    m_type = vrRENDER_VECTOR_C2P_DIPS;
    m_dipWidth = dipwith;
    m_size = size;
    m_useDefaultColour = true;
    m_useOutline = true;
    ClearDipColours();
    AddDipColour(defaultcolour, 0, true);
    m_memoryFamilyID = wxNOT_FOUND;
    m_memoryColour = wxColour();
    wxASSERT(m_memoryColour.IsOk() == false);
}


vrRenderVectorC2PDips::~vrRenderVectorC2PDips()
{
    ClearDipColours();
}


const int vrRenderVectorC2PDips::GetDipWidth() const
{
    return m_dipWidth;
}


void vrRenderVectorC2PDips::SetDipWidth(int value)
{
    m_dipWidth = value;
}


/*
void vrRenderVectorC2PDips::SetTransparency(int value) {
	vrRender::SetTransparency(value);
	// change all colours
	for (unsigned int i = 0; i<m_dipColours.GetCount(); i++) {
		wxColour myColour (m_dipColours.Item(i).m_colour.Red(),
						   m_dipColours.Item(i).m_colour.Green(),
						   m_dipColours.Item(i).m_colour.Blue(),
						   GetTransparencyChar());
		m_dipColours.Item(i).m_colour = myColour;
	}
}
*/


void vrRenderVectorC2PDips::SetSize(int value)
{
    m_size = value;
}


void vrRenderVectorC2PDips::SetOutline(bool value)
{
    m_useOutline = value;
}


wxColour vrRenderVectorC2PDips::GetOutlineColour(const wxColour &dipcolour)
{
    int myColourLimit = 70;
    if (dipcolour.Green() < myColourLimit && dipcolour.Red() < myColourLimit && dipcolour.Blue() < myColourLimit) {
        return *wxWHITE;
    }
    return *wxBLACK;
}


void vrRenderVectorC2PDips::ClearDipColours()
{
    m_dipColours.Clear();
    m_memoryFamilyID = wxNOT_FOUND;
    m_memoryColour = wxColour();
}


bool vrRenderVectorC2PDips::AddDipColour(const wxColour &colour, long familyid, bool visible)
{
    // ensure this family ID didn't exists
    for (unsigned int i = 0; i < m_dipColours.GetCount(); i++) {
        if (m_dipColours.Item(i).m_familyID == familyid) {
            wxLogError(_("Adding '%s' colour with '%d' id isn't allowed. ID allready used."), colour.GetAsString(),
                       familyid);
            return false;
        }
    }

    vrDipColour myDipColour(colour, familyid, visible);
    m_dipColours.Add(myDipColour);
    return true;
}


bool vrRenderVectorC2PDips::SetDipColour(const wxColour &colour, long familyid, bool visible)
{
    for (unsigned int i = 0; i < m_dipColours.GetCount(); i++) {
        if (m_dipColours.Item(i).m_familyID == familyid) {
            m_dipColours.Item(i).m_colour = colour;
            m_dipColours.Item(i).m_visible = visible;
            return true;
        }
    }
    wxLogError(_("Setting '%s' colour with '%d' id isn't allowed. ID didn't exist (dips)."), colour.GetAsString(),
               familyid);
    return false;
}


wxColour vrRenderVectorC2PDips::GetDipColour(long familyID)
{
    wxASSERT(familyID >= 0);
    wxASSERT(m_dipColours.GetCount() >= 1);

    // speed up search, by keeping last colour on memory
    if (familyID == m_memoryFamilyID && m_memoryColour.IsOk()) {
        return m_memoryColour;
    }

    // not the same family, then search into array
    m_memoryColour = wxColour();
    m_memoryFamilyID = wxNOT_FOUND;
    for (unsigned int i = 0; i < m_dipColours.GetCount(); i++) {
        if (m_dipColours.Item(i).m_familyID == familyID) {
            m_memoryFamilyID = familyID;
            m_memoryColour = m_dipColours.Item(i).m_colour;

            char myred = m_memoryColour.Red();
            char mygreen = m_memoryColour.Green();
            char myblue = m_memoryColour.Blue();
            m_memoryColour.Set(myred, mygreen, myblue, GetTransparencyChar());
            return m_memoryColour;
        }
    }

    // if family not found, return the default colour
    return m_dipColours.Item(0).m_colour;
}


bool vrRenderVectorC2PDips::IsFamilyVisible(long familyID)
{
    wxASSERT(familyID >= 0);
    wxASSERT(m_dipColours.GetCount() >= 1);

    for (unsigned int i = 0; i < m_dipColours.GetCount(); i++) {
        if (m_dipColours.Item(i).m_familyID == familyID) {
            return m_dipColours.Item(i).m_visible;
        }
    }

    // if family not found, return true as the default visibility
    return true;
}


int vrRenderVectorC2PDips::GetCountColor()
{
    int myCount = m_dipColours.GetCount();
    // always the default colour
    wxASSERT(myCount > 0);
    return myCount;
}


bool vrRenderVectorC2PDips::Serialize(vrSerialize &serialobj)
{
    vrRender::Serialize(serialobj);
    serialobj.EnterObject();
    if (serialobj.IsStoring()) {
        serialobj << GetSize();
        serialobj << GetDipWidth();
        serialobj << IsUsingDefaultColour();
        serialobj << m_dipColours.Item(0).m_colour;
        serialobj << GetOutline();
    } else {
        serialobj >> m_size;
        serialobj >> m_dipWidth;
        serialobj >> m_useDefaultColour;
        serialobj >> m_dipColours.Item(0).m_colour;
        serialobj >> m_useOutline;
    }
    serialobj.LeaveObject();
    return serialobj.IsOk();
}






/*************************************************************************************//**
@brief c2p Polygon Renderer
@author Lucien Schreiber copyright CREALP
@date 23 janvier 2013
*****************************************************************************************/
vrRenderVectorC2PPoly::vrRenderVectorC2PPoly(const wxColour &defaultcolour)
{
    m_type = vrRENDER_VECTOR_C2P_POLY;
    m_useDefaultBrush = true;
    SetColorBrush(defaultcolour);
    ClearPolyColours();
    AddPolyColour(defaultcolour, 0);
    m_memoryFamilyID = wxNOT_FOUND;
    m_memoryColour = wxColour();
    wxASSERT(m_memoryColour.IsOk() == false);
}


vrRenderVectorC2PPoly::~vrRenderVectorC2PPoly()
{
    ClearPolyColours();
}


void vrRenderVectorC2PPoly::ClearPolyColours()
{
    m_polyColours.Clear();
}


bool vrRenderVectorC2PPoly::AddPolyColour(const wxColour &colour, long familyid)
{
    // ensure this family ID didn't exists
    for (unsigned int i = 0; i < m_polyColours.GetCount(); i++) {
        if (m_polyColours.Item(i).m_familyID == familyid) {
            wxLogError(_("Adding '%s' colour with '%d' id isn't allowed. ID allready used."), colour.GetAsString(),
                       familyid);
            return false;
        }
    }

    vrDipColour myDipColour(colour, familyid, true);
    m_polyColours.Add(myDipColour);
    return true;
}


bool vrRenderVectorC2PPoly::SetPolyColour(const wxColour &colour, long familyid)
{
    for (unsigned int i = 0; i < m_polyColours.GetCount(); i++) {
        if (m_polyColours.Item(i).m_familyID == familyid) {
            m_polyColours.Item(i).m_colour = colour;
            return true;
        }
    }
    //wxLogError(_("Setting '%s' colour with '%ld' id isn't allowed. ID didn't exist (polygons)."),colour.GetAsString(), familyid);
    return false;
}


wxColour vrRenderVectorC2PPoly::GetPolyColour(long familyid)
{
    wxASSERT(familyid >= 0);
    wxASSERT(m_polyColours.GetCount() >= 1);

    // speed up search, by keeping last colour on memory
    if (familyid == m_memoryFamilyID && m_memoryColour.IsOk()) {
        return m_memoryColour;
    }

    // not the same family, then search into array
    m_memoryColour = wxColour();
    m_memoryFamilyID = wxNOT_FOUND;
    for (unsigned int i = 0; i < m_polyColours.GetCount(); i++) {
        if (m_polyColours.Item(i).m_familyID == familyid) {
            m_memoryFamilyID = familyid;
            m_memoryColour = m_polyColours.Item(i).m_colour;

            char myred = m_memoryColour.Red();
            char mygreen = m_memoryColour.Green();
            char myblue = m_memoryColour.Blue();
            m_memoryColour.Set(myred, mygreen, myblue, GetTransparencyChar());
            return m_memoryColour;
        }
    }

    // if family not found, return the default colour
    return m_polyColours.Item(0).m_colour;
}


void vrRenderVectorC2PPoly::SetUseDefaultBrush(bool value)
{
    m_useDefaultBrush = value;
}


bool vrRenderVectorC2PPoly::Serialize(vrSerialize &serialobj)
{
    vrRender::Serialize(serialobj);
    serialobj.EnterObject();
    if (serialobj.IsStoring()) {
        serialobj << GetBrushStyle();
        serialobj << GetColorBrush();
        serialobj << GetColorPen();
        serialobj << IsUsingDefaultBrush();
        serialobj << GetTransparency();
    } else {
        wxBrushStyle myBrushStyle;
        wxColour myPenColour;
        wxColour myBrushColour;

        serialobj >> myBrushStyle;
        serialobj >> myBrushColour;
        serialobj >> myPenColour;
        serialobj >> m_useDefaultBrush;
        serialobj >> m_transparency;

        SetBrushStyle(myBrushStyle);
        SetColorBrush(myBrushColour);
        SetColorPen(myPenColour);
    }
    serialobj.LeaveObject();
    return serialobj.IsOk();
}



