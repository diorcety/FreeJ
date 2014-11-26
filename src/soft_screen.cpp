/*  FreeJ
 *  (c) Copyright 2009 Denis Roio aka jaromil <jaromil@dyne.org>
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

#include "layer.h"
#include "blitter.h"
#include "blit_instance.h"

#include "linear_blits.h"

#include "jutils.h"
#include "soft_screen.h"

class SoftScreenLinearBlitInstance: public AbstractLinearBlitInstance {
private:
    SoftScreenWeakPtr screen;

public:
    SoftScreenLinearBlitInstance(SoftScreenWeakPtr screen, LinearBlitPtr proto);

protected:
    virtual void* getSurface();
    virtual ViewPortPtr getScreen();
};

SoftScreenLinearBlitInstance::SoftScreenLinearBlitInstance(SoftScreenWeakPtr screen, LinearBlitPtr proto) : AbstractLinearBlitInstance(proto), screen(screen) {
}

void* SoftScreenLinearBlitInstance::getSurface() {
    SoftScreenPtr screen = this->screen.lock();
    if(!screen) {
        error("No valid screen found");
        return NULL;
    }
    return screen->screen_buffer;
}

ViewPortPtr SoftScreenLinearBlitInstance::getScreen() {
    SoftScreenPtr screen = this->screen.lock();
    if(!screen) {
        error("No valid screen found");
        return ViewPortPtr();
    }
    return screen;
}


class SoftScreenBlitter: public Blitter {
private:
    SoftScreenWeakPtr screen;

public:
    SoftScreenBlitter(SoftScreenPtr screen) {
        this->screen = screen;
    }

    virtual BlitInstancePtr new_instance(BlitPtr blit) {
        BlitInstancePtr ret = BlitInstancePtr();
        LinearBlitPtr linearBlitPtr = DynamicPointerCast<LinearBlit>(blit);
        if(linearBlitPtr) {
            ret = MakeShared<SoftScreenLinearBlitInstance>(screen, linearBlitPtr);
        }

        if(!ret) {
            error("No valid blit instance found");
        } else {
            ret->init(blit);
        }
        return ret;
    }
};



// our objects are allowed to be created trough the factory engine
FACTORY_REGISTER_INSTANTIATOR(ViewPort, SoftScreen, Screen, soft);

SoftScreen::SoftScreen()
    : ViewPort() {

    screen_buffer = NULL;
    name = "SOFT";
}

SoftScreen::~SoftScreen() {
    func("%s", __PRETTY_FUNCTION__);
    if(screen_buffer) free(screen_buffer);
}

bool SoftScreen::_init() {
    blitter = MakeShared<SoftScreenBlitter>(SharedFromThis(SoftScreen));
    LinkList<Blit> &blitterBlits = blitter->getBlits();
    LinkList<Blit> &linearBlits = get_linear_blits();
    blitterBlits.insert(blitterBlits.end(), linearBlits.begin(), linearBlits.end());
    if(blitterBlits.size() > 0) {
        blitter->setDefaultBlit(blitterBlits.front());
    }

    screen_buffer = malloc(geo.getByteSize());
    clear();
    return(true);
}

void SoftScreen::set_buffer(void *buf) {
    if(screen_buffer) free(screen_buffer);
    screen_buffer = buf;
}

void *SoftScreen::coords(int x, int y) {
//   func("method coords(%i,%i) invoked", x, y);
// if you are trying to get a cropped part of the layer
// use the .pixelsize geometric property for a pre-calculated stride
// that is: number of bytes for one full line
    return
        (x + geo.getPixelSize() +
         (uint32_t*)get_surface());
}

void *SoftScreen::get_surface() {
    if(!screen_buffer) {
        error("SOFT screen output is not properly initialised via set_buffer");
        error("this will likely segfault FreeJ");
        return NULL;
    }
    return screen_buffer;
}


void SoftScreen::show() {
}

void SoftScreen::clear() {
    memset(screen_buffer, 0, geo.getByteSize()); // Put in black
}

void SoftScreen::fullscreen() {
}

void SoftScreen::do_resize(int resize_w, int resize_h) {
}
