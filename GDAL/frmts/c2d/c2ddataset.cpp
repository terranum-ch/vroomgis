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
	
    int         bGeoTransformValid;
    double      adfGeoTransform[6];
	
public:
	//C2DDataset();
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

/*C2DDataset::C2DDataset()
{
    fpImage = NULL;
    bGeoTransformValid = FALSE;
    adfGeoTransform[0] = 0.0;
    adfGeoTransform[1] = 1.0;
    adfGeoTransform[2] = 0.0;
    adfGeoTransform[3] = 0.0;
    adfGeoTransform[4] = 0.0;
    adfGeoTransform[5] = 1.0;
}*/

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
	
    if( bGeoTransformValid )
    {
        memcpy( padfTransform, adfGeoTransform, sizeof(double)*6 );
        return CE_None;
    }
    else
        return CE_Failure;
}

/************************************************************************/
/*                              Identify()                              */
/************************************************************************/

int C2DDataset::Identify( GDALOpenInfo * poOpenInfo )

{
	/* -------------------------------------------------------------------- */
	/*      Verify that this is a _raw_ ppm or pgm file.  Note, we don't    */
	/*      support ascii files, or pbm (1bit) files.                       */
	/* -------------------------------------------------------------------- */
    if( poOpenInfo->nHeaderBytes < 10 || poOpenInfo->fp == NULL )
        return FALSE;
	
    if( poOpenInfo->pabyHeader[0] != 'P'  ||
	   (poOpenInfo->pabyHeader[2] != ' '  &&    // XXX: Magick number
		poOpenInfo->pabyHeader[2] != '\t' &&    // may be followed
		poOpenInfo->pabyHeader[2] != '\n' &&    // any of the blank
		poOpenInfo->pabyHeader[2] != '\r') )    // characters
        return FALSE;
	
    if( poOpenInfo->pabyHeader[1] != '5'
	   && poOpenInfo->pabyHeader[1] != '6' )
        return FALSE;
	
    return TRUE;
}

/************************************************************************/
/*                                Open()                                */
/************************************************************************/

GDALDataset *C2DDataset::Open( GDALOpenInfo * poOpenInfo )

