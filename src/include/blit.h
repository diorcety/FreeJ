/*  FreeJ - blitter layer component
 *  (c) Copyright 2004 Denis Roio aka jaromil <jaromil@dyne.org>
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

#ifndef __BLIT_H__
#define __BLIT_H__

#define BLIT static inline void

#include <SDL.h>
#include "geometry.h"

#include "screen.h"
#include "parameter.h"

FREEJ_FORWARD_PTR(Layer)
FREEJ_FORWARD_PTR(Blitter)
FREEJ_FORWARD_PTR(BlitInstance)
FREEJ_FORWARD_PTR(ViewPort)

FREEJ_FORWARD_PTR(Blit)
class Blit : public Entry {
    friend class Blitter;
    friend class BlitInstance;
    friend class ViewPort;
public:

    Blit(const std::string &name, const std::string &description, LinkList<Parameter> &parameters);
    Blit(const std::string &name, const std::string &description, LinkList<Parameter> &&parameters = LinkList<Parameter>());
    virtual ~Blit();
    
    //BlitInstancePtr new_instance();

private:
    LinkList<Parameter> parameters; ///< linklist of blit parameters
};

#endif
