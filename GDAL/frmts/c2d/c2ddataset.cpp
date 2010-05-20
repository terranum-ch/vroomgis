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
struct C2DInfo {
	const int m_Version;
	int m_Width;
	int m_Height;
	
	C2DInfo():m_Version(1){
		m_Width = -1;
		m_Height = -1;
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
	
    int         bGeoTransformValid;
    double      adfGeoTransform[6];
	
	static int  SaveAsCRLF(char **papszStrList, const char *pszFname);

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
    bGeoTransformValid = FALSE;
    adfGeoTransform[0] = 0.0;
    adfGeoTransform[1] = 1.0;
    adfGeoTransform[2] = 0.0;
    adfGeoTransform[3] = 0.0;
    adfGeoTransform[4] = 0.0;
    adfGeoTransform[5] = 1.0;
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

int C2DDataset::Identify( GDALOpenInfo * poOpenInfo ){
	
	CPLError( CE_Warning, CPLE_NotSupported, "Entering identify function" );
	
	// -------------------------------------------------------------------- 
	//      Verify that this is a c2d file.					
	// -------------------------------------------------------------------- 
    if( poOpenInfo->nHeaderBytes < 20 || poOpenInfo->fp == NULL ){
		CPLError( CE_Warning, CPLE_NotSupported, "This is not a c2d file" );
		return FALSE;
	}
	
	FILE	*fp;
    fp = VSIFOpenL( poOpenInfo->pszFilename, "r" );
    if( fp == NULL )
    {
		CPLError( CE_Warning, CPLE_NotSupported,  "Unable to open the c2d file" );
        return FALSE;
    }
	
	VSIFSeekL(fp, SEEK_SET, 0);
	int myLength = strlen(C2DMagicName);
	char * myChar = new char[myLength];
	int iReadedB = VSIFReadL(myChar, sizeof(char), sizeof(C2DMagicName), fp);
	if (iReadedB != sizeof(C2DMagicName) || strcmp(myChar, C2DMagicName) != 0) {
		CPLError( CE_Warning, CPLE_NotSupported, 
				 "This is not a c2d file : %d bytes readed and magic number is %s",
				 iReadedB,
				 myChar);
		return FALSE;
	}
	
	CPLError( CE_Warning, CPLE_NotSupported, 
			 "Readed magic number OK : %s",myChar);
	
	VSIFCloseL(fp);
	delete [] myChar;

	CPLError( CE_Warning, CPLE_NotSupported,  "leaving identify function" );
    return TRUE;
}

/************************************************************************/
/*                                Open()                                */
/************************************************************************/

GDALDataset *C2DDataset::Open( GDALOpenInfo * poOpenInfo ){
	
	
	CPLError( CE_Warning, CPLE_NotSupported, 
			 "Entering opening function" );
	
	/* -------------------------------------------------------------------- */
	/*      Verify that this is a _raw_ ppm or pgm file.  Note, we don't    */
	/*      support ascii files, or pbm (1bit) files.                       */
	/* -------------------------------------------------------------------- */
    if( !Identify( poOpenInfo ) )
	{
        return NULL;
	}
	
	// creating dataset
	C2DDataset  *poDS;
	CPLError( CE_Warning, CPLE_NotSupported, "Creating dataset");
	poDS = new C2DDataset();
	CPLError( CE_Warning, CPLE_NotSupported, "Dataset created");
	
	/* -------------------------------------------------------------------- */
	/*      Assume ownership of the file handled from the GDALOpenInfo.     */
	/* -------------------------------------------------------------------- */
	VSIFClose(poOpenInfo->fp );
    poOpenInfo->fp = NULL;
	CPLError( CE_Warning, CPLE_NotSupported, "Closing file OK");
	
	
	/* -------------------------------------------------------------------- */
	//      Parse the header, just after the magic number                      
	/* -------------------------------------------------------------------- */
 	// TODO : open the file in poDS->fpImage
	
	FILE	*fp;
    fp = VSIFOpenL( poOpenInfo->pszFilename, "r" );
	CPLError( CE_Warning, CPLE_OpenFailed,"Taking ownership of %s",
			 poOpenInfo->pszFilename );
    if( fp == NULL )
    {
		CPLError( CE_Warning, CPLE_NotSupported,  "Unable to open the c2d file" );
        return NULL;
    }
	VSIFSeekL(fp, sizeof(C2DMagicName), SEEK_SET);
	C2DInfo * myInfo = new C2DInfo();

	CPLError( CE_Warning, CPLE_OpenFailed,"File name is  %s",
			 poOpenInfo->pszFilename );
	
	int iReadedB = VSIFReadL(myInfo, sizeof(C2DInfo), sizeof(myInfo), fp);
	
	CPLError( CE_Warning, CPLE_OpenFailed,"File name is still  %s",
			 poOpenInfo->pszFilename );
	
	if (iReadedB != sizeof(myInfo)){
		CPLError( CE_Warning, CPLE_NotSupported, 
				 "Corrupted C2d header : %d bytes readed instead of %ld",
				 iReadedB, sizeof(myInfo));
		return NULL;
	}

	// No more info here
	CPLError( CE_Warning, CPLE_NotSupported, 
			 "Raster informations : %d , %d, %d",myInfo->m_Version, myInfo->m_Width, myInfo->m_Height);
	VSIFCloseL(fp);
	
	CPLError( CE_Warning, CPLE_NotSupported, "Reading header OK");


	

	//CPLError( CE_Warning, CPLE_NotSupported, "Reading header OK and hello ");
	/* -------------------------------------------------------------------- */
	/*      Confirm the requested access is supported.                      */
	/* -------------------------------------------------------------------- */
   /* if( poOpenInfo->eAccess == GA_Update )
    {
        CPLError( CE_Failure, CPLE_NotSupported, 
				 "The C2D driver does not support update access to existing"
				 " datasets.\n" );
        return NULL;
    }*/
	
	/* -------------------------------------------------------------------- */
	/*      Create a corresponding GDALDataset.                             */
	/* -------------------------------------------------------------------- */
    /*C2DDataset  *poDS;
	CPLError( CE_Warning, CPLE_NotSupported, "Reading header OK and hello ");
	poDS = new C2DDataset();
	CPLError( CE_Warning, CPLE_NotSupported, "Creating Dataset OK");
	//CPLError( CE_Warning, CPLE_NotSupported, "Hello you");
	//return NULL;
	CPLError( CE_Warning, CPLE_NotSupported, "Hello you");*/
	

	
	/* -------------------------------------------------------------------- */
	/*      Capture some information from the file that is of interest.     */
	/* -------------------------------------------------------------------- */
    int nWidth = myInfo->m_Width;
	int nHeight = myInfo->m_Height;
	int iPixelSize = 1;
	GDALDataType eDataType = GDT_Float32;
	int iIn = 2;
	
	poDS->nRasterXSize = nWidth;
    poDS->nRasterYSize = nHeight;
	
	CPLError( CE_Warning, CPLE_NotSupported, "Hello you2");

	//return NULL;
	
	if( poOpenInfo->eAccess == GA_Update ){
		poDS->fpImage = VSIFOpenL(  poOpenInfo->pszFilename, "rb+" );
		CPLError( CE_Warning, CPLE_NotSupported, "opening rb+ OK");
	}
	else{
		poDS->fpImage = VSIFOpenL(  poOpenInfo->pszFilename, "rb" );
		CPLError( CE_Warning, CPLE_NotSupported, "opening rb only OK");
	}
	if( poDS->fpImage == NULL )
    {
        CPLError( CE_Failure, CPLE_OpenFailed,
				 "Failed to re-open %s within C2D driver.\n",
				 poOpenInfo->pszFilename );
        return NULL;
    }
	 poDS->eAccess = poOpenInfo->eAccess;
	
	CPLError( CE_Warning, CPLE_NotSupported, "Reopening file OK");

	
	/* -------------------------------------------------------------------- */
	/*      Create band information objects.                                */
	/* -------------------------------------------------------------------- */
    int         bMSBFirst = TRUE;
    
	
#ifdef CPL_LSB
    bMSBFirst = FALSE;
#endif
	
	poDS->SetBand(
				  1, new RawRasterBand( poDS, 1, poDS->fpImage, iIn, 3*iPixelSize,
									   nWidth*3*iPixelSize, eDataType, bMSBFirst, TRUE ));
	CPLError( CE_Warning, CPLE_NotSupported, "Setting band 1 OK");
	
	poDS->SetBand(
				  2, new RawRasterBand( poDS, 2, poDS->fpImage, iIn+iPixelSize,
									   3*iPixelSize, nWidth*3*iPixelSize,
									   eDataType, bMSBFirst, TRUE ));
	CPLError( CE_Warning, CPLE_NotSupported, "Setting band 2 OK");
	poDS->SetBand(
				  3, new RawRasterBand( poDS, 3, poDS->fpImage, iIn+2*iPixelSize,
									   3*iPixelSize, nWidth*3*iPixelSize,
									   eDataType, bMSBFirst, TRUE ));
	CPLError( CE_Warning, CPLE_NotSupported, "Setting band 3 OK");
	
	poDS->GetRasterBand(1)->SetColorInterpretation( GCI_RedBand );
	poDS->GetRasterBand(2)->SetColorInterpretation( GCI_GreenBand );
	poDS->GetRasterBand(3)->SetColorInterpretation( GCI_BlueBand );
	
	CPLError( CE_Warning, CPLE_NotSupported, "Getting col interpretation OK");
	
	/* -------------------------------------------------------------------- */
	/*      Check for world file.                                           */
	/* -------------------------------------------------------------------- */
	//poDS->bGeoTransformValid = 	GDALReadWorldFile( poOpenInfo->pszFilename, ".wld", 
	//				  poDS->adfGeoTransform );
	
	/* -------------------------------------------------------------------- */
	/*      Initialize any PAM information.                                 */
	/* -------------------------------------------------------------------- */
	poDS->SetDescription( poOpenInfo->pszFilename );
	//poDS->TryLoadXML();
	
	/* -------------------------------------------------------------------- */
	/*      Check for overviews.                                            */
	/* -------------------------------------------------------------------- */
	//poDS->oOvManager.Initialize( poDS, poOpenInfo->pszFilename );
	
	return( poDS );
}

/************************************************************************/
/*                               CSLSaveCRLF()                          */
/************************************************************************/

/***
 * Write a stringlist to a CR + LF terminated text file.
 *
 * Returns the number of lines written, or 0 if the file could not 
 * be written.
 */

int C2DDataset::SaveAsCRLF(char **papszStrList, const char *pszFname)
{
    FILE    *fp;
    int     nLines = 0;
	
    if (papszStrList)
    {
        if ((fp = VSIFOpenL(pszFname, "wt")) != NULL)
        {
            while(*papszStrList != NULL)
            {
                if( VSIFPrintfL( fp, "%s\r\n", *papszStrList ) < 1 )
                {
                    CPLError( CE_Failure, CPLE_FileIO,
							 "CSLSaveCRLF(\"%s\") failed: unable to write to output file.",
							 pszFname );
                    break;
                }
				
                nLines++;
                papszStrList++;
            }
			
            VSIFCloseL(fp);
        }
        else
        {
            CPLError( CE_Failure, CPLE_OpenFailed,
					 "CSLSaveCRLF(\"%s\") failed: unable to open output file.",
					 pszFname );
        }
    }
	
    return nLines;
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
	
	// -------------------------------------------------------------------- 
	//      Create the dataset.                                             
	// --------------------------------------------------------------------
	FILE * fpImage =VSIFOpenL( pszFilename, "wb" );
    if( fpImage == NULL )
    {
        CPLError( CE_Failure, CPLE_OpenFailed, "Unable to create c2d file %s.\n", 
				 pszFilename );
        return NULL;
    }
	
	
	// -------------------------------------------------------------------- 
	//      Write header informations                                            
	// --------------------------------------------------------------------
	VSIFSeekL(fpImage, 0, SEEK_SET); // begining of file
	VSIFWriteL(C2DMagicName, sizeof(char), sizeof(C2DMagicName), fpImage);
	C2DInfo * myRasterInfo = new C2DInfo();
	
	CPLError( CE_Warning, CPLE_NotSupported, 
			 "Sizeof MyRasterInfo - %d", sizeof(myRasterInfo));
	
	myRasterInfo->m_Width = poSrcDS->GetRasterBand(1)->GetXSize();
	myRasterInfo->m_Height = poSrcDS->GetRasterBand(1)->GetYSize();
	
	CPLError( CE_Warning, CPLE_NotSupported, 
			 "Sizeof MyRasterInfo - %d", sizeof(myRasterInfo));
	
	int sizewrited = VSIFWriteL(myRasterInfo, sizeof(C2DInfo), sizeof(myRasterInfo), fpImage);
	
	CPLError( CE_Warning, CPLE_NotSupported, 
			 "Write ok : %d bytes - %d", sizewrited, sizeof(myRasterInfo));

	VSIFCloseL(fpImage);
	CPLError( CE_Warning, CPLE_NotSupported, "Create header passed" );
	
	
	int nWidth = myRasterInfo->m_Width;
	int nHeight = myRasterInfo->m_Height;
	
	delete myRasterInfo;
	myRasterInfo = NULL;
	
	
	C2DDataset * poDS = (C2DDataset*) GDALOpen( pszFilename, GA_Update);
	if (poDS == NULL) {
		CPLError( CE_Warning, CPLE_NotSupported, "Error opening C2DDataset in CreateCopy function" );
		return NULL;
	}
	
	
	// -------------------------------------------------------------------- 
	//      Write bands informations                                            
	// --------------------------------------------------------------------
	int  	nBlockXSize, nBlockYSize, nBlockTotal, nBlocksDone;
	
    poDS->GetRasterBand(1)->GetBlockSize( &nBlockXSize, &nBlockYSize );
	
    nBlockTotal = ((nWidth + nBlockXSize - 1) / nBlockXSize)
	* ((nHeight + nBlockYSize - 1) / nBlockYSize);
    nBlocksDone = 0;
	
	CPLError( CE_Warning, CPLE_NotSupported,  "%d Block to write", nBlockTotal);
	
	//
	// COPYING BAND 1 AKA MNT DATA
	//
	GDALRasterBand *poSrcBand = poSrcDS->GetRasterBand( 1 );
	GDALRasterBand *poDstBand = poDS->GetRasterBand( 1 );
	if (poDstBand == NULL) {
		 CPLError( CE_Warning, CPLE_NotSupported, "No raster band 1 for C2D file" );
	}
	else {
		CPLError( CE_Warning, CPLE_NotSupported, "raster band 1 is present for C2D file" );
	}

	
	
	int	       iYOffset, iXOffset;
	void           *pData;
	CPLErr  eErr;
	int pbSuccess;
	double dfSrcNoDataValue =0.0;
	
	/* Get nodata value, if relevant */
	dfSrcNoDataValue = poSrcBand->GetNoDataValue( &pbSuccess );
	if ( pbSuccess ){
  	    CPLError( CE_Warning, CPLE_NotSupported, "Input raster has NoData values" );
	}
	
	pData = CPLMalloc(nBlockXSize * nBlockYSize * GDALGetDataTypeSize(eType) / 8);
	
	for( iYOffset = 0; iYOffset < nHeight; iYOffset += nBlockYSize )
	{
		for( iXOffset = 0; iXOffset < nWidth; iXOffset += nBlockXSize )
		{
			int	nTBXSize, nTBYSize;
			
			if( !pfnProgress( (nBlocksDone++) / (float) nBlockTotal,
							 NULL, pProgressData ) ){
				CPLError( CE_Failure, CPLE_UserInterrupt,"User terminated" );
				delete poDS;
			}
			
			nTBXSize = MIN(nBlockXSize,nWidth-iXOffset);
			nTBYSize = MIN(nBlockYSize, nHeight-iYOffset);
			
			eErr = poSrcBand->RasterIO( GF_Read, 
									   iXOffset, iYOffset, 
									   nTBXSize, nTBYSize,
									   pData, nTBXSize, nTBYSize,
									   eType, 0, 0 );
			if( eErr != CE_None )
			{
				CPLError( CE_Failure, CPLE_UserInterrupt,"Error reading data @ iteration : %d / %d",
						 iYOffset, iXOffset);
				return NULL;
			}
            
			eErr = poDstBand->RasterIO( GF_Write, 
									   iXOffset, iYOffset, 
									   nTBXSize, nTBYSize,
									   pData, nTBXSize, nTBYSize,
									   eType, 0, 0 );
			
			if( eErr != CE_None )
			{
				CPLError( CE_Failure, CPLE_UserInterrupt,"Error writing data @ iteration : %d / %d",
						 iYOffset, iXOffset);
				return NULL;
			}
		}
	}

    
	CPLError( CE_Warning, CPLE_NotSupported, 
			 "Create copy passed" );
	
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
