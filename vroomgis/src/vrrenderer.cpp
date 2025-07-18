/***************************************************************************
 vrrenderer.cpp
                    
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

#include "vrrenderer.h"

#include "vrlabel.h"
#include "vrlayer.h"
#include "vrlayervector.h"
#include "vrlayervectorc2p.h"
#include "vrrender.h"
#include "vrrendercoltop.h"
#include "vrrendervectorc2p.h"

bool vrRenderer::_IsCorrectRender() {
    wxASSERT(m_layer);
    if (m_render == nullptr) {
        return false;
    }

    bool bReturn = false;
    switch (m_render->GetType()) {
        case vrRENDER_VECTOR:
            if (m_layer->GetType() == vrDRIVER_VECTOR_SHP) {
                bReturn = true;
                break;
            }
            if (m_layer->GetType() == vrDRIVER_VECTOR_MEMORY) {
                bReturn = true;
                break;
            }
            if (m_layer->GetType() == vrDRIVER_VECTOR_C2P) {
                bReturn = true;
            }
            break;

        case vrRENDER_RASTER:
            if (m_layer->GetType() == vrDRIVER_RASTER_TIFF) {
                bReturn = true;
                break;
            }
            if (m_layer->GetType() == vrDRIVER_RASTER_JPEG) {
                bReturn = true;
                break;
            }
            if (m_layer->GetType() == vrDRIVER_RASTER_ESRIGRID) {
                bReturn = true;
                break;
            }
            if (m_layer->GetType() == vrDRIVER_RASTER_EASC) {
                bReturn = true;
                break;
            }
            if (m_layer->GetType() == vrDRIVER_RASTER_SGRD7) {
                bReturn = true;
                break;
            }
            if (m_layer->GetType() == vrDRIVER_RASTER_WMS) {
                bReturn = true;
                break;
            }
            if (m_layer->GetType() == vrDRIVER_RASTER_MEMORY) {
                bReturn = true;
                break;
            }
            break;

        case vrRENDER_RASTER_C2D:
            if (m_layer->GetType() == vrDRIVER_RASTER_C2D) {
                bReturn = true;
            }
            break;

        case vrRENDER_VECTOR_C2P_DIPS:
            if (m_layer->GetType() == vrDRIVER_VECTOR_C2P) {
                bReturn = true;
            }
            break;

        case vrRENDER_VECTOR_C2P_POLY:
            if (m_layer->GetType() == vrDRIVER_VECTOR_C2P) {
                bReturn = true;
            }
            break;

        default:
            break;
    }

    return bReturn;
}

vrRenderer::vrRenderer(vrLayer* layer, vrRender* render, vrLabel* label) {
    wxASSERT(layer);
    m_layer = layer;

    // setting visible status on true
    SetVisible(true);
    SetInEdition(false);

    // create default render and label if null
    if (render == nullptr) {
        switch (m_layer->GetType()) {
            case vrDRIVER_VECTOR_SHP:
            case vrDRIVER_VECTOR_MEMORY:
                m_render = new vrRenderVector();
                {
                    OGRwkbGeometryType myGeomType = ((vrLayerVector*)GetLayer())->GetGeometryType();
                    if (myGeomType == wkbPoint || myGeomType == wkbPoint25D) {
                        ((vrRenderVector*)m_render)->SetSize(vrRENDERER_VECTOR_POINT_DEFAULT_SIZE);
                    }
                }
                break;

            case vrDRIVER_RASTER_TIFF:
            case vrDRIVER_RASTER_JPEG:
            case vrDRIVER_RASTER_ESRIGRID:
            case vrDRIVER_RASTER_EASC:
            case vrDRIVER_RASTER_SGRD7:
            case vrDRIVER_RASTER_WMS:
            case vrDRIVER_RASTER_MEMORY:
                m_render = new vrRenderRaster();
                break;

            case vrDRIVER_RASTER_C2D:
                m_render = new vrRenderRasterColtop();
                break;

            case vrDRIVER_VECTOR_C2P: {
                CT_LAYER_TYPE myLayerType = ((vrLayerVectorC2P*)GetLayer())->GetActiveLayerType();
                if (myLayerType == CT_DIP) {
                    m_render = new vrRenderVectorC2PDips();
                } else if (myLayerType == CT_POLYGON) {
                    m_render = new vrRenderVectorC2PPoly();
                } else {
                    wxLogError(_("Unsupported geometry type: %d"), myLayerType);
                }
            } break;

            default:
                m_render = new vrRender();
                break;
        }
    } else {
        m_render = render;
        // assert correct render is passed.
        bool bCorrectRender = _IsCorrectRender();
        wxASSERT(bCorrectRender);
    }

    if (label == nullptr) {
        m_label = new vrLabel();
    } else {
        m_label = label;
    }
}

vrRenderer::~vrRenderer() {
    wxASSERT(m_render);
    wxASSERT(m_label);
    wxDELETE(m_render);
    wxDELETE(m_label);
}

bool vrRenderer::GetBitmapDataThread(wxImage* bmp, const vrRealRect& coord, double pxsize, long& vectorcount) {
    wxASSERT(bmp);
    wxASSERT(GetVisible());

    // TODO: GetRender and GetLabel aren't working in thread mode
    // we should create copy and send them to the get layer function
    bool bValue = GetLayer()->GetDataThread(bmp, coord, pxsize, GetRender(), GetLabel());

    vectorcount = 0;
    if (GetLayer()->GetType() != vrDRIVER_UNKNOWN && GetLayer()->GetType() < vrDRIVER_RASTER_TIFF) {
        vectorcount = ((vrLayerVector*)GetLayer())->GetObjectDrawn();
    }
    return bValue;
}

bool vrRenderer::GetBitmapData(wxBitmap* bmp, const vrRealRect& coord, double pxsize, long& vectorcount,
                               long& drawnvertex, long& skippedvertex) {
    wxASSERT(bmp);
    wxASSERT(GetVisible());

    bool bValue = GetLayer()->GetData(bmp, coord, pxsize, GetRender(), GetLabel());
    vectorcount = 0;
    if (GetLayer()->GetType() != vrDRIVER_UNKNOWN && GetLayer()->GetType() < vrDRIVER_RASTER_TIFF) {
        vectorcount = ((vrLayerVector*)GetLayer())->GetObjectDrawn();
        drawnvertex = ((vrLayerVector*)GetLayer())->GetVertexDrawn();
        skippedvertex = ((vrLayerVector*)GetLayer())->GetVertexSkipped();
    }
    return bValue;
}

void vrRenderer::SetRender(vrRender* value) {
    m_render = value;
}

void vrRenderer::SetLabel(vrLabel* value) {
    m_label = value;
}

void vrRenderer::SetVisible(bool value) {
    m_visible = value;
}

void vrRenderer::SetInEdition(bool value) {
    m_inEdition = value;
}

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(vrArrayRenderer);
