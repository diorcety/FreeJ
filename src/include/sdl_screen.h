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

#ifndef __SDL_SCREEN_H__
#define __SDL_SCREEN_H__

#include <SDL.h>
#include "screen.h"

#include "factory.h"

FREEJ_FORWARD_PTR(SdlScreen)
class SdlScreen : public ViewPort {
    friend class SdlScreenSdlBlitInstance;
    friend class SdlScreenLinearBlitInstance;
public:
    SdlScreen();
    virtual ~SdlScreen();


protected:
    void do_resize(int resize_w, int resize_h);

public:
    void show();
    void clear();

    void fullscreen();
    void *get_surface();
    fourcc get_pixel_format() {
        return BGRA32;
    };

private:
    SDL_Surface *sdl_screen;

public:
    void *coords(int x, int y);

    bool lock();
    bool unlock();


private:

    bool _init();

    int setres(int wx, int hx);
    SDL_Surface *emuscr;

    bool switch_fullscreen;
    bool dbl;
    uint32_t sdl_flags;

    // allow to use Factory on this class
    FACTORY_ALLOWED;

};

#endif
