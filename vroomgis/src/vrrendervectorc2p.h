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

class vrDipColour
{
public:
    vrDipColour(const wxColour &colour, long familyid, bool visible);

    ~vrDipColour();

    wxColour m_colour;
    long m_familyID;
    bool m_visible;

};

WX_DECLARE_OBJARRAY(vrDipColour, vrArrayDipColours);


class vrRenderVectorC2PDips
        : public vrRenderVector
{
private:
    vrArrayDipColours m_dipColours;

    int m_dipWidth;
    int m_size;
    bool m_useDefaultColour;
    bool m_useOutline;


    long m_memoryFamilyID;
    wxColour m_memoryColour;


public:
    vrRenderVectorC2PDips(const wxColour &defaultcolour = *wxBLACK, int size = 1, int dipwith = 10);

    virtual ~vrRenderVectorC2PDips();

    const int GetDipWidth() const;

    void SetDipWidth(int value);
    //virtual void SetTransparency(int value);

    inline const int GetSize() const;

    void SetSize(int value);

    inline const bool GetOutline() const;

    void SetOutline(bool value);

    wxColour GetOutlineColour(const wxColour &dipcolour);


    void ClearDipColours();

    bool AddDipColour(const wxColour &colour, long familyid, bool visible);

    bool SetDipColour(const wxColour &colour, long familyid, bool visible);

    wxColour GetDipColour(long familyID);

    bool IsFamilyVisible(long familyID);

    int GetCountColor();

    inline const bool IsUsingDefaultColour() const;

    void SetUsingDefaultColour(bool value)
    {
        m_useDefaultColour = value;
    }

    virtual bool Serialize(vrSerialize &serialobj);

};

inline const int vrRenderVectorC2PDips::GetSize() const
{
    return m_size;
}


inline const bool vrRenderVectorC2PDips::IsUsingDefaultColour() const
{
    return m_useDefaultColour;
}

inline const bool vrRenderVectorC2PDips::GetOutline() const
{
    return m_useOutline;
}






/*************************************************************************************//**
@brief c2p Polygon Renderer
@author Lucien Schreiber copyright CREALP
@date 23 janvier 2013
*****************************************************************************************/
class vrRenderVectorC2PPoly
        : public vrRenderVector
{
private:
    bool m_useDefaultBrush;
    vrArrayDipColours m_polyColours;
    long m_memoryFamilyID;
    wxColour m_memoryColour;


public:
    vrRenderVectorC2PPoly(const wxColour &defaultcolour = *wxBLACK);

    virtual ~vrRenderVectorC2PPoly();

    void ClearPolyColours();

    bool AddPolyColour(const wxColour &colour, long familyid);

    bool SetPolyColour(const wxColour &colour, long familyid);

    wxColour GetPolyColour(long familyid);

    inline const bool IsUsingDefaultBrush() const;

    void SetUseDefaultBrush(bool value);

    virtual bool Serialize(vrSerialize &serialobj);

};


inline const bool vrRenderVectorC2PPoly::IsUsingDefaultBrush() const
{
    return m_useDefaultBrush;
}


#endif

