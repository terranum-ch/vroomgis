/******************************************************************************
 * Project:  C2D Coltop2D raster
 * Purpose:  All code for Coltop2D composite rater
 * Author:   Lucien Schreiber <lucien.schreiber@crealp.vs.ch>
 *
 ******************************************************************************
 * Copyright (c) 2010, Lucien Schreiber <lucien.schreiber@crealp.vs.ch>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ****************************************************************************/


#include "../raw/rawdataset.h"
#include "cpl_string.h"
#include <ctype.h>


// ------------------------------------------------------------------------
// Constant for C2D formats
// ------------------------------------------------------------------------
const char C2DMagicName[] = "C2D";
class C2DInfo {
public:
	const int m_Version;
	int m_Width;
	int m_Height;
	
	C2DInfo():m_Version(1){
		m_Width = -1;
		m_Height = -1;
	}
	
	C2DInfo & operator = (const C2DInfo & other){
		if (this != &other) {
			m_Width = other.m_Width;
			m_Height = other.m_Height;
		}
		return *this;
	}
	
};


CPL_C_START
void    GDALRegister_C2D(void);
CPL_C_END

/************************************************************************/
/* ==================================================================== */
/*                              C2DDataset                              */
/* ==================================================================== */
/************************************************************************/

class C2DDataset : public RawDataset
{
    FILE        *fpImage;       // image data file.
	
	static bool		ReadMagicNumber(GDALOpenInfo * poOpenInfo);
	static bool		WriteMagicNumber(const char * pszFilename);
	static bool		WriteHeader(const char * pszFilename, const C2DInfo & info);
	static bool		ReadHeader(const char * pszFilename, C2DInfo & info);

	
public:
	C2DDataset();
	~C2DDataset();
	
    virtual CPLErr GetGeoTransform( double * );
	
    static int          Identify( GDALOpenInfo * );
    static GDALDataset *Open( GDALOpenInfo * );
	static GDALDataset *CreateCopy( const char * pszFilename, 
								   GDALDataset *poSrcDS, 
								   int bStrict, char ** papszOptions, 
								   GDALProgressFunc pfnProgress, 
								   void * pProgressData );
};

/************************************************************************/
/*                            C2DDataset()                             */
/************************************************************************/

C2DDataset::C2DDataset()
{
    fpImage = NULL;
}

/************************************************************************/
/*                            ~C2DDataset()                            */
/************************************************************************/

C2DDataset::~C2DDataset()

{
    FlushCache();
    if( fpImage != NULL )
        VSIFCloseL( fpImage );
}

/************************************************************************/
/*                          GetGeoTransform()                           */
/************************************************************************/

CPLErr C2DDataset::GetGeoTransform( double * padfTransform )

{
	// TODO: Implement this function
	return CE_None;
	
	
    /*if( bGeoTransformValid )
    {
        memcpy( padfTransform, adfGeoTransform, sizeof(double)*6 );
        return CE_None;
    }
    else
        return CE_Failure;*/
}

/************************************************************************/
/*                              Identify()                              */
/************************************************************************/

int C2DDataset::Identify( GDALOpenInfo * poOpenInfo ){

	// Some bytes availlable
	if( poOpenInfo->nHeaderBytes < 3 || poOpenInfo->fp == NULL ){
		CPLError( CE_Warning, CPLE_NotSupported, "This %s is not a c2d file",
				 poOpenInfo->pszFilename);
		return FALSE;
	}
	
	// is magic number present ?
    return ReadMagicNumber(poOpenInfo);
}



bool C2DDataset::ReadMagicNumber(GDALOpenInfo * poOpenInfo){
	FILE	*fp = NULL;
    fp = VSIFOpenL( poOpenInfo->pszFilename, "r" );
    if( fp == NULL )
    {
		CPLError( CE_Warning, CPLE_NotSupported,  "Unable to open the %s c2d file",
				 poOpenInfo->pszFilename);
        return FALSE;
    }
	
	int myLength = strlen(C2DMagicName);
	char * myChar = new char[myLength];
	int iReadedB = VSIFReadL(myChar, sizeof(char), sizeof(C2DMagicName), fp);
	if (iReadedB != sizeof(C2DMagicName) || strcmp(myChar, C2DMagicName) != 0) {
		CPLError( CE_Warning, CPLE_NotSupported, 
				 "This is not a c2d file : %d bytes readed and magic number is %s",
				 iReadedB,
				 myChar);
		delete [] myChar;
		return FALSE;
	}
	
	CPLError( CE_Warning, CPLE_NotSupported, 
			 "Readed magic number OK : %s",myChar);
	
	VSIFCloseL(fp);
	delete [] myChar;
	return TRUE;
}




bool C2DDataset::WriteMagicNumber(const char * pszFilename){
	FILE * fpImage =VSIFOpenL( pszFilename, "wb" );
	if (fpImage == NULL) {
		CPLError( CE_Warning, CPLE_NotSupported, 
				 "Opening file %s for Writing magic number failed",
				 pszFilename);
		return FALSE;
	}
	VSIFWriteL(C2DMagicName, sizeof(char), sizeof(C2DMagicName), fpImage);
	VSIFCloseL(fpImage);
	return TRUE;
}



