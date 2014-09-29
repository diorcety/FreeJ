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

#include <config.h>
#include <stdlib.h>

#include <jutils.h>
#include <blitter.h>
#include <blit.h>
#include <parameter_instance.h>

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
        LockedLinkList<ParameterInstance> list = LockedLinkList<ParameterInstance>(params);
        float alpha = *(float*)(list.front()->get()); // only one value
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
        LockedLinkList<ParameterInstance> list = LockedLinkList<ParameterInstance>(params);
        float alpha = *(float*)(list.front()->get()); // only one value
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

void setup_sdl_blits(BlitterPtr blitter) {
    ParameterPtr p;

    LockedLinkList<Blit> list = LockedLinkList<Blit>(blitter->blitlist);

    /////////////

    {
        list.push_back(MakeShared<Blit>(
            Blit::SDL,
            "SDL",
            "RGB blit (SDL)",
            (void*) sdl_rgb
        ));
    }

    /////////////

    {
        LinkList<Parameter> parameters;
        LockedLinkList<Parameter> p = LockedLinkList<Parameter>(parameters);
        p.push_back(MakeShared<Parameter>(
            Parameter::NUMBER,
            "alpha",
            "level of transparency of alpha channel (0.0 - 1.0)",
            255.0
        ));

        list.push_back(MakeShared<Blit>(
            Blit::SDL,
            "ALPHA",
            "alpha blit (SDL)",
            (void*)sdl_alpha,
            parameters
        ));
    }

    /////////////

    {
        LinkList<Parameter> parameters;
        LockedLinkList<Parameter> p = LockedLinkList<Parameter>(parameters);
        p.push_back(MakeShared<Parameter>(
            Parameter::NUMBER,
            "alpha",
            "level of transparency of alpha channel (0.0 - 1.0)",
            255.0
        ));

        list.push_back(MakeShared<Blit>(
            Blit::SDL,
            "SRCALPHA",
            "source alpha blit (SDL)",
            (void*)sdl_srcalpha,
            parameters
        ));
    }

    blitter->default_blit = list.front(); // SDL is default
}

