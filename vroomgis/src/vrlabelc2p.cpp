/***************************************************************************
 vrlabelc2p.cpp
 -------------------
 copyright            : (C) 2011 CREALP Lucien Schreiber 
 email                : lucien.schreiber at crealp dot vs dot ch
 ***************************************************************************/


#include "vrlabelc2p.h"
#include "vrrender.h"

vrLabelC2P::vrLabelC2P(wxFont font, wxColour color) : vrLabel(font, color) {
}



vrLabelC2P::~vrLabelC2P() {
}



bool vrLabelC2P::AddFeature(long fid, OGRGeometry * geom, const wxString & text, double rotation) {
    return false;
}



bool vrLabelC2P::Draw(wxGraphicsContext * gdc, const wxRect2DDouble & coord, vrRender * render, double pixsize) {
    return false;
}

