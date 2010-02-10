/***************************************************************************
				vrlayerraster.h
                    
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

#ifndef _VRLAYERRASTER_H
#define _VRLAYERRASTER_H

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
// Include wxWidgets' headers
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "vrlayer.h"

class vrLayerRaster : public vrLayer {
  protected:
    GDALDataset * m_Dataset;


  public:
    vrLayerRaster();
    virtual ~vrLayerRaster();
	virtual bool Create(const wxFileName & filename){return false;}
    virtual bool Open(const wxFileName & filename, bool readwrite = false){return false;}
	
	virtual bool GetExtent(wxRect2DDouble & rect){return false;}
	
	virtual bool IsOK();


};




class vrLayerRasterGDAL : public vrLayerRaster {
  private:
    bool _Close();


  public:
    vrLayerRasterGDAL();
    virtual ~vrLayerRasterGDAL();
	
	virtual bool Create(const wxFileName & filename);
    virtual bool Open(const wxFileName & filename, bool readwrite = false);
	
	virtual bool GetExtent(wxRect2DDouble & rect);


};
#endif
