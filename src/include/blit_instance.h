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
#include "blit.h"


FREEJ_FORWARD_PTR(BlitInstance)
class BlitInstance : public Entry {

public:
    BlitInstance();
    BlitInstance(BlitPtr proto);
    virtual ~BlitInstance();

    virtual void init(BlitPtr proto);

protected:
    LinkList<ParameterInstance> parameters; ///< linklist of blit parameters
    bool active;

public:
    virtual void operator()(LayerPtr layer) = 0;
    
    inline LinkList<ParameterInstance> &getParameters() {
        return parameters;
    }
    
    FACTORY_ALLOWED
};

#endif