bool C2DDataset::WriteHeader(const char * pszFilename, const C2DInfo & info){
	FILE * fpImage =VSIFOpenL( pszFilename, "ab" );
	if (fpImage == NULL) {
		CPLError( CE_Warning, CPLE_NotSupported, 
				 "Opening file %s for Writing header failed",
				 pszFilename);
		return FALSE;
	}
	
	C2DInfo * myInfo = new C2DInfo(info);
	VSIFWriteL(myInfo, sizeof(C2DInfo), 1, fpImage);
	VSIFCloseL(fpImage);
	delete myInfo;
	return TRUE;
}


bool C2DDataset::ReadHeader(const char * pszFilename, C2DInfo & info){
	FILE	*fp = NULL;
    fp = VSIFOpenL( pszFilename, "r" );
    if( fp == NULL )
    {
		CPLError( CE_Warning, CPLE_NotSupported,  "Unable to open the %s c2d file",
				 pszFilename);
        return FALSE;
    }
	
	C2DInfo * pInfo = new C2DInfo();
	VSIFSeekL(fp, sizeof(C2DMagicName), SEEK_SET);
	int iReadedB = VSIFReadL(pInfo, sizeof(C2DInfo), 1, fp);
	
	CPLError( CE_Warning, CPLE_NotSupported,  "Readed header partially OK : %d bytes",
			 iReadedB);
	
		
	// ensure version is supported
	if (pInfo->m_Version != info.m_Version) {
		CPLError( CE_Failure, CPLE_NotSupported,  "Unable to open, driver version conflit (found %d, expected %d)",
				 pInfo->m_Version, info.m_Version);
	}
	
	info = *pInfo;
	delete pInfo;
	VSIFCloseL(fp);
	
	CPLError( CE_Warning, CPLE_NotSupported, "Readed header ok, file size is %d, %d",
			 info.m_Width, info.m_Height);
	
	return TRUE;
}



/************************************************************************/
/*                                Open()                                */
/************************************************************************/

GDALDataset *C2DDataset::Open( GDALOpenInfo * poOpenInfo ){
	
	if( !Identify( poOpenInfo ) ){
        return NULL;
	}
	CPLError( CE_Warning, CPLE_NotSupported, "Entering opening function" );
	
	C2DInfo myRasterInfo;
	if (ReadHeader(poOpenInfo->pszFilename, myRasterInfo)==FALSE) {
		CPLError( CE_Warning, CPLE_NotSupported, "Unable to read header for %s",
				 poOpenInfo->pszFilename);
		return NULL;		
	}
	

	C2DDataset * poDS;
	FILE *fpImage = VSIFOpenL(poOpenInfo->pszFilename, "rb");
	if (fpImage == NULL) {
		CPLError( CE_Warning, CPLE_NotSupported, "Unable to open %s file",
				 poOpenInfo->pszFilename);
		return NULL;
	}
	
	poDS = new C2DDataset();
	poDS->fpImage = fpImage;
	
	
	poDS->SetDescription( poOpenInfo->pszFilename );
    poDS->TryLoadXML();
	return (poDS);
}






/************************************************************************/
/*                             CreateCopy()                             */
/************************************************************************/
GDALDataset *
C2DDataset::CreateCopy( const char * pszFilename, GDALDataset *poSrcDS, 
					   int bStrict, char ** papszOptions, 
					   GDALProgressFunc pfnProgress, void * pProgressData )

{
	CPLError( CE_Warning, CPLE_NotSupported,"Entering create copy function" );
	
	int nBands = poSrcDS->GetRasterCount();
	if (nBands != 1){
        CPLError( CE_Failure, CPLE_NotSupported,
				 "C2D driver does not support source dataset with less or more than one band.\n");
        return NULL;
    }
	
	
	GDALDataType eType = poSrcDS->GetRasterBand(1)->GetRasterDataType();
	if( !pfnProgress( 0.0, NULL, pProgressData ) ){
      return NULL;
	}
	
	// Write magic number
	if (WriteMagicNumber(pszFilename) == FALSE) {
		return NULL;
	}
	CPLError( CE_Warning, CPLE_NotSupported, "Writing magic number passed" );

	// Write header
	C2DInfo mySrcRasterInfo;
	mySrcRasterInfo.m_Width = poSrcDS->GetRasterBand(1)->GetXSize();
	mySrcRasterInfo.m_Height = poSrcDS->GetRasterBand(1)->GetYSize();
	if (WriteHeader(pszFilename, mySrcRasterInfo) == FALSE) {
		return NULL;
	}
	
    
	CPLError( CE_Warning, CPLE_NotSupported, "Create copy passed" );
	
	return (GDALDataset *) GDALOpen( pszFilename, GA_ReadOnly );
}



/************************************************************************/
/*                          GDALRegister_C2D()                          */
/************************************************************************/

void GDALRegister_C2D()

{
    GDALDriver	*poDriver;

    if( GDALGetDriverByName( "C2D" ) == NULL )
    {
        poDriver = new GDALDriver();
        
        poDriver->SetDescription( "C2D" );
        poDriver->SetMetadataItem( GDAL_DMD_LONGNAME, 
                                   "Coltop 2D files (.c2d)" );
        poDriver->SetMetadataItem( GDAL_DMD_HELPTOPIC, 
                                   "frmt_various.html#C2D" );
        poDriver->SetMetadataItem( GDAL_DMD_EXTENSION, "c2d" );

        poDriver->pfnOpen = C2DDataset::Open;
		poDriver->pfnCreateCopy = C2DDataset::CreateCopy;
		//poDriver->pfnCreateCopy = C2DCreateCopy;
		poDriver->pfnIdentify = C2DDataset::Identify;

		
		
        GetGDALDriverManager()->RegisterDriver( poDriver );
    }
}
