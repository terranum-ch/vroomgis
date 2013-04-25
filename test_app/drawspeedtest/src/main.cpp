#include "wx/wxprec.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/app.h>
#include <wx/cmdline.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/dcgraph.h>

enum RENDERER_TYPE {
    RENDERER_WXGRAPHICSCONTEXT = 0,
    RENDERER_DC,
    RENDERER_GCDC
};


static const wxString availlableRendererName [] = {
    _T("wxGraphicsContext"),
    _T("wxDC"),
    _T("wxGCDC")
};



static const wxCmdLineEntryDesc cmdLineDesc[] =
{
    { wxCMD_LINE_SWITCH, "h", "help", "show this help message",
        wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
    { wxCMD_LINE_OPTION, "v", "vmax", "max vertex before cut (default 0)", wxCMD_LINE_VAL_NUMBER},
    { wxCMD_LINE_OPTION, "i", "vincrement", "vertex increment (default 100)", wxCMD_LINE_VAL_NUMBER},
    { wxCMD_LINE_OPTION, "r", "renderer", "renderer processor (default is 0) see bellow for availlable renderer processor", wxCMD_LINE_VAL_NUMBER},
    { wxCMD_LINE_NONE }
};



void PrintArray(const wxArrayString & array, const wxString & msg){
    wxPrintf(msg + _T("\n"));
    for (unsigned int i = 0; i< array.GetCount(); i++) {
        wxPrintf(array[i] + _T("\n"));
    }
}



int GetRandomNumber (int maxnumber){
    int output = 0 + (rand() % (int)(maxnumber - 0 + 1));
    return output;
}



long DrawRandomLineswxGraphicsContext (int numberlines, int maxvertexbeforecut, wxSize bitmapsize, const wxString & bitmappath, const wxString & bitmapname = wxEmptyString, bool keepbitmap = false) {
    wxStopWatch sw;
    
    // create bitmap
    wxBitmap * myBmp = new wxBitmap(bitmapsize);
	wxMemoryDC dc (*myBmp);
	dc.SetBackground(*wxWHITE_BRUSH);
	dc.Clear();
	
    wxGraphicsContext * gdc = wxGraphicsContext::Create(dc);

    int iMaxVertex = maxvertexbeforecut;
    if (iMaxVertex == 0) {
        iMaxVertex = numberlines;
    }

    gdc->SetPen(*wxBLUE_PEN);
    
    int iLineDrawn = 0;
    int iLoop = 0;
    while (1) {
        wxGraphicsPath myPath = gdc->CreatePath();
        myPath.MoveToPoint(GetRandomNumber(bitmapsize.GetWidth()), GetRandomNumber(bitmapsize.GetHeight()));
        for (unsigned int i = 0; i < iMaxVertex; i++) {
            myPath.AddLineToPoint(GetRandomNumber(bitmapsize.GetWidth()), GetRandomNumber(bitmapsize.GetHeight()));
            iLineDrawn++;
        }
        // unused but more real
        myPath.GetBox();
        gdc->StrokePath(myPath);
        iLoop++;
        if (iLineDrawn >= numberlines) {
            break;
        }
    }
    //wxLogMessage(_("Number of loop: %d"), iLoop);
    wxDELETE(gdc);
    
    dc.SelectObject(wxNullBitmap);
    
    // saving bitmap
    if (keepbitmap == true){
        wxString myFileNameTxt = bitmapname;
        if (bitmapname == wxEmptyString) {
            wxFileName myTempFileName (wxFileName::CreateTempFileName(_T("bmp")));
            myFileNameTxt = myTempFileName.GetName();
        }
        
        wxFileName myBmpFilename (bitmappath, myFileNameTxt, _T("png"));
        wxLogMessage(myBmpFilename.GetFullPath());
        myBmp->SaveFile(myBmpFilename.GetFullPath(), wxBITMAP_TYPE_PNG);
    }
    return sw.Time();
}


long DrawRandomLineswxDC (int numberlines, int maxvertexbeforecut, wxSize bitmapsize, const wxString & bitmappath, const wxString & bitmapname = wxEmptyString, bool keepbitmap = false) {
    wxStopWatch sw;
    
    // create bitmap
    wxBitmap * myBmp = new wxBitmap(bitmapsize);
	wxMemoryDC dc (*myBmp);
	dc.SetBackground(*wxWHITE_BRUSH);
	dc.Clear();
	   
    int iMaxVertex = maxvertexbeforecut;
    if (iMaxVertex == 0) {
        iMaxVertex = numberlines;
    }
    
    dc.SetPen(*wxBLUE_PEN);
    
    int iLineDrawn = 0;
    int iLoop = 0;
    while (1) {
        
        wxPoint * myPt = new wxPoint[iMaxVertex];
        for (unsigned int i = 0; i< iMaxVertex; i++) {
            myPt[i] = wxPoint(GetRandomNumber(bitmapsize.GetWidth()), GetRandomNumber(bitmapsize.GetHeight()));
            ++iLineDrawn;
        }
        iLineDrawn = iLineDrawn -1; // ignore first vertex when counting lines
        dc.DrawLines(iMaxVertex, myPt);
        wxDELETEA(myPt);
        
        iLoop++;
        if (iLineDrawn >= numberlines) {
            break;
        }
    }
    dc.SelectObject(wxNullBitmap);
    
    // saving bitmap
    if (keepbitmap == true){
        wxString myFileNameTxt = bitmapname;
        if (bitmapname == wxEmptyString) {
            wxFileName myTempFileName (wxFileName::CreateTempFileName(_T("bmp")));
            myFileNameTxt = myTempFileName.GetName();
        }
        
        wxFileName myBmpFilename (bitmappath, myFileNameTxt, _T("png"));
        wxLogMessage(myBmpFilename.GetFullPath());
        myBmp->SaveFile(myBmpFilename.GetFullPath(), wxBITMAP_TYPE_PNG);
    }
    return sw.Time();
}



long DrawRandomLineswxGCDC (int numberlines, int maxvertexbeforecut, wxSize bitmapsize, const wxString & bitmappath, const wxString & bitmapname = wxEmptyString, bool keepbitmap = false) {
    wxStopWatch sw;
    
    // create bitmap
    wxBitmap * myBmp = new wxBitmap(bitmapsize);
	wxMemoryDC dc (*myBmp);
	dc.SetBackground(*wxWHITE_BRUSH);
	dc.Clear();
    
    wxGCDC myGCDC (dc);
    
    int iMaxVertex = maxvertexbeforecut;
    if (iMaxVertex == 0) {
        iMaxVertex = numberlines;
    }
    
    myGCDC.SetPen(*wxBLUE_PEN);
    
    int iLineDrawn = 0;
    int iLoop = 0;
    while (1) {
        
        wxPoint * myPt = new wxPoint[iMaxVertex];
        for (unsigned int i = 0; i< iMaxVertex; i++) {
            myPt[i] = wxPoint(GetRandomNumber(bitmapsize.GetWidth()), GetRandomNumber(bitmapsize.GetHeight()));
            ++iLineDrawn;
        }
        iLineDrawn = iLineDrawn -1; // ignore first vertex when counting lines
        myGCDC.DrawLines(iMaxVertex, myPt);
        wxDELETEA(myPt);
        
        iLoop++;
        if (iLineDrawn >= numberlines) {
            break;
        }
    }
    dc.SelectObject(wxNullBitmap);
    
    // saving bitmap
    if (keepbitmap == true){
        wxString myFileNameTxt = bitmapname;
        if (bitmapname == wxEmptyString) {
            wxFileName myTempFileName (wxFileName::CreateTempFileName(_T("bmp")));
            myFileNameTxt = myTempFileName.GetName();
        }
        
        wxFileName myBmpFilename (bitmappath, myFileNameTxt, _T("png"));
        wxLogMessage(myBmpFilename.GetFullPath());
        myBmp->SaveFile(myBmpFilename.GetFullPath(), wxBITMAP_TYPE_PNG);
    }
    return sw.Time();
}




int main(int argc, char **argv)
{
    // debugging string for OSX
    // this is needed for viewing string content with Xcode !!
    wxString myTest = _T("Test debugging");
    myTest.ToUTF8().data();
    myTest.Len();
    
    wxApp::CheckBuildOptions(WX_BUILD_OPTIONS_SIGNATURE, "program");
    wxInitializer initializer;
    if ( !initializer )
    {
        fprintf(stderr, "Failed to initialize the wxWidgets library, aborting.");
        return -1;
    }
    wxInitAllImageHandlers();
    
    wxString myLogoTxt = _T("*\n* drawSpeedTest \n* Testing line drawing speed \n* (c) Copyright 2013 Lucien Schreiber - CREALP . All Rights Reserved. \n*\n");
    wxCmdLineParser parser(cmdLineDesc, argc, argv);
    parser.SetLogo(myLogoTxt);
    int myparseResult = parser.Parse(false);
    if (myparseResult != 0){
        parser.Usage();
        wxPrintf(_("available renderer: \n"));
        for (unsigned int i = 0; i< (sizeof(availlableRendererName) / sizeof(wxString)); i++) {
            wxPrintf(_("%d - %s\n"), i, availlableRendererName[i]);
        }
        return 0;
    }
    
    
    long myTotalLines = 10000;
    wxSize myBmpSize (1000,1000);
    long myMaxVertex = 0;
    long myRendererInt = 0;
    long myMaxIncrement = 2;
    RENDERER_TYPE myRendererType = RENDERER_WXGRAPHICSCONTEXT;
    
    parser.Found("vmax", &myMaxVertex);
    parser.Found("vincrement", &myMaxIncrement);
    parser.Found("renderer", &myRendererInt);
    
    if (myRendererInt > (sizeof(availlableRendererName) / sizeof(wxString))){
        wxLogError(_("Renderer not available ! falling back to default"));
        myRendererInt = 0;
    }
    myRendererType = static_cast<RENDERER_TYPE>(myRendererInt) ;
    wxPrintf(myLogoTxt);
    wxPrintf(_("Using: ") + availlableRendererName[myRendererType] + _T("\n"));


    if (myMaxVertex == 0) {
        myMaxVertex = myTotalLines;
    }
    long myActualIncrement = myMaxIncrement;
    long iLoop = 0;
    while (1) {
        long myTime = 0;
        
        switch (myRendererType) {
            case RENDERER_DC:
                myTime = DrawRandomLineswxDC(myTotalLines, myActualIncrement, myBmpSize, _T("/Users/lucien/DATA/PRJ/COLTOP-GIS/test/speed/bmp"));
                break;
                
            default: // RENDERER_WXGRAPHICSCONTEXT
                myTime = DrawRandomLineswxGraphicsContext (myTotalLines, myActualIncrement, myBmpSize, _T("/Users/lucien/DATA/PRJ/COLTOP-GIS/test/speed/bmp"));
                break;
        }
        myActualIncrement = myActualIncrement * 2;
        if (myActualIncrement > myTotalLines) {
            break;
        }
        iLoop++;
        wxPrintf(_T("%ld\t%ld\t%ld\n"), iLoop, myTime, myActualIncrement);
    }
    
    return 0;
}
