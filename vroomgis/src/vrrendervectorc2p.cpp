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

vrDipColour::vrDipColour(const wxColour & colour, long familyid) {
	m_Colour = colour;
	m_FamilyID = familyid;
}

vrDipColour::~vrDipColour() {
}
#include <wx/arrimpl.cpp> // This is a magic incantation which must be done!
WX_DEFINE_OBJARRAY(vrArrayDipColours);





vrRenderVectorC2PDips::vrRenderVectorC2PDips(const wxColour & defaultcolour, int size, int dipwith) {
	m_Type = vrRENDER_VECTOR_C2P_DIPS;
	m_DipWidth = dipwith;
	m_Size = size;
	m_UseDefaultColour = true;
    m_UseOutline = true;
	ClearDipColours();
	AddDipColour(defaultcolour, 0);
	m_MemoryFamilyID = wxNOT_FOUND;
	m_MemoryColour = wxColour();
	wxASSERT(m_MemoryColour.IsOk() == false);
}



vrRenderVectorC2PDips::~vrRenderVectorC2PDips() {
	ClearDipColours();
}



const int vrRenderVectorC2PDips::GetDipWidth() const {
  return m_DipWidth;
}


void vrRenderVectorC2PDips::SetDipWidth(int value) {
  m_DipWidth = value;
}


/*
void vrRenderVectorC2PDips::SetTransparency(int value) {
	vrRender::SetTransparency(value);
	// change all colours
	for (unsigned int i = 0; i<m_DipColours.GetCount(); i++) {
		wxColour myColour (m_DipColours.Item(i).m_Colour.Red(),
						   m_DipColours.Item(i).m_Colour.Green(),
						   m_DipColours.Item(i).m_Colour.Blue(),
						   GetTransparencyChar());
		m_DipColours.Item(i).m_Colour = myColour;
	}
}
*/


void vrRenderVectorC2PDips::SetSize(int value) {
  m_Size = value;
}



void vrRenderVectorC2PDips::SetOutline(bool value) {
    m_UseOutline = value;
}



wxColour vrRenderVectorC2PDips::GetOutlineColour(const wxColour & dipcolour) {
    int myColourLimit = 70;
    if (dipcolour.Green() < myColourLimit && 
        dipcolour.Red() < myColourLimit && 
        dipcolour.Blue() < myColourLimit) {
        return *wxWHITE;
    }
    return *wxBLACK;
}



void vrRenderVectorC2PDips::ClearDipColours() {
	m_DipColours.Clear();
	m_MemoryFamilyID = wxNOT_FOUND;
	m_MemoryColour = wxColour();
}



bool vrRenderVectorC2PDips::AddDipColour(const wxColour & colour, long familyid) {
	// ensure this family ID didn't exists
	for (unsigned int i = 0; i<m_DipColours.GetCount(); i++) {
		if (m_DipColours.Item(i).m_FamilyID == familyid) {
			wxLogError(_("Adding '%s' colour with '%d' id isn't allowed. ID allready used."),
					   colour.GetAsString(), familyid);
			return false;
		}
	}
	
	vrDipColour myDipColour (colour, familyid);
	m_DipColours.Add(myDipColour);
	return true;
}


bool vrRenderVectorC2PDips::SetDipColour(const wxColour & colour, long familyid) {
	for (unsigned int i = 0; i<m_DipColours.GetCount(); i++) {
		if (m_DipColours.Item(i).m_FamilyID == familyid) {
			m_DipColours.Item(i).m_Colour = colour;
			return true;
		}
	}
	wxLogError(_("Setting '%s' colour with '%d' id isn't allowed. ID didn't exist."),
			   colour.GetAsString(), familyid);
	return false;
}


wxColour vrRenderVectorC2PDips::GetDipColour(long familyID) {
	wxASSERT(familyID >= 0);
	wxASSERT(m_DipColours.GetCount() >= 1);
	
	// speed up search, by keeping last colour on memory
	if (familyID == m_MemoryFamilyID && m_MemoryColour.IsOk()) {
		return m_MemoryColour;
	}
	
	// not the same family, then search into array
	m_MemoryColour = wxColour();
	m_MemoryFamilyID = wxNOT_FOUND;
	for (unsigned int i = 0; i<m_DipColours.GetCount(); i++) {
		if (m_DipColours.Item(i).m_FamilyID == familyID) {
			m_MemoryFamilyID = familyID;
			m_MemoryColour = m_DipColours.Item(i).m_Colour;
            
            char myred = m_MemoryColour.Red();
            char mygreen = m_MemoryColour.Green();
            char myblue = m_MemoryColour.Blue();
            m_MemoryColour.Set(myred, mygreen, myblue, GetTransparencyChar());
			return m_MemoryColour;
		}
	}
	
	// if family not found, return the default colour
	return m_DipColours.Item(0).m_Colour;
}



int vrRenderVectorC2PDips::GetCountColor(){
	int myCount = m_DipColours.GetCount();
	// always the default colour
	wxASSERT(myCount > 0);
	return myCount;
}



