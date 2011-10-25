/***************************************************************************
 vrserialize.h
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




#ifndef _VR_SERIALIZE_H_
#define _VR_SERIALIZE_H_

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <wx/tokenzr.h>	// for string tokenizer


const int VRSERIALIZE_VERSION = 1; // increment each time something change
const wxString vrSERIAL_MAINSEP = _T("|");
const wxString vrSERIAL_AUXSEP = _T("§");


class vrSerialize
	{
	private:
		bool m_outdirection;
		wxString m_stream;
		wxStringTokenizer m_divStream;
		
		bool CanStore () {return m_outdirection;}
		bool CanRead ();
		void AddSeparator() {m_stream.Append(vrSERIAL_MAINSEP);}
		void AddAuxSeparator() {m_stream.Append(vrSERIAL_AUXSEP);}
		
		void WriteInt (int value);
		int ReadInt (const wxString & part);
		bool ReadStream (wxString & part);
		
	protected:
	public:
		vrSerialize();
		vrSerialize(wxString stream);
		virtual ~vrSerialize(){;}
		
		
		
		virtual vrSerialize &operator << (bool value);
		virtual vrSerialize &operator << (wxString value);
		virtual vrSerialize &operator <<(const wxString& value);	
		virtual vrSerialize &operator <<(const wxChar* pvalue);
		virtual vrSerialize &operator <<(wxColour value);
		virtual vrSerialize &operator <<(wxFont value);
		virtual vrSerialize &operator <<(int value);

		virtual vrSerialize &operator >> (bool & value);
		virtual vrSerialize &operator >> (wxString  & value);
		virtual vrSerialize &operator >> (wxColour & value);
		virtual vrSerialize &operator >> (wxFont & value);
		virtual vrSerialize &operator >> (int & value);
        virtual vrSerialize &operator >> (wxBrushStyle & value);
		
				
		
		wxString GetString(){return m_stream;}
		bool IsStoring();
		bool IsOk() {return true;}
		
		// for compatibility with wxSerialize
		void EnterObject(){;}
		void LeaveObject(){;}

	};





#endif
