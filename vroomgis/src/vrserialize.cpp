/***************************************************************************
 vrserialize.cpp
 for serializating object in a simple way
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

#include "vrserialize.h"


// serializing data out
vrSerialize::vrSerialize()
{
	m_outdirection = true;
	m_stream = _T("");
}


// serializing data in
vrSerialize::vrSerialize(wxString stream)
{
	m_outdirection = false;
	m_stream = stream;
	m_divStream.SetString(m_stream, vrSERIAL_MAINSEP);
}



bool vrSerialize::CanRead ()
{
	if (m_outdirection)
	{
		wxLogError(_T("Error, stream is in writing mode"));
		return false;
	}
	
	if (m_stream.IsEmpty())
	{
		wxLogWarning(_T("Nothing to read, stream empty"));
		return false;
	}
	return true;
}




void vrSerialize::WriteInt (int value)
{
	if(CanStore())
	{
		m_stream.Append(wxString::Format(_T("%d"),value));
	}
	
}


int vrSerialize::ReadInt (const wxString & part)
{
	long lvalue = 0;
	if(!part.ToLong(&lvalue))
		wxLogError(_T("Error trying to convert string to integer"));
	
	return lvalue;
}



bool vrSerialize::ReadStream (wxString & part)
{
	if(CanRead())
	{
		if (m_divStream.HasMoreTokens())
		{ 
			part = m_divStream.GetNextToken();
			return true;
		}
	}
	part = wxEmptyString;
	return false;
}





vrSerialize & vrSerialize::operator << (bool value)
{
	if (CanStore())
	{
		WriteInt((int) value);
		AddSeparator();
	}
	return *this;
}


vrSerialize & vrSerialize::operator << (wxString value)
{
	if (CanStore())
	{
		m_stream.Append(value);
		AddSeparator();
	}
	return *this;
}


vrSerialize & vrSerialize::operator <<(const wxString& value)
{
	if (CanStore())
	{
		m_stream.Append(value);
		AddSeparator();
	}
	return *this;
}

vrSerialize & vrSerialize::operator <<(const wxChar* pvalue)
{
	wxString value(pvalue);
	if (CanStore())
	{
		m_stream.Append(value);
		AddSeparator();
	}
	return *this;
}


vrSerialize & vrSerialize::operator <<(wxColour value)
{
	if(CanStore())
	{
		m_stream.Append(value.GetAsString(wxC2S_CSS_SYNTAX));
		AddSeparator();
	}
	return *this;
}



vrSerialize & vrSerialize::operator <<(wxFont value)
{
	if(CanStore())
	{
		m_stream.Append(value.GetNativeFontInfoDesc());
		AddSeparator();
	}
	return *this;
}



vrSerialize & vrSerialize::operator <<(int value)
{
	if (CanStore())
	{
		WriteInt(value);
		AddSeparator();
	}
	return * this;
	
}



vrSerialize & vrSerialize::operator >> (bool & value)
{
	wxString partstream = wxEmptyString;
	if (ReadStream(partstream))
	{
        value = false;
        if (ReadInt(partstream) > 0) {
            value = true;
        }
	}
	return * this;
}


vrSerialize & vrSerialize::operator >> (wxString & value)
{
	wxString partstream = wxEmptyString;
	if(ReadStream(partstream))
	{
		value = partstream;
	}
	return * this;
}


vrSerialize & vrSerialize::operator >> (wxColour & value)
{
	wxString partstream = wxEmptyString;
	if (ReadStream(partstream))
	{
		wxColour tmpcol(partstream);
		if (tmpcol.IsOk())
			value = tmpcol;
	}
	return * this;
}


vrSerialize & vrSerialize::operator >> (wxFont & value)
{
	wxString partstream = wxEmptyString;
	if (ReadStream(partstream))
	{
        // TODO: This message is needed, otherwise crash in release mode (Mac) ?
        wxLogMessage("font steam = " + partstream);
		wxFont tmpfont (partstream);
		if (tmpfont.IsOk()){
			value = tmpfont;
		}
	}
	return * this;
}


vrSerialize & vrSerialize::operator >> (int & value)
{
	wxString partstream = wxEmptyString;
	if (ReadStream(partstream))
	{
		value = ReadInt(partstream);
	}
	return * this;
}


bool vrSerialize::IsStoring()
{
	if (m_outdirection)
		return true;
	return false;
}


