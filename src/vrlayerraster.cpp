/***************************************************************************
				vrlayerraster.cpp
                    
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


#include "vrlayerraster.h"
#include "vrrender.h"	// for symbology;

vrLayerRaster::vrLayerRaster() {
	m_Dataset = NULL;
}

vrLayerRaster::~vrLayerRaster() {
}


bool vrLayerRaster::IsOK() {
	if (m_Dataset==NULL){
		wxLogError("Dataset not inited");
		return false;
	}
	return true;
}







bool vrLayerRasterGDAL::_Close() {
	if (m_Dataset==NULL){
		return false;
	}
	
	wxLogMessage("Closing raster data NOW");
	GDALClose(m_Dataset);
	m_Dataset = NULL;
	return true;
}



vrLayerRasterGDAL::vrLayerRasterGDAL() {
}



vrLayerRasterGDAL::~vrLayerRasterGDAL() {
	_Close();
}



bool vrLayerRasterGDAL::Create(const wxFileName & filename) {
	return false;
}



bool vrLayerRasterGDAL::Open(const wxFileName & filename, bool readwrite) {
	// try to close
	_Close();
	wxASSERT(m_Dataset == NULL);
	
	// init filename and type
	m_FileName = filename;
	vrDrivers myDriver;
	m_DriverType = myDriver.GetType(filename.GetExt());
	
	GDALAccess myAcces = GA_ReadOnly;
	if (readwrite)
		myAcces = GA_Update;
	
	m_Dataset = (GDALDataset *) GDALOpen( filename.GetFullPath(), GA_ReadOnly );
	if(m_Dataset == NULL){
		wxLogError("Unable to open %s, maybe driver not regristred -  GDALAllRegister()", filename.GetFullName());
		return false;
	}
	
	return true;
}


bool vrLayerRasterGDAL::GetExtent(vrRealRect & rect) {
	
	if (m_Dataset == NULL) {
		wxLogError("No layer opened");
		return false;
	}
	
	wxASSERT(m_Dataset);
	double        adfGeoTransform[6];
	if( m_Dataset->GetGeoTransform( adfGeoTransform ) != CE_None ){
		wxLogError("Error getting geometric informations for layer : %s",
				   m_FileName.GetFullName());
		return false;
	}
	
	if (adfGeoTransform[5] > 0) {
		wxLogError("vertical pixel size is Wrong (should be negative) : %.2f",
				   adfGeoTransform[5]);
	}
	
	
	// top left corner
	wxPoint2DDouble mytopleft;
	mytopleft.m_x = adfGeoTransform[0] + adfGeoTransform[1] * 0.0 + adfGeoTransform[2] * 0.0;
	mytopleft.m_y = adfGeoTransform[3] + adfGeoTransform[4] * 0.0 + adfGeoTransform[5] * 0.0;
	
	// bottom left corner
	wxPoint2DDouble mybottomleft;
	mybottomleft.m_x = adfGeoTransform[0] + adfGeoTransform[1] * 0.0 + adfGeoTransform[2] * m_Dataset->GetRasterYSize();
	mybottomleft.m_y = adfGeoTransform[3] + adfGeoTransform[4] * 0.0 + adfGeoTransform[5] * m_Dataset->GetRasterYSize();
	
	
	// top right corner
	wxPoint2DDouble mytopright;
	mytopright.m_x = adfGeoTransform[0] + adfGeoTransform[1] * m_Dataset->GetRasterXSize() + adfGeoTransform[2] * 0.0;
	mytopright.m_y = adfGeoTransform[3] + adfGeoTransform[4] * m_Dataset->GetRasterXSize() + adfGeoTransform[5] * 0.0;
	
	// bottom right corner
	wxPoint2DDouble mybottomright;
	mybottomright.m_x = adfGeoTransform[0] + adfGeoTransform[1] * m_Dataset->GetRasterXSize() + adfGeoTransform[2] * m_Dataset->GetRasterYSize();
	mybottomright.m_y = adfGeoTransform[3] + adfGeoTransform[4] * m_Dataset->GetRasterXSize() + adfGeoTransform[5] * m_Dataset->GetRasterYSize();
	
	// compute extent
	// left
	double myleft = mytopleft.m_x;
	if (mybottomleft.m_x < mytopleft.m_x) {
		myleft = mybottomleft.m_x;
	}
	rect.SetLeft(myleft);
	
	// right
	double myright = mytopright.m_x;
	if (mybottomright.m_x > mytopright.m_x) {
		myright = mybottomright.m_x;
	}
	rect.SetRight(myright);
	
	// top
	double mytop = mytopleft.m_y;
	if (mytopright.m_y > mytopleft.m_y) {
		mytop = mytopright.m_y;
	}
	rect.SetTop(mytop);
	
	// bottom
	double mybottom = mybottomleft.m_y;
	if (mybottomright.m_y < mybottomleft.m_y) {
		mybottom = mybottomright.m_y;
	}
	rect.SetBottom(mybottom);
	
	
	
	if (adfGeoTransform[2] != 0 || adfGeoTransform[4] != 0) {
		wxLogWarning("Layer %s contain following rotation informations (%.4f, %.4f). It may not be displayed correctly",
					 m_FileName.GetFullName(),
					 adfGeoTransform[2],
					 adfGeoTransform[4]);
	}
	
	return true;
	
}




bool vrLayerRasterGDAL::GetData(wxImage * bmp, const vrRealRect & coord,
								const vrRender * render, const vrLabel * label) {
	
	// TODO: Add code for getting raster data into bitmap.
	// there is now way to grab data with real coordinates. Thus we need to convert
	// real data to row and cols to fetch from data.
	
	//FIXME: remove this temp code used only to draw something into a bitmap
	wxInitAllImageHandlers();

	wxImage myImgTwo ("D:\\LS\\PRJ\\COLTOPGIS\\test-data\\two.png", wxBITMAP_TYPE_PNG);
	if (myImgTwo.IsOk()==false) {
		return false;
	}
	
	wxBitmap myBmpTwo (myImgTwo);
	wxPoint myImgPosition (50,50);
	
	wxBitmap myBmp(bmp->GetSize());
	wxMemoryDC dc (myBmp);
	
	dc.Clear();
	dc.SetBrush(*wxWHITE_BRUSH);
	dc.SetPen(*wxWHITE_PEN);
	dc.DrawRectangle(0,0,myBmp.GetWidth(), myBmp.GetHeight());
	
	//dc.SetPen(*wxBLUE_PEN);
	//dc.DrawLine(myBmp.GetWidth(),0,0, myBmp.GetHeight());
	dc.DrawBitmap(myBmpTwo, myImgPosition.x, myImgPosition.y, false);
	
	dc.SelectObject(wxNullBitmap);
	*bmp = myBmp.ConvertToImage();
	
	
	// set image transparency. Image is fully transparent, except for pixels where bitmap 
	// was drawn. Those are set the the user defined transparency
	char myBackgroundTransparency = 0;
	int myUserTransparency = 255 - (render->GetTransparency() * 255 / 100);
	
	
	unsigned char * alphachar = NULL;
	unsigned int myimglen = bmp->GetWidth() * bmp->GetHeight();
	alphachar= (unsigned char*)malloc(myimglen);
	if (alphachar == NULL)
	{
		wxLogError(_T("Error creating translucency"));
		return false;
	}
	
	
	for ( unsigned int i = 0; i< myimglen; i++) {
		*(alphachar + i) =  myBackgroundTransparency;
	}
	if (bmp->HasAlpha() == false){
		bmp->InitAlpha();
		wxLogMessage("Initing alpha");
	}
	bmp->SetAlpha(alphachar, false);
	
	for (int x = myImgPosition.x; x< myImgTwo.GetWidth() + myImgPosition.x ; x++) {
		for (int y = myImgPosition.y; y < myImgTwo.GetHeight() + myImgPosition.y ; y++) {
			bmp->SetAlpha(x, y, (char) myUserTransparency);
		}
	}
	
	
	return true;
}


