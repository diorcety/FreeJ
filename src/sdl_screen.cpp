/*  FreeJ
 *  (c) Copyright 2001 Denis Roio aka jaromil <jaromil@dyne.org>
 *
 * This source code is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License as published
 * by the Free Software Foundation; either version 2 of the License,
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
 * "$Id$"
 *
 */

#include "config.h"

#include <stdlib.h>
#include <string.h>
#include <SDL_syswm.h>

#include "layer.h"
#include "blitter.h"
#include "blit_instance.h"
#include "linklist.h"
#include "sdl_screen.h"

#include "linear_blits.h"
#include "sdl_blits.h"

#include <SDL_imageFilter.h>
#include <SDL_framerate.h>
#include <SDL_rotozoom.h>

#include "jutils.h"

#include <algorithm>

class SdlScreenSdlBlitInstance: public AbstractSdlBlitInstance {
private:
    SdlScreenWeakPtr screen;

public:
    SdlScreenSdlBlitInstance(SdlScreenWeakPtr screen, SdlBlitPtr proto);

protected:
    virtual SDL_Surface* getSdlSurface();
    virtual ViewPortPtr getScreen();
};

SdlScreenSdlBlitInstance::SdlScreenSdlBlitInstance(SdlScreenWeakPtr screen, SdlBlitPtr proto) : AbstractSdlBlitInstance(proto), screen(screen) {
}

SDL_Surface* SdlScreenSdlBlitInstance::getSdlSurface() {
    SdlScreenPtr screen = this->screen.lock();
    if(!screen) {
        error("No valid screen found");
        return NULL;
    }
    return screen->sdl_screen;
}

ViewPortPtr SdlScreenSdlBlitInstance::getScreen() {
    SdlScreenPtr screen = this->screen.lock();
    if(!screen) {
        error("No valid screen found");
        return ViewPortPtr();
    }
    return screen;
}

class SdlScreenLinearBlitInstance: public AbstractLinearBlitInstance {
private:
    SdlScreenWeakPtr screen;

public:
    SdlScreenLinearBlitInstance(SdlScreenWeakPtr screen, LinearBlitPtr proto);

protected:
    virtual void* getSurface();
    virtual ViewPortPtr getScreen();
};

SdlScreenLinearBlitInstance::SdlScreenLinearBlitInstance(SdlScreenWeakPtr screen, LinearBlitPtr proto) : AbstractLinearBlitInstance(proto), screen(screen) {
}

void* SdlScreenLinearBlitInstance::getSurface() {
    SdlScreenPtr screen = this->screen.lock();
    if(!screen) {
        return NULL;
    }
    return screen->sdl_screen->pixels;
}

ViewPortPtr SdlScreenLinearBlitInstance::getScreen() {
    SdlScreenPtr screen = this->screen.lock();
    if(!screen) {
        return ViewPortPtr();
    }
    return screen;
}


class SdlScreenBlitter: public Blitter {
private:
    SdlScreenWeakPtr screen;

public:
    SdlScreenBlitter(SdlScreenPtr screen) {
        this->screen = screen;
    }

    virtual BlitInstancePtr new_instance(BlitPtr blit) {
        SdlBlitPtr sdlBlitPtr = DynamicPointerCast<SdlBlit>(blit);
        if(sdlBlitPtr) {
            return MakeShared<SdlScreenSdlBlitInstance>(screen, sdlBlitPtr);
        } else {
            LinearBlitPtr linearBlitPtr = DynamicPointerCast<LinearBlit>(blit);
            if(linearBlitPtr) {
                return MakeShared<SdlScreenLinearBlitInstance>(screen, linearBlitPtr);
            }
        }

        error("No valid blit instance found");
        return BlitInstancePtr();
    }
};


// our objects are allowed to be created trough the factory engine
FACTORY_REGISTER_INSTANTIATOR(ViewPort, SdlScreen, Screen, sdl);

SdlScreen::SdlScreen()
    : ViewPort() {

    sdl_screen = NULL;
    emuscr = NULL;

    dbl = false;
    sdl_flags = (SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_HWACCEL);
    //| SDL_DOUBLEBUF | SDL_HWACCEL | SDL_RESIZABLE);
    // add above | SDL_FULLSCREEN to go fullscreen from the start

    switch_fullscreen = false;

    name = "SDL";
}

