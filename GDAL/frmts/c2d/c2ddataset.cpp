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
    if( fp == NULL )
    {
		CPLError( CE_Warning, CPLE_NotSupported,  "Unable to open the c2d file" );
        return NULL;
    }
	
	VSIFSeekL(fp, sizeof(C2DMagicName), SEEK_SET);
	C2DInfo myInfo;
	int iReadedB = VSIFReadL(&myInfo, sizeof(C2DInfo), sizeof(myInfo), fp);
	if (iReadedB != sizeof(C2DInfo)){
		CPLError( CE_Warning, CPLE_NotSupported, 
				 "Corrupted C2d header : %d bytes readed instead of %ld",
				 iReadedB, sizeof(C2DInfo));
		return NULL;
	}
	
	
	CPLError( CE_Warning, CPLE_NotSupported, 
			 "Raster informations : %d , %d, %d",myInfo.m_Version, myInfo.m_Width, myInfo.m_Height);
	VSIFCloseL(fp);
	
	CPLError( CE_Warning, CPLE_NotSupported, "Reading header OK");


	
	/* -------------------------------------------------------------------- */
	/*      Confirm the requested access is supported.                      */
	/* -------------------------------------------------------------------- */
    if( poOpenInfo->eAccess == GA_Update )
    {
        CPLError( CE_Failure, CPLE_NotSupported, 
				 "The C2D driver does not support update access to existing"
				 " datasets.\n" );
        return NULL;
    }
	
	/* -------------------------------------------------------------------- */
	/*      Create a corresponding GDALDataset.                             */
	/* -------------------------------------------------------------------- */
    C2DDataset  *poDS;
	poDS = new C2DDataset();
	CPLError( CE_Warning, CPLE_NotSupported, "Creating Dataset OK");

	
	
	/* -------------------------------------------------------------------- */
	/*      Capture some information from the file that is of interest.     */
	/* -------------------------------------------------------------------- */
    int nWidth = myInfo.m_Width;
	int nHeight = myInfo.m_Height;
	int iPixelSize = 1;
	GDALDataType eDataType = GDT_Float32;
	int iIn = 2;
	
	poDS->nRasterXSize = nWidth;
    poDS->nRasterYSize = nHeight;
	
	//CPLError( CE_Warning, CPLE_NotSupported, "Hello you");

	return NULL;
	
	poDS->fpImage = VSIFOpenL( poOpenInfo->pszFilename, "rb+" );
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
	
	/* -------------------------------------------------------------------- */
	/*      Check for world file.                                           */
	/* -------------------------------------------------------------------- */
	//poDS->bGeoTransformValid = 	GDALReadWorldFile( poOpenInfo->pszFilename, ".wld", 
	//				  poDS->adfGeoTransform );
	
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
	
	
	//GDALDataType eType = poSrcDS->GetRasterBand(1)->GetRasterDataType();
	//if( !pfnProgress( 0.0, NULL, pProgressData ) ){
      //  return NULL;
	//}
	
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
	
	VSIFSeekL(fpImage, 0, SEEK_SET); // begining of file
	VSIFWriteL(C2DMagicName, sizeof(char), sizeof(C2DMagicName), fpImage);
	C2DInfo myRasterInfo;
	myRasterInfo.m_Width = poSrcDS->GetRasterBand(1)->GetXSize();
	myRasterInfo.m_Height = poSrcDS->GetRasterBand(1)->GetYSize();
	int sizewrited = VSIFWriteL(&myRasterInfo, sizeof(C2DInfo), sizeof(myRasterInfo), fpImage);
	
	
	/*int sizewrited = VSIFWriteL(mychar, sizeof(char), sizeof(mychar), fpImage);
	if (sizewrited != sizeof(mychar)) {
		CPLError( CE_Warning, CPLE_NotSupported, 
				 "Not the good number of bytes written (%d), sizeof mychar = %ld",
				 sizewrited,
				 sizeof(mychar));
		//return NULL;
	}*/
	
	
	CPLError( CE_Warning, CPLE_NotSupported, 
			 "Write ok : %d bytes", sizewrited);
		
	//VSIFWriteL( (void *) "\0\0", 2, 1, fpImage );	
	VSIFCloseL(fpImage);
    
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
