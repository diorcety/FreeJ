/*  FreeJ - blitter layer component
 *  (c) Copyright 2004-2009 Denis Roio aka jaromil <jaromil@dyne.org>
 *
 * This source code is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License as published
 * by the Free Software Foundation; either version 3 of the License,
 * or (at your option) any later version.
 *
 * This source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * Please refer to the GNU Public License for more details.
 *
 * You should have received a copy of the GNU Public License along with
 * this source code; if not, write to:
 * Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include "config.h"
#include <stdlib.h>

#include "jutils.h"
#include "blitter.h"
#include "blit.h"
#include "parameter_instance.h"
#include "sdl_blits.h"
#include <SDL_rotozoom.h>

AbstractSdlBlitInstance::AbstractSdlBlitInstance(SdlBlitPtr proto) : proto(proto) {

}

void AbstractSdlBlitInstance::operator()(LayerPtr layer) {
    void *offset;
    SDL_Surface *pre_rotozoom = NULL;
    SDL_Surface *rotozoom = NULL; ///< pointer to blittable surface (rotated and zoomed if necessary)

    ViewPortPtr scr = getScreen();

    if(layer->rotating | layer->zooming) {

        // if we have to rotate or scale,
        // create a sdl surface from current pixel buffer
        pre_rotozoom = SDL_CreateRGBSurfaceFrom
                           (layer->buffer,
                           layer->geo.w, layer->geo.h, layer->geo.bpp,
                           layer->geo.getByteWidth(), red_bitmask, green_bitmask, blue_bitmask, alpha_bitmask);

        if(layer->rotating) {

            rotozoom =
                rotozoomSurface(pre_rotozoom, layer->rotate, layer->zoom_x, (int)layer->antialias);

        } else if(layer->zooming) {

            rotozoom =
                zoomSurface(pre_rotozoom, layer->zoom_x, layer->zoom_y, (int)layer->antialias);

        }

        offset = rotozoom->pixels;
        // free the temporary surface (needed again in sdl blits)
        layer->geo_rotozoom.init(rotozoom->w, rotozoom->h, layer->geo.bpp);


    } else offset = layer->buffer;



    //if(lay->need_crop)
    crop(layer, scr);

    this->proto->fct(offset, &sdl_rect, getSdlSurface(), NULL, &layer->geo, parameters);

    // free rotozooming temporary surface
    if(rotozoom) {
        SDL_FreeSurface(pre_rotozoom);
        pre_rotozoom = NULL;
        SDL_FreeSurface(rotozoom);
        rotozoom = NULL;
    }

}

void AbstractSdlBlitInstance::crop(LayerPtr lay, ViewPortPtr scr) {
    func("crop on layer %s x%i y%i w%i h%i for blit %s",
         lay->getName().c_str(), lay->geo.x, lay->geo.y,
         lay->geo.w, lay->geo.h, getName().c_str());

    // assign the right pointer to the *geo used in crop
    // we use the normal geometry if not roto|zoom
    // otherwise the layer::geo_rotozoom
    Geometry *geo;
    const Geometry &scr_geo = scr->getGeometry();
    if(lay->rotating | lay->zooming) {
        geo = &lay->geo_rotozoom;

        // shift up/left to center rotation
        geo->x = lay->geo.x - (geo->w - lay->geo.w) / 2;
        geo->y = lay->geo.y - (geo->h - lay->geo.h) / 2;

    } else {
        geo = &lay->geo;
    }

    //////////////////////

    sdl_rect.x = -(geo->x);
    sdl_rect.y = -(geo->y);
    sdl_rect.w = scr_geo.w;
    sdl_rect.h = scr_geo.h;

    // calculate bytes per row
    lay_bytepitch = lay_pitch * 4;

    //lay->need_crop = false;
}

void AbstractSdlBlitInstance::init(SdlBlitPtr blit) {
    this->proto = blit;
    BlitInstance::init(blit);
}


SdlBlit::SdlBlit(const std::string &name, const std::string &description, blit_fct fct, LinkList<Parameter> &parameters) : Blit(name, description, parameters) {
    this->fct = fct;
}

SdlBlit::SdlBlit(const std::string &name, const std::string &description, blit_fct fct, LinkList<Parameter> &&parameters): SdlBlit(name, description, fct, parameters) {

}

SdlBlit::~SdlBlit() {

}

///////////////////////////////////////////////////////////////////
// SDL BLITS
// TODO: more SDL blits playing with color masks

// temporary surface used in SDL blits
static SDL_Surface *sdl_surf;

BLIT sdl_rgb(void *src, SDL_Rect *src_rect,
             SDL_Surface *dst, SDL_Rect *dst_rect,
             Geometry *geo, LinkList<ParameterInstance> &param) {

    sdl_surf = SDL_CreateRGBSurfaceFrom
                   (src, geo->w, geo->h, geo->bpp,
                   geo->getByteWidth(), red_bitmask, green_bitmask, blue_bitmask, 0x0);

    SDL_BlitSurface(sdl_surf, src_rect, dst, dst_rect);
    //SDL_UpdateRects(sdl_surf, 1, dst_rect);

    SDL_FreeSurface(sdl_surf);
}

BLIT sdl_alpha(void *src, SDL_Rect *src_rect,
               SDL_Surface *dst, SDL_Rect *dst_rect,
               Geometry *geo, LinkList<ParameterInstance> &params) {

    unsigned int int_alpha;
    {
        float alpha = *(float*)(params.front()->get()); // only one value
        int_alpha = (unsigned int) alpha;
    }

    sdl_surf = SDL_CreateRGBSurfaceFrom
                   (src, geo->w, geo->h, geo->bpp,
                   geo->getByteWidth(), red_bitmask, green_bitmask, blue_bitmask, 0x0);

    SDL_SetAlpha(sdl_surf, SDL_SRCALPHA | SDL_RLEACCEL, int_alpha);

    SDL_BlitSurface(sdl_surf, src_rect, dst, dst_rect);

    SDL_FreeSurface(sdl_surf);

}

BLIT sdl_srcalpha(void *src, SDL_Rect *src_rect,
                  SDL_Surface *dst, SDL_Rect *dst_rect,
                  Geometry *geo, LinkList<ParameterInstance> &params) {

    unsigned int int_alpha;
    {
        float alpha = *(float*)(params.front()->get()); // only one value
        int_alpha = (unsigned int) alpha;
    }

    sdl_surf = SDL_CreateRGBSurfaceFrom
                   (src, geo->w, geo->h, geo->bpp,
                   geo->getByteWidth(), red_bitmask, green_bitmask, blue_bitmask, alpha_bitmask);

    SDL_SetAlpha(sdl_surf, SDL_SRCALPHA | SDL_RLEACCEL, int_alpha);

    SDL_BlitSurface(sdl_surf, src_rect, dst, dst_rect);

    SDL_FreeSurface(sdl_surf);

}

BLIT sdl_chromakey(void *src, SDL_Rect *src_rect,
                   SDL_Surface *dst, SDL_Rect *dst_rect,
                   Geometry *geo, LinkList<ParameterInstance> &params) {


    // TODO color

    sdl_surf = SDL_CreateRGBSurfaceFrom
                   (src, geo->w, geo->h, geo->bpp,
                   geo->getByteWidth(), red_bitmask, green_bitmask, blue_bitmask, alpha_bitmask);

    // TODO
    SDL_SetColorKey(sdl_surf, SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);

    //  SDL_SetAlpha(sdl_surf, SDL_RLEACCEL, 0);

    SDL_Surface *colorkey_surf = SDL_DisplayFormat(sdl_surf);

    SDL_BlitSurface(colorkey_surf, src_rect, dst, dst_rect);

    SDL_FreeSurface(sdl_surf);
    SDL_FreeSurface(colorkey_surf);

}

static LinkList<Blit> blits;
static bool init = false;

LinkList<Blit> &get_sdl_blits() {
    if(!init) {
        /////////////

        {
            blits.push_back(MakeShared<SdlBlit>(
                "SDL",
                "RGB blit (SDL)",
                sdl_rgb
            ));
        }

        /////////////

        {
            LinkList<Parameter> parameters;
            parameters.push_back(MakeShared<Parameter>(
                Parameter::NUMBER,
                "alpha",
                "level of transparency of alpha channel (0.0 - 1.0)",
                255.0
            ));

            blits.push_back(MakeShared<SdlBlit>(
                "ALPHA",
                "alpha blit (SDL)",
                sdl_alpha,
                parameters
            ));
        }

        /////////////

        {
            LinkList<Parameter> parameters;
            parameters.push_back(MakeShared<Parameter>(
                Parameter::NUMBER,
                "alpha",
                "level of transparency of alpha channel (0.0 - 1.0)",
                255.0
            ));

            blits.push_back(MakeShared<SdlBlit>(
                "SRCALPHA",
                "source alpha blit (SDL)",
                sdl_srcalpha,
                parameters
            ));
        }
        init = true;
    }

    return blits;
}

