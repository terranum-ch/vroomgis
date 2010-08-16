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

	virtual bool GetExtent(vrRealRect & rect){return false;}

	virtual bool IsOK();


};




class vrLayerRasterGDAL : public vrLayerRaster {
private:
	vrRealRect m_ImgExtent;
    wxSize m_ImgPxSize;
	
	// stat for one band data (Greyscale, MNT, etc)
	double m_OneBandMin;
    double m_OneBandMax;
    double m_OneBandNoData;
	
	
	
    bool _Close();
	bool _ComputeDisplayPosSize(const wxSize & pximgsize, const vrRealRect & imgextent,
								const vrRealRect & wndextent, double pxsize,
								wxRect & pximginfo, wxRect & pximgpos);
		
	bool _ComputeStat();
    bool _HasStat();
	bool _ComputeExtent();
	bool _HasExtent();
	
protected:
	double _ReadGDALValueToDouble(void* & data, const GDALDataType & type, int index);
	virtual bool _GetRasterData(unsigned char ** imgdata, const wxSize & outimgpxsize,
								const wxRect & readimgpxinfo, const vrRender * render);
	
	
public:
    vrLayerRasterGDAL();
    virtual ~vrLayerRasterGDAL();
	
	virtual bool Create(const wxFileName & filename);
    virtual bool Open(const wxFileName & filename, bool readwrite = false);
	
	virtual bool GetExtent(vrRealRect & rect);
	virtual bool GetData(wxImage * bmp, const vrRealRect & coord,  double pxsize,
						 const vrRender * render = NULL, const vrLabel * label = NULL);
	
	virtual bool GetPixelValue(double coordx, double coordy, wxArrayDouble & values);

	
	
};



class vrLayerRasterEGRID : public vrLayerRasterGDAL {
public:
    vrLayerRasterEGRID();
    virtual ~vrLayerRasterEGRID();
	
    virtual wxFileName GetDisplayName();
	
};

#endif
