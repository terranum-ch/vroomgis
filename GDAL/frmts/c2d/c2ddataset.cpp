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
	double m_GeoTransform[6];
	bool m_NoDataEnabled;
	double m_NoDataValue;
	GDALDataType m_DEMDataType;
	
	C2DInfo():m_Version(1){
		m_Width = -1;
		m_Height = -1;
		m_NoDataEnabled = FALSE;
		m_NoDataValue = 0;
		m_DEMDataType = GDT_Float32;
		
		m_GeoTransform[0] = 0.0;
		m_GeoTransform[1] = 1.0;
		m_GeoTransform[2] = 0.0;
		m_GeoTransform[3] = 0.0;
		m_GeoTransform[4] = 0.0;
		m_GeoTransform[5] = 1.0;
		
	}
	
	C2DInfo & operator = (const C2DInfo & other){
		if (this != &other) {
			m_Width = other.m_Width;
			m_Height = other.m_Height;
			
			m_DEMDataType = other.m_DEMDataType;
			
			m_NoDataEnabled = other.m_NoDataEnabled;
			m_NoDataValue = other.m_NoDataValue;
			
			m_GeoTransform[0] = other.m_GeoTransform[0];
			m_GeoTransform[1] = other.m_GeoTransform[1];
			m_GeoTransform[2] = other.m_GeoTransform[2];
			m_GeoTransform[3] = other.m_GeoTransform[3];
			m_GeoTransform[4] = other.m_GeoTransform[4];
			m_GeoTransform[5] = other.m_GeoTransform[5];

		}
		return *this;
	}
	
};


CPL_C_START
void    GDALRegister_C2D(void);
CPL_C_END


/************************************************************************/
/*                    SLOPE												*/
/************************************************************************/

typedef struct
{
    double nsres;
    double ewres;
    double scale;
    int    slopeFormat;
} GDALSlopeAlgData;


/*                    HORN's   Slope()                                  */
float GDALSlopeAlg (float* afWin, float fDstNoDataValue, void* pData)
{
    const double radiansToDegrees = 180.0 / M_PI;
    GDALSlopeAlgData* psData = (GDALSlopeAlgData*)pData;
    double dx, dy, key;
    
    dx = ((afWin[0] + afWin[3] + afWin[3] + afWin[6]) - 
          (afWin[2] + afWin[5] + afWin[5] + afWin[8]))/psData->ewres;
	
    dy = ((afWin[6] + afWin[7] + afWin[7] + afWin[8]) - 
          (afWin[0] + afWin[1] + afWin[1] + afWin[2]))/psData->nsres;
	
    key = (dx * dx + dy * dy);
	
    if (psData->slopeFormat == 1) 
        return atan(sqrt(key) / (8*psData->scale)) * radiansToDegrees;
    else
        return 100*(sqrt(key) / (8*psData->scale));
}


/*                    ZEVENBERGEN AND THORNE Slope()                    */
float GDALSlopeAlgZevenbergen (float* afWin, float fDstNoDataValue, void* pData)
{
	const double radiansToDegrees = 180.0 / M_PI;
    GDALSlopeAlgData* psData = (GDALSlopeAlgData*)pData;
    double dg, dh, key;
    
    dg = (-1.0 * afWin[3]+afWin[5]) / (2*psData->ewres);

    dh = (afWin[7]-afWin[1]) / (2*psData->nsres);
	
    key = (dg * dg + dh * dh);
	
    if (psData->slopeFormat == 1) 
        return atan(sqrt(key)) * radiansToDegrees;
    else
        return 100*(sqrt(key));
}



void*  GDALCreateSlopeData(double* adfGeoTransform,
                           double scale,
                           int slopeFormat)
{
    GDALSlopeAlgData* pData =
	(GDALSlopeAlgData*)CPLMalloc(sizeof(GDALSlopeAlgData));
	
    pData->nsres = adfGeoTransform[5];
    pData->ewres = adfGeoTransform[1];
    pData->scale = scale;
    pData->slopeFormat = slopeFormat;
    return pData;
}


/************************************************************************/
/*                    ASPECT											*/
/************************************************************************/

typedef struct
{
	double nsres;
    double ewres;
    int bAngleAsAzimuth;
} GDALAspectAlgData;


/*                    HORN's ASPECT                                  */
float GDALAspectAlg (float* afWin, float fDstNoDataValue, void* pData)
{
    const double degreesToRadians = M_PI / 180.0;
    GDALAspectAlgData* psData = (GDALAspectAlgData*)pData;
    double dx, dy;
    float aspect;
    
    dx = ((afWin[2] + afWin[5] + afWin[5] + afWin[8]) -
          (afWin[0] + afWin[3] + afWin[3] + afWin[6]));
	
    dy = ((afWin[6] + afWin[7] + afWin[7] + afWin[8]) - 
          (afWin[0] + afWin[1] + afWin[1] + afWin[2]));
	
    aspect = atan2(dy,-dx) / degreesToRadians;
	
    if (dx == 0 && dy == 0)
    {
        /* Flat area */
        aspect = fDstNoDataValue;
    } 
    else if ( psData->bAngleAsAzimuth )
    {
        if (aspect > 90.0) 
            aspect = 450.0 - aspect;
        else
            aspect = 90.0 - aspect;
    }
    else
    {
        if (aspect < 0)
            aspect += 360.0;
    }
	
    if (aspect == 360.0) 
        aspect = 0.0;
	
    return aspect;
}