SdlScreen::~SdlScreen() {
    SDL_Quit();
}

bool SdlScreen::_init() {
    blitter = MakeShared<SdlScreenBlitter>(SharedFromThis(SdlScreen));
    LinkList<Blit> &blitterBlits = blitter->getBlits();
    LinkList<Blit> &sdlBlits = get_sdl_blits();
    blitterBlits.insert(blitterBlits.end(), sdlBlits.begin(), sdlBlits.end());
    LinkList<Blit> &linearBlits = get_linear_blits();
    blitterBlits.insert(blitterBlits.end(), linearBlits.begin(), linearBlits.end());
    if(blitterBlits.size() > 0) {
        blitter->setDefaultBlit(blitterBlits.front());
    }


    char temp[120];

    setres(geo.w, geo.h);
    sdl_screen = SDL_GetVideoSurface();

    SDL_VideoDriverName(temp, 120);

    notice("SDL Viewport is %s %ix%i %ibpp",
           temp, geo.w, geo.h, geo.bpp);

    /* be nice with the window manager */
    sprintf(temp, "%s %s", PACKAGE, VERSION);
    SDL_WM_SetCaption(temp, temp);

    /* hide mouse cursor */
    //SDL_ShowCursor(SDL_DISABLE);

    // optimise sdl_gfx blits
    if(SDL_imageFilterMMXdetect())
        act("SDL using MMX accelerated blit");

    return(true);
}

void SdlScreen::do_resize(int resize_w, int resize_h) {
    act("resizing viewport to %u x %u", resize_w, resize_h);
    sdl_screen = SDL_SetVideoMode(resize_w, resize_h, 32, sdl_flags);
    geo.init(resize_w, resize_h, 32);
}

void *SdlScreen::coords(int x, int y) {
    return
        (x + geo.getPixelSize() * y +
         (uint32_t*)sdl_screen->pixels);
}

void SdlScreen::show() {

    if(switch_fullscreen) {
#ifdef HAVE_DARWIN
#ifndef WITH_COCOA
        if((sdl_flags & SDL_FULLSCREEN) == SDL_FULLSCREEN)
            sdl_flags &= ~SDL_FULLSCREEN;
        else
            sdl_flags |= SDL_FULLSCREEN;
        screen = SDL_SetVideoMode(w, h, bpp, sdl_flags);
#else
        SDL_WM_ToggleFullScreen(sdl_screen);
#endif
#else
        SDL_WM_ToggleFullScreen(sdl_screen);
#endif
        switch_fullscreen = false;
    }

    lock();
    SDL_Flip(sdl_screen);
    unlock();

}

void *SdlScreen::get_surface() {
    return sdl_screen->pixels;
}

void SdlScreen::clear() {
    SDL_FillRect(sdl_screen, NULL, 0x0);
}

void SdlScreen::fullscreen() {
    switch_fullscreen = true;
}

bool SdlScreen::lock() {
    if(!SDL_MUSTLOCK(sdl_screen)) return true;
    if(SDL_LockSurface(sdl_screen) < 0) {
        error("%s", SDL_GetError());
        return false;
    }
    return(true);
}

bool SdlScreen::unlock() {
    if(SDL_MUSTLOCK(sdl_screen)) {
        SDL_UnlockSurface(sdl_screen);
    }
    return true;
}

int SdlScreen::setres(int wx, int hx) {
    /* check and set available videomode */
    int res;
    int bpp = 32;
    act("setting resolution to %u x %u", wx, hx);
    res = SDL_VideoModeOK(wx, hx, bpp, sdl_flags);


    sdl_screen = SDL_SetVideoMode(wx, hx, bpp, sdl_flags);
    //  screen = SDL_SetVideoMode(wx, hx, 0, sdl_flags);
    if(sdl_screen == NULL) {
        error("can't set video mode: %s\n", SDL_GetError());
        return(false);
    }


    if(res != bpp) {
        act("your screen does'nt support %ubpp", bpp);
        act("doing video surface software conversion");

        emuscr = SDL_GetVideoSurface();
        act("emulated surface geometry %ux%u %ubpp",
            emuscr->w, emuscr->h, emuscr->format->BitsPerPixel);
    }


    return res;
}