bool vrRenderVectorC2PDips::Serialize(vrSerialize & serialobj) {
	vrRender::Serialize(serialobj);
	serialobj.EnterObject();
	if (serialobj.IsStoring()) {
		serialobj << GetSize();
		serialobj << GetDipWidth();
		serialobj << IsUsingDefaultColour();
		serialobj << m_DipColours.Item(0).m_Colour;
        serialobj << GetOutline();
	}
	else {
		serialobj >> m_Size;
		serialobj >> m_DipWidth;
		serialobj >> m_UseDefaultColour;
		serialobj >> m_DipColours.Item(0).m_Colour;
        serialobj >> m_UseOutline;
	}
	serialobj.LeaveObject();
	return serialobj.IsOk();
}






/*************************************************************************************//**
@brief c2p Polygon Renderer
@author Lucien Schreiber copyright CREALP
@date 23 janvier 2013
*****************************************************************************************/
vrRenderVectorC2PPoly::vrRenderVectorC2PPoly(const wxColour & defaultcolour) {
    m_Type = vrRENDER_VECTOR_C2P_POLY;
    m_UseDefaultBrush = true;
    SetColorBrush(defaultcolour);
    ClearPolyColours();
    m_MemoryFamilyID = wxNOT_FOUND;
	m_MemoryColour = wxColour();
	wxASSERT(m_MemoryColour.IsOk() == false);
}



vrRenderVectorC2PPoly::~vrRenderVectorC2PPoly() {
    ClearPolyColours();
}



void vrRenderVectorC2PPoly::ClearPolyColours() {
    m_PolyColours.Clear();
}



bool vrRenderVectorC2PPoly::AddPolyColour(const wxColour & colour, long familyid) {
    // ensure this family ID didn't exists
	for (unsigned int i = 0; i<m_PolyColours.GetCount(); i++) {
		if (m_PolyColours.Item(i).m_FamilyID == familyid) {
			wxLogError(_("Adding '%s' colour with '%d' id isn't allowed. ID allready used."),colour.GetAsString(), familyid);
			return false;
		}
	}
	
	vrDipColour myDipColour (colour, familyid);
	m_PolyColours.Add(myDipColour);
	return true;
}



bool vrRenderVectorC2PPoly::SetPolyColour(const wxColour & colour, long familyid) {
    for (unsigned int i = 0; i<m_PolyColours.GetCount(); i++) {
		if (m_PolyColours.Item(i).m_FamilyID == familyid) {
			m_PolyColours.Item(i).m_Colour = colour;
			return true;
		}
	}
	wxLogError(_("Setting '%s' colour with '%d' id isn't allowed. ID didn't exist."),colour.GetAsString(), familyid);
	return false;
}



wxColour vrRenderVectorC2PPoly::GetPolyColour(long familyid) {
    wxASSERT(familyid >= 0);
	wxASSERT(m_PolyColours.GetCount() >= 1);
	
	// speed up search, by keeping last colour on memory
	if (familyid == m_MemoryFamilyID && m_MemoryColour.IsOk()) {
		return m_MemoryColour;
	}
	
	// not the same family, then search into array
	m_MemoryColour = wxColour();
	m_MemoryFamilyID = wxNOT_FOUND;
	for (unsigned int i = 0; i<m_PolyColours.GetCount(); i++) {
		if (m_PolyColours.Item(i).m_FamilyID == familyid) {
			m_MemoryFamilyID = familyid;
			m_MemoryColour = m_PolyColours.Item(i).m_Colour;
            
            char myred = m_MemoryColour.Red();
            char mygreen = m_MemoryColour.Green();
            char myblue = m_MemoryColour.Blue();
            m_MemoryColour.Set(myred, mygreen, myblue, GetTransparencyChar());
			return m_MemoryColour;
		}
	}
	
	// if family not found, return the default colour
	return m_PolyColours.Item(0).m_Colour;
}


void vrRenderVectorC2PPoly::SetUseDefaultBrush(bool value) {
    m_UseDefaultBrush = value;
}


bool vrRenderVectorC2PPoly::Serialize(vrSerialize & serialobj) {
    vrRender::Serialize(serialobj);
	serialobj.EnterObject();
	if (serialobj.IsStoring()) {
		serialobj << GetBrushStyle();
		serialobj << GetColorBrush();
        serialobj << GetColorPen();
		serialobj << IsUsingDefaultBrush();
        serialobj << GetTransparency();
	}
	else {
        wxBrushStyle myBrushStyle;
        wxColour myPenColour;
        wxColour myBrushColour;
        
		serialobj >> myBrushStyle;
		serialobj >> myBrushColour;
        serialobj >> myPenColour;
		serialobj >> m_UseDefaultBrush;
        serialobj >> m_Transparency;
        
        SetBrushStyle(myBrushStyle);
        SetColorBrush(myBrushColour);
        SetColorPen(myPenColour);
	}
	serialobj.LeaveObject();
	return serialobj.IsOk();
}