/*                 ZEVENBERGEN AND THORNE's ASPECT               */
float GDALAspectAlgZevenbergen (float* afWin, float fDstNoDataValue, void* pData)
{
    const double degreesToRadians = M_PI / 180.0;
    GDALAspectAlgData* psData = (GDALAspectAlgData*)pData;
    double dg, dh;
    float aspect;
    
	dg = (-1.0 * afWin[3]+afWin[5]) / (2*psData->ewres);
	
    dh = (afWin[7]-afWin[1]) / (2*psData->nsres);
	
	
    aspect = atan2(-dh,-dg) / degreesToRadians;

    if (dh == 0 && dg == 0)
    {
        /* Flat area */
        aspect = fDstNoDataValue;
    } 
    else if ( psData->bAngleAsAzimuth )
    {
        if (aspect > 90.0) 
            aspect = 450.0 - aspect;
        else
            aspect = 90.0 - aspect;
    }
    else
    {
        if (aspect < 0)
            aspect += 360.0;
    }
	
    if (aspect == 360.0) 
        aspect = 0.0;
	
    return aspect;
}


void*  GDALCreateAspectData(int bAngleAsAzimuth, double* adfGeoTransform)
{
    GDALAspectAlgData* pData =
	(GDALAspectAlgData*)CPLMalloc(sizeof(GDALAspectAlgData));
	
    pData->bAngleAsAzimuth = bAngleAsAzimuth;
	pData->nsres = adfGeoTransform[5];
    pData->ewres = adfGeoTransform[1];
    return pData;
}




/************************************************************************/
/*                  GDALGeneric3x3Processing()                          */
/************************************************************************/

typedef float (*GDALGeneric3x3ProcessingAlg) (float* pafWindow, float fDstNoDataValue, void* pData);

CPLErr GDALGeneric3x3Processing  ( GDALRasterBandH hSrcBand,
								  GDALRasterBandH hDstBand,
								  GDALGeneric3x3ProcessingAlg pfnAlg,
								  void* pData,
								  GDALProgressFunc pfnProgress,
								  void * pProgressData,
								  int iBand)
{
    CPLErr eErr;
    float *pafThreeLineWin; /* 3 line rotating source buffer */
    float *pafOutputBuf;     /* 1 line destination buffer */
    int i, j;
	
    int bSrcHasNoData, bDstHasNoData;
    float fSrcNoDataValue = 0.0, fDstNoDataValue = 0.0;
	
    int nXSize = GDALGetRasterBandXSize(hSrcBand);
    int nYSize = GDALGetRasterBandYSize(hSrcBand);
	
    if (pfnProgress == NULL)
        pfnProgress = GDALDummyProgress;
	
	/* -------------------------------------------------------------------- */
	/*      Initialize progress counter.                                    */
	/* -------------------------------------------------------------------- */
    if( !pfnProgress((iBand - 1) / 3.0, NULL, pProgressData ) )
    {
        CPLError( CE_Failure, CPLE_UserInterrupt, "User terminated" );
        return CE_Failure;
    }
	
    pafOutputBuf = (float *) CPLMalloc(sizeof(float)*nXSize);
    pafThreeLineWin  = (float *) CPLMalloc(3*sizeof(float)*(nXSize+1));
	
    fSrcNoDataValue = (float) GDALGetRasterNoDataValue(hSrcBand, &bSrcHasNoData);
    fDstNoDataValue = (float) GDALGetRasterNoDataValue(hDstBand, &bDstHasNoData);
    if (!bDstHasNoData)
        fDstNoDataValue = 0.0;
	
    // Move a 3x3 pafWindow over each cell 
    // (where the cell in question is #4)
    // 
    //      0 1 2
    //      3 4 5
    //      6 7 8
	
    /* Preload the first 2 lines */
    for ( i = 0; i < 2 && i < nYSize; i++)
    {
        GDALRasterIO(   hSrcBand,
					 GF_Read,
					 0, i,
					 nXSize, 1,
					 pafThreeLineWin + i * nXSize,
					 nXSize, 1,
					 GDT_Float32,
					 0, 0);
    }
    
    // Exclude the edges
    for (j = 0; j < nXSize; j++)
    {
        pafOutputBuf[j] = fDstNoDataValue;
    }
    GDALRasterIO(hDstBand, GF_Write,
                 0, 0, nXSize, 1,
                 pafOutputBuf, nXSize, 1, GDT_Float32, 0, 0);
	
    if (nYSize > 1)
    {
        GDALRasterIO(hDstBand, GF_Write,
                     0, nYSize - 1, nXSize, 1,
                     pafOutputBuf, nXSize, 1, GDT_Float32, 0, 0);
    }
    
    int nLine1Off = 0*nXSize;
    int nLine2Off = 1*nXSize;
    int nLine3Off = 2*nXSize;
	
    for ( i = 1; i < nYSize-1; i++)
    {
        /* Read third line of the line buffer */
        eErr = GDALRasterIO(   hSrcBand,
							GF_Read,
							0, i+1,
							nXSize, 1,
							pafThreeLineWin + nLine3Off,
							nXSize, 1,
							GDT_Float32,
							0, 0);
        if (eErr != CE_None)
            goto end;
		
        // Exclude the edges
        pafOutputBuf[0] = fDstNoDataValue;
        if (nXSize > 1)
            pafOutputBuf[nXSize - 1] = fDstNoDataValue;
        
        for (j = 1; j < nXSize - 1; j++)
        {
            float afWin[9];
            afWin[0] = pafThreeLineWin[nLine1Off + j-1];
            afWin[1] = pafThreeLineWin[nLine1Off + j];
            afWin[2] = pafThreeLineWin[nLine1Off + j+1];
            afWin[3] = pafThreeLineWin[nLine2Off + j-1];
            afWin[4] = pafThreeLineWin[nLine2Off + j];
            afWin[5] = pafThreeLineWin[nLine2Off + j+1];
            afWin[6] = pafThreeLineWin[nLine3Off + j-1];
            afWin[7] = pafThreeLineWin[nLine3Off + j];
            afWin[8] = pafThreeLineWin[nLine3Off + j+1];
			
            if (bSrcHasNoData && (
								  afWin[0] == fSrcNoDataValue ||
								  afWin[1] == fSrcNoDataValue ||
								  afWin[2] == fSrcNoDataValue ||
								  afWin[3] == fSrcNoDataValue ||
								  afWin[4] == fSrcNoDataValue ||
								  afWin[5] == fSrcNoDataValue ||
								  afWin[6] == fSrcNoDataValue ||
								  afWin[7] == fSrcNoDataValue ||
								  afWin[8] == fSrcNoDataValue))
            {
                // We have nulls so write nullValue and move on
                pafOutputBuf[j] = fDstNoDataValue;
            }
            else
            {
                // We have a valid 3x3 window.
                pafOutputBuf[j] = pfnAlg(afWin, fDstNoDataValue, pData);
            }
        }
		
        /* -----------------------------------------
         * Write Line to Raster
         */
        eErr = GDALRasterIO(hDstBand, GF_Write, 0, i, nXSize, 1,
							pafOutputBuf, nXSize, 1, GDT_Float32, 0, 0);
        if (eErr != CE_None)
            goto end;
		
		double dprogress =  1.0 * (iBand - 1)/3.0 + (1.0 * (i+1) / nYSize)/3.0;
        if( !pfnProgress(dprogress, NULL, pProgressData ) )
        {
            CPLError( CE_Failure, CPLE_UserInterrupt, "User terminated" );
            eErr = CE_Failure;
            goto end;
        }
        
        int nTemp = nLine1Off;
        nLine1Off = nLine2Off;
        nLine2Off = nLine3Off;
        nLine3Off = nTemp;
    }
	
    pfnProgress( iBand / 3, NULL, pProgressData );
    eErr = CE_None;
	
end:
    CPLFree(pafOutputBuf);
    CPLFree(pafThreeLineWin);
	
    return eErr;
}



