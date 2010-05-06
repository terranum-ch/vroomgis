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

#include "gdal_pam.h"


CPL_C_START
void	GDALRegister_C2D(void);
CPL_C_END

/************************************************************************/
/*                            C2DGetField()                            */
/************************************************************************/

static int C2DGetField( char *pszField, int nWidth )

{
    char	szWork[32];

    CPLAssert( nWidth < (int) sizeof(szWork) );

    strncpy( szWork, pszField, nWidth );
    szWork[nWidth] = '\0';

    return atoi(szWork);
}

/************************************************************************/
/*                            C2DGetAngle()                            */
/************************************************************************/

static double C2DGetAngle( char *pszField )

{
    int		nAngle = C2DGetField( pszField, 7 );
    int		nDegree, nMin, nSec;

    // Note, this isn't very general purpose, but it would appear
    // from the field widths that angles are never negative.  Nice
    // to be a country in the "first quadrant". 

    nDegree = nAngle / 10000;
    nMin = (nAngle / 100) % 100;
    nSec = nAngle % 100;
    
    return nDegree + nMin / 60.0 + nSec / 3600.0;
}

/************************************************************************/
/* ==================================================================== */
/*				C2DDataset				*/
/* ==================================================================== */
/************************************************************************/

class C2DRasterBand;

class C2DDataset : public GDALPamDataset
{
    friend class C2DRasterBand;
	
    FILE	*fp;
    GByte	abyHeader[1012];
	
public:
	~C2DDataset();
    
    static GDALDataset *Open( GDALOpenInfo * );
	
    CPLErr 	GetGeoTransform( double * padfTransform );
    const char *GetProjectionRef();
};

/************************************************************************/
/* ==================================================================== */
/*                            C2DRasterBand                             */
/* ==================================================================== */
/************************************************************************/

class C2DRasterBand : public GDALPamRasterBand
{
    friend class C2DDataset;

    int          nRecordSize;
    char*        pszRecord;
    
  public:

    		C2DRasterBand( C2DDataset *, int );
                ~C2DRasterBand();
    
    virtual CPLErr IReadBlock( int, int, void * );
};


/************************************************************************/
/*                           C2DRasterBand()                            */
/************************************************************************/

C2DRasterBand::C2DRasterBand( C2DDataset *poDS, int nBand )

{
    this->poDS = poDS;
    this->nBand = nBand;

    eDataType = GDT_Float32;

    nBlockXSize = poDS->GetRasterXSize();
    nBlockYSize = 1;

    /* Cannot overflow as nBlockXSize <= 999 */
    nRecordSize = nBlockXSize*5 + 9 + 2;
    pszRecord = NULL;
}

/************************************************************************/
/*                          ~C2DRasterBand()                            */
/************************************************************************/

C2DRasterBand::~C2DRasterBand()
{
    VSIFree(pszRecord);
}

/************************************************************************/
/*                             IReadBlock()                             */
/************************************************************************/

CPLErr C2DRasterBand::IReadBlock( int nBlockXOff, int nBlockYOff,
                                  void * pImage )

{
    C2DDataset *poGDS = (C2DDataset *) poDS;
    int		i;
    
    if (pszRecord == NULL)
    {
        if (nRecordSize < 0)
            return CE_Failure;

        pszRecord = (char *) VSIMalloc(nRecordSize);
        if (pszRecord == NULL)
        {
            CPLError(CE_Failure, CPLE_OutOfMemory,
                     "Cannot allocate scanline buffer");
            nRecordSize = -1;
            return CE_Failure;
        }
    }

    VSIFSeekL( poGDS->fp, 1011 + nRecordSize*nBlockYOff, SEEK_SET );

    VSIFReadL( pszRecord, 1, nRecordSize, poGDS->fp );

    if( !EQUALN((char *) poGDS->abyHeader,pszRecord,6) )
    {
        CPLError( CE_Failure, CPLE_AppDefined, 
                  "C2D Scanline corrupt.  Perhaps file was not transferred\n"
                  "in binary mode?" );
        return CE_Failure;
    }
    
    if( C2DGetField( pszRecord + 6, 3 ) != nBlockYOff + 1 )
    {
        CPLError( CE_Failure, CPLE_AppDefined, 
                  "C2D scanline out of order, C2D driver does not\n"
                  "currently support partial datasets." );
        return CE_Failure;
    }

    for( i = 0; i < nBlockXSize; i++ )
        ((float *) pImage)[i] = (float)
            (C2DGetField( pszRecord + 9 + 5 * i, 5) * 0.1);

    return CE_None;
}

