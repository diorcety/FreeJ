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

#ifndef __BLIT_INSTANCE_H__
#define __BLIT_INSTANCE_H__

#include <SDL.h>
#include <blit.h>


FREEJ_FORWARD_PTR(BlitInstance)
class BlitInstance : public Entry {

public:
    BlitInstance();
    BlitInstance(BlitPtr fr);
    virtual ~BlitInstance();

    virtual void init(BlitPtr fr);

    inline Blit::BlitType getType() const {
        return proto->type;
    }
    
    inline void * getFun() const {
        return proto->fun;
    }

private:
    LinkList<ParameterInstance> parameters; ///< linklist of blit parameters
    BlitPtr proto;
    bool active;

public:
    inline LinkList<ParameterInstance> &getParameters() {
        return parameters;
    }
//TODOprivate:

    int32_t scr_stride_dx;
    int32_t scr_stride_sx;
    int32_t scr_stride_up;
    int32_t scr_stride;
    uint32_t scr_offset;

    int32_t lay_pitch;
    int32_t lay_bytepitch;
    int32_t lay_stride;
    int32_t lay_stride_sx;
    int32_t lay_stride_dx;
    int32_t lay_stride_up;
    int32_t lay_height;
    uint32_t lay_offset;

    SDL_Rect sdl_rect; // sdl crop rectangle

    // past blit buffer
    void *past_frame;
    
    FACTORY_ALLOWED
};

#endif