/************************************************************************/
/* ==================================================================== */
/*                       GDALGeneric3x3Dataset                        */
/* ==================================================================== */
/************************************************************************/

class GDALGeneric3x3RasterBand;

class GDALGeneric3x3Dataset : public GDALDataset
{
    friend class GDALGeneric3x3RasterBand;
	
    GDALGeneric3x3ProcessingAlg pfnAlg;
    void*              pAlgData;
    GDALDatasetH       hSrcDS;
    GDALRasterBandH    hSrcBand;
    float*             apafSourceBuf[3];
    int                bDstHasNoData;
    double             dfDstNoDataValue;
    int                nCurLine;
	
public:
	GDALGeneric3x3Dataset(GDALDatasetH hSrcDS,
						  GDALRasterBandH hSrcBand,
						  GDALDataType eDstDataType,
						  int bDstHasNoData,
						  double dfDstNoDataValue,
						  GDALGeneric3x3ProcessingAlg pfnAlg,
						  void* pAlgData);
	~GDALGeneric3x3Dataset();
	
    CPLErr      GetGeoTransform( double * padfGeoTransform );
    const char *GetProjectionRef();
};

/************************************************************************/
/* ==================================================================== */
/*                    GDALGeneric3x3RasterBand                       */
/* ==================================================================== */
/************************************************************************/

class GDALGeneric3x3RasterBand : public GDALRasterBand
{
    friend class GDALGeneric3x3Dataset;
	
    
public:
	GDALGeneric3x3RasterBand( GDALGeneric3x3Dataset *poDS,
							 GDALDataType eDstDataType );
    
    virtual CPLErr          IReadBlock( int, int, void * );
    virtual double          GetNoDataValue( int* pbHasNoData );
};

GDALGeneric3x3Dataset::GDALGeneric3x3Dataset(
											 GDALDatasetH hSrcDS,
											 GDALRasterBandH hSrcBand,
											 GDALDataType eDstDataType,
											 int bDstHasNoData,
											 double dfDstNoDataValue,
											 GDALGeneric3x3ProcessingAlg pfnAlg,
											 void* pAlgData)
{
    this->hSrcDS = hSrcDS;
    this->hSrcBand = hSrcBand;
    this->pfnAlg = pfnAlg;
    this->pAlgData = pAlgData;
    this->bDstHasNoData = bDstHasNoData;
    this->dfDstNoDataValue = dfDstNoDataValue;
    
    CPLAssert(eDstDataType == GDT_Byte || eDstDataType == GDT_Float32);
	
    nRasterXSize = GDALGetRasterXSize(hSrcDS);
    nRasterYSize = GDALGetRasterYSize(hSrcDS);
    
    SetBand(1, new GDALGeneric3x3RasterBand(this, eDstDataType));
    
    apafSourceBuf[0] = (float *) CPLMalloc(sizeof(float)*nRasterXSize);
    apafSourceBuf[1] = (float *) CPLMalloc(sizeof(float)*nRasterXSize);
    apafSourceBuf[2] = (float *) CPLMalloc(sizeof(float)*nRasterXSize);
    
    nCurLine = -1;
}

