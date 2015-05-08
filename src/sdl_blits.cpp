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

    Geometry geo = layer->getGeometry();
    const auto &transformation = geo.getTransformation();
    Matrix rotationMatrix;
    Matrix scalingMatrix;
    transformation.computeRotationScaling(&rotationMatrix, &scalingMatrix);

    const Vector &&rotationAngle = rotationMatrix.eulerAngles(0, 1, 2);
    const Vector &&scalingVector = scalingMatrix.diagonal();
    if(scalingVector.sum() != 3.0 || rotationAngle.z() != 0.0) {
        pre_rotozoom = SDL_CreateRGBSurfaceFrom
               (layer->buffer,
               geo.getSize().x(), geo.getSize().y(), geo.getBpp(),
               geo.getByteWidth(), red_bitmask, green_bitmask, blue_bitmask, alpha_bitmask);

        rotozoom = rotozoomSurfaceXY(pre_rotozoom, rotationAngle.z() / M_PI * 180.0, scalingVector.x(), scalingVector.y(), (int)layer->antialias);
        offset = rotozoom->pixels;
        geo.init(rotozoom->w, rotozoom->h, 32);
    } else {
        offset = layer->buffer;
    }

    //if(lay->need_crop)
    crop(layer, scr, rotozoom);

    this->proto->fct(offset, &sdl_rect, getSdlSurface(), NULL, &geo, parameters);

    // free rotozooming temporary surface
    if(pre_rotozoom != NULL) {
        SDL_FreeSurface(pre_rotozoom);
    }
    if(rotozoom != NULL) {
        SDL_FreeSurface(rotozoom);
    }
}

void AbstractSdlBlitInstance::crop(LayerPtr lay, ViewPortPtr scr, SDL_Surface *sdl) {
    const Geometry &lay_geo = lay->getGeometry();
    const Geometry &scr_geo = scr->getGeometry();
    Vector position = lay_geo.getTransformation() * Vector(0.0, 0.0, 0.0);
    Vector size = Vector(scr_geo.getSize().x(), scr_geo.getSize().y(), 0.0);
    func("crop on layer %s x%i y%i w%u h%u for blit %s",
         lay->getName().c_str(), (int)position.x(), (int)position.y(),
         (unsigned int)lay_geo.getSize().x(), (unsigned int)lay_geo.getSize().y(), getName().c_str());

    if(sdl != NULL) {
        /* Remove this comment il change the position following the zoom
        position = Vector(
            position.x() - (sdl->w - lay_geo.getSize().x()) / 2.0,
            position.y() - (sdl->h - lay_geo.getSize().y()) / 2.0,
            0.0
        );**/
    }

    //////////////////////

    sdl_rect.x = -(position.x());
    sdl_rect.y = -(position.y());
    sdl_rect.w = size.x();
    sdl_rect.h = size.y();

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
                   (src, geo->getSize().x(), geo->getSize().y(), geo->getBpp(),
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
                   (src, geo->getSize().x(), geo->getSize().y(), geo->getBpp(),
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
                   (src, geo->getSize().x(), geo->getSize().y(), geo->getBpp(),
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
                   (src, geo->getSize().x(), geo->getSize().y(), geo->getBpp(),
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

