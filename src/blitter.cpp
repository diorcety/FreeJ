/*  FreeJ - blitter layer component
 *
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


#include "layer.h"
#include "blitter.h"
#include "blit.h"
#include "blit_instance.h"
#include "context.h"
#include "iterator.h"
#include "linklist.h"

#include "sdl_screen.h"

#include "jutils.h"
#include "config.h"

Blitter::Blitter() {
    old_lay_x = 0;
    old_lay_y = 0;
    old_lay_w = 0;
    old_lay_h = 0;
}

Blitter::~Blitter() {
}

LinkList<Blit> &Blitter::getBlits() {
    return blitlist;
}

BlitPtr Blitter::getDefaultBlit() {
    return default_blit;
}

void Blitter::setDefaultBlit(BlitPtr blit) {
    default_blit = blit;
}