GDALGeneric3x3Dataset::~GDALGeneric3x3Dataset()
{
    CPLFree(apafSourceBuf[0]);
    CPLFree(apafSourceBuf[1]);
    CPLFree(apafSourceBuf[2]);
}

CPLErr GDALGeneric3x3Dataset::GetGeoTransform( double * padfGeoTransform )
{
    return GDALGetGeoTransform(hSrcDS, padfGeoTransform);
}

const char *GDALGeneric3x3Dataset::GetProjectionRef()
{
    return GDALGetProjectionRef(hSrcDS);
}

GDALGeneric3x3RasterBand::GDALGeneric3x3RasterBand(GDALGeneric3x3Dataset *poDS,
                                                   GDALDataType eDstDataType)
{
    this->poDS = poDS;
    this->nBand = 1;
    eDataType = eDstDataType;
    nBlockXSize = poDS->GetRasterXSize();
    nBlockYSize = 1;
}

CPLErr GDALGeneric3x3RasterBand::IReadBlock( int nBlockXOff,
											int nBlockYOff,
											void *pImage )
{
    GDALGeneric3x3Dataset * poGDS = (GDALGeneric3x3Dataset *) poDS;
    
    if ( nBlockYOff == 0 || nBlockYOff == nRasterYSize - 1)
    {
        int j;
        if (eDataType == GDT_Byte)
        {
            for(j=0;j<nBlockXSize;j++)
                ((GByte*)pImage)[j] = (GByte) poGDS->dfDstNoDataValue;
        }
        else
        {
            for(j=0;j<nBlockXSize;j++)
                ((float*)pImage)[j] = (float) poGDS->dfDstNoDataValue;
        }
		
        return CE_None;
    }
	
    if ( poGDS->nCurLine != nBlockYOff )
    {
        if (nBlockYOff != 1 &&
            poGDS->nCurLine == nBlockYOff + 1)
        {
            float* pafTmp =  poGDS->apafSourceBuf[0];
            poGDS->apafSourceBuf[0] = poGDS->apafSourceBuf[1];
            poGDS->apafSourceBuf[1] = poGDS->apafSourceBuf[2];
            poGDS->apafSourceBuf[2] = pafTmp;
            
            poGDS->nCurLine = nBlockYOff;
            
            CPLErr eErr = GDALRasterIO( poGDS->hSrcBand,
									   GF_Read,
									   0, nBlockYOff + 1, nBlockXSize, 1,
									   poGDS->apafSourceBuf[2],
									   nBlockXSize, 1,
									   GDT_Float32,
									   0, 0);
			
            if (eErr != CE_None)
            {
                memset(pImage, 0, nBlockXSize * sizeof(float));
                return eErr;
            }
        }
        else
        {
            poGDS->nCurLine = nBlockYOff;
            int i;
            for(i=0;i<3;i++)
            {
                CPLErr eErr = GDALRasterIO( poGDS->hSrcBand,
										   GF_Read,
										   0, nBlockYOff + i - 1, nBlockXSize, 1,
										   poGDS->apafSourceBuf[i],
										   nBlockXSize, 1,
										   GDT_Float32,
										   0, 0);
                if (eErr != CE_None)
                {
                    memset(pImage, 0, nBlockXSize * sizeof(float));
                    return eErr;
                }
            }
        }
    }
	
    int j;
    
    if (eDataType == GDT_Byte)
    {
        ((GByte*)pImage)[0] = (GByte) poGDS->dfDstNoDataValue;
        if (nBlockXSize > 1)
            ((GByte*)pImage)[nBlockXSize - 1] = (GByte) poGDS->dfDstNoDataValue;
    }
    else
    {
        ((float*)pImage)[0] = (float) poGDS->dfDstNoDataValue;
        if (nBlockXSize > 1)
            ((float*)pImage)[nBlockXSize - 1] = (float) poGDS->dfDstNoDataValue;
    }
    
    int bSrcHasNoData;
    double dfSrcNoDataValue = GDALGetRasterNoDataValue(poGDS->hSrcBand,
                                                       &bSrcHasNoData);
	
    for(j=1;j<nBlockXSize - 1;j++)
    {
        float afWin[9];
        afWin[0] = poGDS->apafSourceBuf[0][j-1];
        afWin[1] = poGDS->apafSourceBuf[0][j];
        afWin[2] = poGDS->apafSourceBuf[0][j+1];
        afWin[3] = poGDS->apafSourceBuf[1][j-1];
        afWin[4] = poGDS->apafSourceBuf[1][j];
        afWin[5] = poGDS->apafSourceBuf[1][j+1];
        afWin[6] = poGDS->apafSourceBuf[2][j-1];
        afWin[7] = poGDS->apafSourceBuf[2][j];
        afWin[8] = poGDS->apafSourceBuf[2][j+1];
		
        // Check if afWin has null value
        int bContainsNull = FALSE;
        if (bSrcHasNoData)
        {
            for ( int n = 0; n <= 8; n++)
            {
                if(afWin[n] == dfSrcNoDataValue)
                {
                    bContainsNull = TRUE;
                    break;
                }
            }
        }
		
        if (bContainsNull)
        {
            // We have nulls so write nullValue and move on
            if (eDataType == GDT_Byte)
                ((GByte*)pImage)[j] = (GByte) poGDS->dfDstNoDataValue;
            else
                ((float*)pImage)[j] = (float) poGDS->dfDstNoDataValue;
        } else {
            // We have a valid 3x3 window.
            if (eDataType == GDT_Byte)
                ((GByte*)pImage)[j] = (GByte) (poGDS->pfnAlg(
															 afWin, poGDS->dfDstNoDataValue, poGDS->pAlgData) + 0.5);
            else
                ((float*)pImage)[j] = (float) poGDS->pfnAlg(
															afWin, poGDS->dfDstNoDataValue, poGDS->pAlgData);
        }
    }
    
    return CE_None;
}