/************************************************************************/
/* ==================================================================== */
/*				C2DDataset				*/
/* ==================================================================== */
/************************************************************************/

/************************************************************************/
/*                            ~C2DDataset()                             */
/************************************************************************/

C2DDataset::~C2DDataset()

{
    FlushCache();
    if( fp != NULL )
        VSIFCloseL( fp );
}

/************************************************************************/
/*                          GetGeoTransform()                           */
/************************************************************************/

CPLErr C2DDataset::GetGeoTransform( double * padfTransform )

{
    double	dfLLLat, dfLLLong, dfURLat, dfURLong;

    dfLLLat = C2DGetAngle( (char *) abyHeader + 29 );
    dfLLLong = C2DGetAngle( (char *) abyHeader + 36 );
    dfURLat = C2DGetAngle( (char *) abyHeader + 43 );
    dfURLong = C2DGetAngle( (char *) abyHeader + 50 );
    
    padfTransform[0] = dfLLLong;
    padfTransform[3] = dfURLat;
    padfTransform[1] = (dfURLong - dfLLLong) / GetRasterXSize();
    padfTransform[2] = 0.0;
        
    padfTransform[4] = 0.0;
    padfTransform[5] = -1 * (dfURLat - dfLLLat) / GetRasterYSize();


    return CE_None;
}

/************************************************************************/
/*                          GetProjectionRef()                          */
/************************************************************************/

const char *C2DDataset::GetProjectionRef()

{
    return( "GEOGCS[\"Tokyo\",DATUM[\"Tokyo\",SPHEROID[\"Bessel 1841\",6377397.155,299.1528128,AUTHORITY[\"EPSG\",7004]],TOWGS84[-148,507,685,0,0,0,0],AUTHORITY[\"EPSG\",6301]],PRIMEM[\"Greenwich\",0,AUTHORITY[\"EPSG\",8901]],UNIT[\"DMSH\",0.0174532925199433,AUTHORITY[\"EPSG\",9108]],AUTHORITY[\"EPSG\",4301]]" );
}

/************************************************************************/
/*                                Open()                                */
/************************************************************************/

GDALDataset * C2DDataset::Open( GDALOpenInfo * poOpenInfo )

{
/* -------------------------------------------------------------------- */
/*      First we check to see if the file has the expected header       */
/*      bytes.  For now we expect the C2D file to start with a line     */
/*      containing the letters C2D										*/
/* -------------------------------------------------------------------- */
    if( poOpenInfo->nHeaderBytes < 32 
	   || strstr((const char *) poOpenInfo->pabyHeader,"C2D") == NULL){
        return NULL;
	}

    
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
    C2DDataset 	*poDS;

    poDS = new C2DDataset();

    poDS->fp = VSIFOpenL( poOpenInfo->pszFilename, "rb" );
    
/* -------------------------------------------------------------------- */
/*      Read the header.                                                */
/* -------------------------------------------------------------------- */
    VSIFReadL( poDS->abyHeader, 1, 1012, poDS->fp );

	// TODO: Change this to realy get raster dimension
    poDS->nRasterXSize = C2DGetField( (char *) poDS->abyHeader + 23, 3 );
    poDS->nRasterYSize = C2DGetField( (char *) poDS->abyHeader + 26, 3 );
    if  (poDS->nRasterXSize <= 0 || poDS->nRasterYSize <= 0 )
    {
        CPLError( CE_Failure, CPLE_AppDefined, 
                  "Invalid dimensions : %d x %d", 
                  poDS->nRasterXSize, poDS->nRasterYSize); 
        delete poDS;
        return NULL;
    }

/* -------------------------------------------------------------------- */
/*      Create band information objects.                                */
/* -------------------------------------------------------------------- */
    poDS->SetBand( 1, new C2DRasterBand( poDS, 1 ));
	poDS->SetBand( 2, new C2DRasterBand( poDS, 2 ));
	poDS->SetBand( 3, new C2DRasterBand( poDS, 3 ));
	

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

        GetGDALDriverManager()->RegisterDriver( poDriver );
    }
}