{
	/* -------------------------------------------------------------------- */
	/*      Verify that this is a _raw_ ppm or pgm file.  Note, we don't    */
	/*      support ascii files, or pbm (1bit) files.                       */
	/* -------------------------------------------------------------------- */
    if( !Identify( poOpenInfo ) )
        return NULL;
	
	/* -------------------------------------------------------------------- */
	/*      Parse out the tokens from the header.                           */
	/* -------------------------------------------------------------------- */
    const char  *pszSrc = (const char *) poOpenInfo->pabyHeader;
    char        szToken[512];
    int         iIn, iToken = 0, nWidth =-1, nHeight=-1, nMaxValue=-1;
    unsigned int iOut;
	
    iIn = 2;
    while( iIn < poOpenInfo->nHeaderBytes && iToken < 3 )
    {
        iOut = 0;
        szToken[0] = '\0';
        while( iOut < sizeof(szToken) && iIn < poOpenInfo->nHeaderBytes )
        {
            if( pszSrc[iIn] == '#' )
            {
                while( pszSrc[iIn] != 10 && pszSrc[iIn] != 13
					  && iIn < poOpenInfo->nHeaderBytes - 1 )
                    iIn++;
            }
			
            if( iOut != 0 && isspace((unsigned char)pszSrc[iIn]) )
            {
                szToken[iOut] = '\0';
				
                if( iToken == 0 )
                    nWidth = atoi(szToken);
                else if( iToken == 1 )
                    nHeight = atoi(szToken);
                else if( iToken == 2 )
                    nMaxValue = atoi(szToken);
				
                iToken++;
                iIn++;
                break;
            }
			
            else if( !isspace((unsigned char)pszSrc[iIn]) )
            {
                szToken[iOut++] = pszSrc[iIn];
            }
			
            iIn++;
        }
    }
	
    CPLDebug( "C2D", "C2D header contains: width=%d, height=%d, maxval=%d",
			 nWidth, nHeight, nMaxValue );
	
    if( iToken != 3 || nWidth < 1 || nHeight < 1 || nMaxValue < 1 )
        return NULL;
	
	/* -------------------------------------------------------------------- */
	/*      Create a corresponding GDALDataset.                             */
	/* -------------------------------------------------------------------- */
    C2DDataset  *poDS;
	
    poDS = new C2DDataset();
	
	/* -------------------------------------------------------------------- */
	/*      Capture some information from the file that is of interest.     */
	/* -------------------------------------------------------------------- */
    poDS->nRasterXSize = nWidth;
    poDS->nRasterYSize = nHeight;
	
	/* -------------------------------------------------------------------- */
	/*      Assume ownership of the file handled from the GDALOpenInfo.     */
	/* -------------------------------------------------------------------- */
    VSIFClose( poOpenInfo->fp );
    poOpenInfo->fp = NULL;
	
    if( poOpenInfo->eAccess == GA_Update )
        poDS->fpImage = VSIFOpenL( poOpenInfo->pszFilename, "rb+" );
    else
        poDS->fpImage = VSIFOpenL( poOpenInfo->pszFilename, "rb" );
	
    if( poDS->fpImage == NULL )
    {
        CPLError( CE_Failure, CPLE_OpenFailed,
				 "Failed to re-open %s within C2D driver.\n",
				 poOpenInfo->pszFilename );
        return NULL;
    }
	
    poDS->eAccess = poOpenInfo->eAccess;
	
	/* -------------------------------------------------------------------- */
	/*      Create band information objects.                                */
	/* -------------------------------------------------------------------- */
    int         bMSBFirst = TRUE, iPixelSize;
    GDALDataType eDataType;
	
#ifdef CPL_LSB
    bMSBFirst = FALSE;
#endif
	
    if ( nMaxValue < 256 )
        eDataType = GDT_Byte;
    else
        eDataType = GDT_UInt16;
	
    iPixelSize = GDALGetDataTypeSize( eDataType ) / 8;
	
    if( poOpenInfo->pabyHeader[1] == '5' )
    {
        if (nWidth > INT_MAX / iPixelSize)
        {
            CPLError( CE_Failure, CPLE_AppDefined, 
					 "Int overflow occured.");
            delete poDS;
            return NULL;
        }
        poDS->SetBand(
					  1, new RawRasterBand( poDS, 1, poDS->fpImage, iIn, iPixelSize,
										   nWidth*iPixelSize, eDataType, bMSBFirst, TRUE ));
        poDS->GetRasterBand(1)->SetColorInterpretation( GCI_GrayIndex );
    }
    else
    {
        if (nWidth > INT_MAX / (3 * iPixelSize))
        {
            CPLError( CE_Failure, CPLE_AppDefined, 
					 "Int overflow occured.");
            delete poDS;
            return NULL;
        }
        poDS->SetBand(
					  1, new RawRasterBand( poDS, 1, poDS->fpImage, iIn, 3*iPixelSize,
										   nWidth*3*iPixelSize, eDataType, bMSBFirst, TRUE ));
        poDS->SetBand(
					  2, new RawRasterBand( poDS, 2, poDS->fpImage, iIn+iPixelSize,
										   3*iPixelSize, nWidth*3*iPixelSize,
										   eDataType, bMSBFirst, TRUE ));
        poDS->SetBand(
					  3, new RawRasterBand( poDS, 3, poDS->fpImage, iIn+2*iPixelSize,
										   3*iPixelSize, nWidth*3*iPixelSize,
										   eDataType, bMSBFirst, TRUE ));
		
        poDS->GetRasterBand(1)->SetColorInterpretation( GCI_RedBand );
        poDS->GetRasterBand(2)->SetColorInterpretation( GCI_GreenBand );
        poDS->GetRasterBand(3)->SetColorInterpretation( GCI_BlueBand );
    }
	
	/* -------------------------------------------------------------------- */
	/*      Check for world file.                                           */
	/* -------------------------------------------------------------------- */
    poDS->bGeoTransformValid = 
	GDALReadWorldFile( poOpenInfo->pszFilename, ".wld", 
					  poDS->adfGeoTransform );
	
	/* -------------------------------------------------------------------- */
	/*      Initialize any PAM information.                                 */
	/* -------------------------------------------------------------------- */
    poDS->SetDescription( poOpenInfo->pszFilename );
    poDS->TryLoadXML();
	
	/* -------------------------------------------------------------------- */
	/*      Check for overviews.                                            */
	/* -------------------------------------------------------------------- */
    poDS->oOvManager.Initialize( poDS, poOpenInfo->pszFilename );
	
    return( poDS );
}



/************************************************************************/
/*                             CreateCopy()                             */
/************************************************************************/
GDALDataset *
C2DDataset::CreateCopy( const char * pszFilename, GDALDataset *poSrcDS, 
					   int bStrict, char ** papszOptions, 
					   GDALProgressFunc pfnProgress, void * pProgressData )

{
	int nBands = poSrcDS->GetRasterCount();
	if (nBands != 1){
        CPLError( CE_Failure, CPLE_NotSupported, 
				 "C2D driver does not support source dataset with less or more than one band.\n");
        return NULL;
    }
	
	//GDALDataType eType = poSrcDS->GetRasterBand(1)->GetRasterDataType();
	if( !pfnProgress( 0.0, NULL, pProgressData ) ){
        return NULL;
	}
	
	// -------------------------------------------------------------------- 
	//      Create the dataset.                                             
	// --------------------------------------------------------------------
	FILE	*fpImage =VSIFOpen( pszFilename, "wb" );
    if( fpImage == NULL )
    {
        CPLError( CE_Failure, CPLE_OpenFailed, 
				 "Unable to create c2d file %s.\n", 
				 pszFilename );
        return NULL;
    }
	
	//VSIFWrite(
	
	VSIFClose(fpImage);
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