double GDALGeneric3x3RasterBand::GetNoDataValue( int* pbHasNoData )
{
    GDALGeneric3x3Dataset * poGDS = (GDALGeneric3x3Dataset *) poDS;
    if (pbHasNoData)
        *pbHasNoData = poGDS->bDstHasNoData;
    return poGDS->dfDstNoDataValue;
}






/************************************************************************/
/* ==================================================================== */
/*                              C2DDataset                              */
/* ==================================================================== */
/************************************************************************/

class C2DDataset : public RawDataset
{
    FILE        *fpImage;       // image data file.
	C2DInfo		m_RasterInfo;	// raster info
	bool		m_GeoTransformValid;
	char *		m_ProjValue;
	
	static bool		ReadMagicNumber(GDALOpenInfo * poOpenInfo);
	static bool		WriteMagicNumber(const char * pszFilename);
	static bool		WriteHeader(const char * pszFilename, const C2DInfo & info);
	static bool		ReadHeader(const char * pszFilename, C2DInfo & info);
	static bool		WriteProj (const char * pszFilename, const char * proj);
	static bool		ReadProj (char * pszFilename, char ** proj);
	static bool		CreateMetaData (C2DDataset * poDS, GDALDataset *poSrcDS, char ** papszOptions);
	
public:
	C2DDataset();
	~C2DDataset();
	
    virtual CPLErr GetGeoTransform( double * );
	virtual const char *GetProjectionRef();
	
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
	m_GeoTransformValid = FALSE;
	m_ProjValue = NULL;
	
}

/************************************************************************/
/*                            ~C2DDataset()                            */
/************************************************************************/

C2DDataset::~C2DDataset()

{
    FlushCache();
    if( fpImage != NULL )
        VSIFCloseL( fpImage );
	
	if (m_ProjValue != NULL) {
		delete [] m_ProjValue;
	}
	
}

/************************************************************************/
/*                          GetGeoTransform()                           */
/************************************************************************/

CPLErr C2DDataset::GetGeoTransform( double * padfTransform )

{
    if( m_GeoTransformValid)
    {
		memcpy( padfTransform, m_RasterInfo.m_GeoTransform, sizeof(double)*6 );
        return CE_None;
    }
    else
        return CE_Failure;
}


const char * C2DDataset::GetProjectionRef(){
	return m_ProjValue;
}

/************************************************************************/
/*                              Identify()                              */
/************************************************************************/

int C2DDataset::Identify( GDALOpenInfo * poOpenInfo ){

	// Some bytes availlable
	if( poOpenInfo->nHeaderBytes < 3 || poOpenInfo->fp == NULL ){
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
		CPLError( CE_Failure, CPLE_NotSupported,  "Unable to open the %s c2d file",
				 poOpenInfo->pszFilename);
        return FALSE;
    }
	
	int myLength = strlen(C2DMagicName);
	char * myChar = new char[myLength];
	int iReadedB = VSIFReadL(myChar, sizeof(char), sizeof(C2DMagicName), fp);
	if (iReadedB != sizeof(C2DMagicName) || strcmp(myChar, C2DMagicName) != 0) {
		delete [] myChar;
		return FALSE;
	}
	
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
	
		
	// ensure version is supported
	if (pInfo->m_Version != info.m_Version) {
		CPLError( CE_Failure, CPLE_NotSupported,  "Unable to open, driver version conflit (found %d, expected %d)",
				 pInfo->m_Version, info.m_Version);
	}

	info = *pInfo;
	delete pInfo;
	VSIFCloseL(fp);	
	return TRUE;
}




bool C2DDataset::WriteProj (const char * pszFilename, const char * proj){
	FILE * fpImage =VSIFOpenL( pszFilename, "ab" );
	if (fpImage == NULL) {
		CPLError( CE_Warning, CPLE_NotSupported, 
				 "Opening file %s for Writing projection failed",
				 pszFilename);
		return FALSE;
	}
	
	int * myLength = new int (strlen(proj));
	VSIFWriteL(myLength, sizeof(int), 1, fpImage);

	if (*myLength > 0) {
		VSIFWriteL(proj, sizeof(char), *myLength, fpImage);
	}
	VSIFCloseL(fpImage);
	delete myLength;
	return TRUE;
}



