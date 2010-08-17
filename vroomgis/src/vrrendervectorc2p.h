/***************************************************************************
 vrrendervectorc2p.h
 
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

#ifndef _VRRENDERVECTORC2P_H_
#define _VRRENDERVECTORC2P_H_

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
// Include wxWidgets' headers
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif


#include "vrrender.h"

class vrDipColour {
public:
    vrDipColour(const wxColour & colour, long familyid);
    ~vrDipColour();
	
    wxColour m_Colour;
    long m_FamilyID;
	
};
WX_DECLARE_OBJARRAY(vrDipColour, vrArrayDipColours);



class vrRenderVectorC2PDips : public vrRender {
private:
    vrArrayDipColours m_DipColours;
	
    int m_DipWidth;
    int m_Size;
	bool m_UseDefaultColour;

	
	long m_MemoryFamilyID;
	wxColour m_MemoryColour;
	
	
public:
    vrRenderVectorC2PDips(const wxColour & defaultcolour = *wxBLACK, int size = 1, int dipwith = 10);
    virtual ~vrRenderVectorC2PDips();
	
    const int GetDipWidth() const;
    void SetDipWidth(int value);
	virtual void SetTransparency(int value);
	
    inline const int GetSize() const;
    void SetSize(int value);
	
    void ClearDipColours();
    bool AddDipColour(const wxColour & colour, long familyid);
    wxColour GetDipColour(long familyID);
	int GetCountColor();
	
	inline const bool IsUsingDefaultColour() const;
    void SetUsingDefaultColour(bool value){m_UseDefaultColour = value;}
	
    virtual bool Serialize(vrSerialize & serialobj);
	
};

inline const int vrRenderVectorC2PDips::GetSize() const {
	return m_Size;
}


inline const bool vrRenderVectorC2PDips::IsUsingDefaultColour() const {
	return m_UseDefaultColour;
}



#endif

