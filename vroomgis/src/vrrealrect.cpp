/***************************************************************************
 vrrealrect.cpp
 support intersection with negative width / height
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

#include "vrrealrect.h"

vrRealRect::vrRealRect()
{
    vrRealRect(0, 0, 0, 0);
}


vrRealRect::vrRealRect(const vrRealRect &source)
{
    m_x = source.m_x;
    m_y = source.m_y;
    m_width = source.m_width;
    m_height = source.m_height;
}

vrRealRect::vrRealRect(double left, double top, double width, double height)
{
    m_x = left;
    m_y = top;
    m_width = width;
    m_height = height;
}

vrRealRect::~vrRealRect()
{
}


vrRealRect vrRealRect::Intersect(const vrRealRect &rect1)
{
    wxDouble left, right, bottom, top;

    left = wxMax(rect1.m_x, m_x);
    right = wxMin(rect1.m_x + rect1.m_width, m_x + m_width);

    if (m_height < 0) {
        wxASSERT(rect1.m_height < 0);
        top = wxMin(rect1.m_y, m_y);
        bottom = wxMax(rect1.m_y + rect1.m_height, m_y + m_height);
    } else {
        top = wxMax(rect1.m_y, m_y);
        bottom = wxMin(rect1.m_y + rect1.m_height, m_y + m_height);
    }


    vrRealRect myIntersect;
    if (right < left) {
        // invalid rect (negative width)
        return myIntersect;
    }

    if (top < bottom) {
        // invalid rect (negative height)
        return myIntersect;
    }

    myIntersect.SetLeftTop(wxPoint2DDouble(left, top));
    myIntersect.SetRightBottom(wxPoint2DDouble(right, bottom));

    return myIntersect;
}


vrRealRect vrRealRect::Union(const wxRect2DDouble &rect)
{
    wxDouble left, right, bottom, top;

    left = wxMin(rect.m_x, m_x);
    right = wxMax(rect.m_x + rect.m_width, m_x + m_width);

    if (m_height < 0) {
        wxASSERT(rect.m_height < 0);
        top = wxMax(rect.m_y, m_y);
        bottom = wxMin(rect.m_y + rect.m_height, m_y + m_height);
    } else {
        top = wxMin(rect.m_y, m_y);
        bottom = wxMax(rect.m_y + rect.m_height, m_y + m_height);
    }


    vrRealRect myUnion;
    if (right < left) {
        // invalid rect (negative width)
        return myUnion;
    }

    if (top < bottom) {
        // invalid rect (negative height)
        return myUnion;
    }

    myUnion.SetLeftTop(wxPoint2DDouble(left, top));
    myUnion.SetRightBottom(wxPoint2DDouble(right, bottom));

    return myUnion;

}


bool vrRealRect::Contains(const wxPoint2DDouble &pt)
{
    if (pt.m_x < GetLeft()) {
        return false;
    }

    if (pt.m_x > GetRight()) {
        return false;
    }


    wxDouble myMin = wxMin(GetTop(), GetBottom());
    wxDouble myMax = wxMax(GetTop(), GetBottom());

    if (pt.m_y < myMin) {
        return false;
    }

    if (pt.m_y > myMax) {
        return false;
    }
    return true;
}


bool vrRealRect::IsOk() const
{
    if (m_width != 0 && m_width != 0) {
        return true;
    }
    return false;
}