bool C2DDataset::ReadProj (char * pszFilename, char ** proj){
	FILE	*fp = NULL;
    fp = VSIFOpenL( pszFilename, "r" );
    if( fp == NULL )
    {
		CPLError( CE_Warning, CPLE_NotSupported,  "Unable to open the %s c2d file",
				 pszFilename);
        return FALSE;
    }
	
	VSIFSeekL(fp, sizeof(C2DMagicName) + sizeof(C2DInfo), SEEK_SET);
	int * myLength = new int(0);
	VSIFReadL(myLength, sizeof(int), 1, fp);
	
	if (*myLength > 0) {
		*proj = new char[*myLength];
		VSIFReadL(*proj, sizeof(char), *myLength, fp);
	}
	VSIFCloseL(fp);
	return TRUE;
}




bool C2DDataset::CreateMetaData (C2DDataset * poDS, GDALDataset *poSrcDS, char ** papszOptions){
	if (poDS == NULL || poSrcDS == NULL) {
		CPLError( CE_Failure, CPLE_NotSupported,  "Creating metadata failed");
        return FALSE;
	}
	
	// RASTER VERSION
	C2DInfo myEmptyInfo;
	char myVersionChar [10] = {"\0"};
	sprintf(myVersionChar, "%d", myEmptyInfo.m_Version);
	poDS->SetMetadataItem( "C2D_VERSION", myVersionChar, NULL);
	
	// RASTER CREATION DATE / TIME
	time_t myTime = time(NULL);
	poDS->SetMetadataItem ("C2D_CREATION_DATE_TIME",
						   ctime(&myTime), NULL);
	
	// ORIGIN FILE
	char** papszFileList = poSrcDS->GetFileList();
	int iNumFile = CSLCount(papszFileList);
	if (iNumFile != 0) {
		char myCount [5] = {"\0"};
		sprintf(myCount, "%d", iNumFile);
		poDS->SetMetadataItem("C2D_ORIGIN_FILE_COUNT", myCount, NULL);
		
		for (int i = 1; i<= iNumFile; i++) {
			char myItemName [25] = {"0"};
			sprintf(myItemName, "C2D_ORIGIN_FILE_%02d", i);
			poDS->SetMetadataItem (myItemName, *(papszFileList+(i-1)), NULL);
		}
	}
	CSLDestroy(papszFileList);
	
	// SLOPE ALGORITHM USED
	const char * pszSlopeAlg = CSLFetchNameValue(papszOptions, "ALGORITHM");
	if (pszSlopeAlg == NULL) {
		pszSlopeAlg = "SHARP";
	}
	poDS->SetMetadataItem ("C2D_ALGORITHM", pszSlopeAlg);
	
	
	return TRUE;
}




/************************************************************************/
/*                                Open()                                */
/************************************************************************/

GDALDataset *C2DDataset::Open( GDALOpenInfo * poOpenInfo ){
	
	if( !Identify( poOpenInfo ) ){
        return NULL;
	}
	
	C2DInfo myRasterInfo;
	if (ReadHeader(poOpenInfo->pszFilename, myRasterInfo)==FALSE) {
		CPLError( CE_Warning, CPLE_NotSupported, "Unable to read header for %s",
				 poOpenInfo->pszFilename);
		return NULL;		
	}
	
	char * myProj = NULL;
	if (ReadProj(poOpenInfo->pszFilename, &myProj)==FALSE) {
		CPLError( CE_Warning, CPLE_NotSupported, "Unable to read projection for %s",
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
	poDS->nRasterXSize = myRasterInfo.m_Width;
	poDS->nRasterYSize = myRasterInfo.m_Height;
	poDS->m_RasterInfo = myRasterInfo;
	poDS->m_GeoTransformValid = TRUE;
	
	if (myProj != NULL) {
		poDS->m_ProjValue = new char [strlen(myProj)];
		strcpy(poDS->m_ProjValue, myProj);
		delete [] myProj;
		myProj = NULL;
	}
	
	// Take ownership of file handled from GDALOpeninfo.
	VSIFClose(poOpenInfo->fp);
	poOpenInfo->fp = NULL;
	
	if( poOpenInfo->eAccess == GA_Update )
        poDS->fpImage = VSIFOpenL( poOpenInfo->pszFilename, "rb+" );
    else
        poDS->fpImage = VSIFOpenL( poOpenInfo->pszFilename, "rb" );
	
    if( poDS->fpImage == NULL ){
        CPLError( CE_Failure, CPLE_OpenFailed,
				 "Failed to re-open %s within C2D driver.\n",
				 poOpenInfo->pszFilename );
        return NULL;
    }
    poDS->eAccess = poOpenInfo->eAccess;
	
	
	// create one band
	int iPixelSize =  GDALGetDataTypeSize( GDT_Float32 ) / 8;
	int iIn = (sizeof(C2DMagicName) / sizeof(char)) + sizeof(C2DInfo) + sizeof(int);
	int         bMSBFirst = TRUE;
#ifdef CPL_LSB
    bMSBFirst = FALSE;
#endif
	
	// if projection was readed then move forward
	if (poDS->m_ProjValue != NULL) {
		iIn += sizeof(char) * strlen(poDS->m_ProjValue);
	}
	
	
	poDS->SetBand(1, new RawRasterBand( poDS, 1, poDS->fpImage, iIn, iPixelSize,
									   myRasterInfo.m_Width*iPixelSize, myRasterInfo.m_DEMDataType, bMSBFirst, TRUE ));
	poDS->GetRasterBand(1)->SetColorInterpretation( GCI_GrayIndex );

	iIn += sizeof(myRasterInfo.m_DEMDataType) * myRasterInfo.m_Width * myRasterInfo.m_Height; 

	poDS->SetBand(2, new RawRasterBand( poDS, 2, poDS->fpImage, iIn, iPixelSize,
									   myRasterInfo.m_Width*iPixelSize, GDT_Float32, bMSBFirst, TRUE ));
	poDS->GetRasterBand(2)->SetColorInterpretation( GCI_GrayIndex );
	
	iIn += sizeof(GDT_Float32) * myRasterInfo.m_Width * myRasterInfo.m_Height; 
	
	poDS->SetBand(3, new RawRasterBand( poDS, 3, poDS->fpImage, iIn, iPixelSize,
									   myRasterInfo.m_Width*iPixelSize, GDT_Float32, bMSBFirst, TRUE ));
	poDS->GetRasterBand(3)->SetColorInterpretation( GCI_GrayIndex );
	

	if (myRasterInfo.m_NoDataEnabled == TRUE) {
		poDS->GetRasterBand(1)->SetNoDataValue(myRasterInfo.m_NoDataValue);
		poDS->GetRasterBand(2)->SetNoDataValue(-9999);
		poDS->GetRasterBand(3)->SetNoDataValue(-9999);
	}
	
	poDS->SetDescription( poOpenInfo->pszFilename );
    poDS->TryLoadXML();
	
	// for overviews support
	poDS->oOvManager.Initialize( poDS, poOpenInfo->pszFilename );
	
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

	// get geotransform informations
	C2DInfo mySrcRasterInfo;
	double * pSrcTransform =  (double *) CPLMalloc(6*sizeof(double));
	poSrcDS->GetGeoTransform (pSrcTransform);
	mySrcRasterInfo.m_GeoTransform[0] = *pSrcTransform;
	mySrcRasterInfo.m_GeoTransform[1] = *(pSrcTransform+1);
	mySrcRasterInfo.m_GeoTransform[2] = *(pSrcTransform+2);
	mySrcRasterInfo.m_GeoTransform[3] = *(pSrcTransform+3);
	mySrcRasterInfo.m_GeoTransform[4] = *(pSrcTransform+4);
	mySrcRasterInfo.m_GeoTransform[5] = *(pSrcTransform+5);
	CPLFree(pSrcTransform);
	
	// get No Data value
	int bSuccess = FALSE;
	double myNodataValue = poSrcDS->GetRasterBand(1)->GetNoDataValue(&bSuccess);
	if (bSuccess == TRUE) {
		mySrcRasterInfo.m_NoDataEnabled = TRUE;
		mySrcRasterInfo.m_NoDataValue = myNodataValue;
	}
	
	// data type for DEM
	mySrcRasterInfo.m_DEMDataType = eType;
	
		
	// Write header
	mySrcRasterInfo.m_Width = poSrcDS->GetRasterBand(1)->GetXSize();
	mySrcRasterInfo.m_Height = poSrcDS->GetRasterBand(1)->GetYSize();
	if (WriteHeader(pszFilename, mySrcRasterInfo) == FALSE) {
		return NULL;
	}
	
	
	
	// write proj
	const char * pSrcPrjRef = poSrcDS->GetProjectionRef();
	if(WriteProj(pszFilename, pSrcPrjRef)==FALSE){
		return NULL;
	}
	
    
	C2DDataset * poDS = (C2DDataset*) GDALOpen( pszFilename, GA_Update ); 
	
	
	/* -------------------------------------------------------------------- */
	/*      Copy the First band image data.                                 */
	/* -------------------------------------------------------------------- */
    int         nXSize = mySrcRasterInfo.m_Width;
    int         nYSize = mySrcRasterInfo.m_Height;
    int  	nBlockXSize, nBlockYSize, nBlockTotal, nBlocksDone;
	
    poSrcDS->GetRasterBand(1)->GetBlockSize( &nBlockXSize, &nBlockYSize );
	
    nBlockTotal = ((nXSize + nBlockXSize - 1) / nBlockXSize)
	* ((nYSize + nBlockYSize - 1) / nBlockYSize);
	
    nBlocksDone = 0;
	GDALRasterBand *poSrcBand = poSrcDS->GetRasterBand( 1 );
	GDALRasterBand *poDstBand = poDS->GetRasterBand( 1 );
	int	       iYOffset, iXOffset;
	void           *pData;
	CPLErr  eErr;
	
	pData = CPLMalloc(nBlockXSize * nBlockYSize
					  * GDALGetDataTypeSize(eType) / 8);
	
	for( iYOffset = 0; iYOffset < nYSize; iYOffset += nBlockYSize )
	{
		for( iXOffset = 0; iXOffset < nXSize; iXOffset += nBlockXSize )
		{
			int	nTBXSize, nTBYSize;
			
			if( !pfnProgress( (nBlocksDone++) / (float) nBlockTotal / 3, /// 3 ,
							 NULL, pProgressData ) )
			{
				CPLError( CE_Failure, CPLE_UserInterrupt, 
						 "User terminated" );
				delete poDS;
				
				GDALDriver *poC2DDriver = 
				(GDALDriver *) GDALGetDriverByName( "C2D" );
				poC2DDriver->Delete( pszFilename );
				return NULL;
			}

			nTBXSize = MIN(nBlockXSize,nXSize-iXOffset);
			nTBYSize = MIN(nBlockYSize,nYSize-iYOffset);

			eErr = poSrcBand->RasterIO( GF_Read, 
									   iXOffset, iYOffset, 
									   nTBXSize, nTBYSize,
									   pData, nTBXSize, nTBYSize,
									   eType, 0, 0 );
			
			if( eErr != CE_None )
			{
				CPLError( CE_Warning, CPLE_NotSupported, "Reading failed");
				return NULL;
			}
			
			
			eErr = poDstBand->RasterIO( GF_Write, 
									   iXOffset, iYOffset, 
									   nTBXSize, nTBYSize,
									   pData, nTBXSize, nTBYSize,
									   eType, 0, 0 );
			
			if( eErr != CE_None )
			{
				CPLError( CE_Warning, CPLE_NotSupported, "Writing failed");
				return NULL;
			}
		}
	}
	
	CPLFree( pData );

	/* Make sure image data gets flushed */
	RawRasterBand *poDstRawBand =  (RawRasterBand *) poDS->GetRasterBand( 1 );
	poDstRawBand->FlushCache();
	
	
   /* if( !pfnProgress( 1.0, NULL, pProgressData ) )
    {
        CPLError( CE_Failure, CPLE_UserInterrupt, 
				 "User terminated" );
        delete poDS;
		
        GDALDriver *poC2DDriver = 
		(GDALDriver *) GDALGetDriverByName( "C2D" );
        poC2DDriver->Delete( pszFilename );
        return NULL;
    }*/
		
	
	
	
	/* -------------------------------------------------------------------- */
	/*      Copy the slope band image data.                                 */
	/* -------------------------------------------------------------------- */
	GDALRasterBand *poDstBand2 = poDS->GetRasterBand( 2 );
	
	pData = NULL;
	GDALGeneric3x3ProcessingAlg pfnAlg = NULL;


	int dfDstNoDataValue = -9999;
	bool bDstHasNoData = TRUE;
	double scale = 1.0;
	// 0 = 'percent' or 1 = 'degrees'
    int slopeFormat = 1; 
	double  adfGeoTransform[6];
	GDALGetGeoTransform(poSrcDS, adfGeoTransform);
	pData = GDALCreateSlopeData(adfGeoTransform, scale, slopeFormat);
	
	const char * pszSlopeAlg = CSLFetchNameValue(papszOptions, "ALGORITHM");
	if (pszSlopeAlg == NULL) {
		pszSlopeAlg = "SHARP";
	}
	
	if (strcmp(pszSlopeAlg, "SOFT")==0){
		pfnAlg = GDALSlopeAlg;
	}
	else {
		pfnAlg = GDALSlopeAlgZevenbergen;
	}
	
	
	GDALGeneric3x3Processing(poSrcBand, poDstBand2,
							 pfnAlg, pData,
							 pfnProgress, NULL, 2);
	
	/* Make sure image data gets flushed */
	RawRasterBand *poDstRawBand2 =  (RawRasterBand *) poDS->GetRasterBand( 2 );
	poDstRawBand2->FlushCache();
	

	
	/* -------------------------------------------------------------------- */
	/*      Copy the aspect band image data.                                 */
	/* -------------------------------------------------------------------- */
	GDALRasterBand *poDstBand3 = poDS->GetRasterBand( 3 );
	
	pData = NULL;
	pfnAlg = NULL;
	
	int bAngleAsAzimuth = TRUE;
	dfDstNoDataValue = -9999;
	bDstHasNoData = TRUE;
	
	pData = GDALCreateAspectData(bAngleAsAzimuth, adfGeoTransform);
	
	if (strcmp(pszSlopeAlg, "SOFT")==0){
		pfnAlg = GDALAspectAlg;
	}
	else {
		pfnAlg = GDALAspectAlgZevenbergen;
	}
		
	
	GDALGeneric3x3Processing(poSrcBand, poDstBand3,
							 pfnAlg, pData,
							 pfnProgress, NULL, 3);
	
	/* Make sure image data gets flushed */
	RawRasterBand *poDstRawBand3 =  (RawRasterBand *) poDS->GetRasterBand( 3 );
	poDstRawBand3->FlushCache();
	
	
	// WRITE METADATA
	CreateMetaData(poDS, poSrcDS, papszOptions);

	
	return poDS;
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
		
		char szCreateOptions[500];
		strcat(szCreateOptions, ""
			  "<CreationOptionList>"
			  "<Option name='ALGORITHM' type='string-select' default='SHARP'>"
					"<Value>SHARP</Value>"
					"<Value>SOFT</Value>"
			   "</Option>"
			   "</CreationOptionList>");
		poDriver->SetMetadataItem(GDAL_DMD_CREATIONOPTIONLIST, szCreateOptions);
		

        poDriver->pfnOpen = C2DDataset::Open;
		poDriver->pfnCreateCopy = C2DDataset::CreateCopy;
		poDriver->pfnIdentify = C2DDataset::Identify;
        GetGDALDriverManager()->RegisterDriver( poDriver );
    }
}
