/*  FreeJ - FreeFrame videoFX wrapper
 *
 *  Copyright (C) 2008-2010 Denis Roio <jaromil@dyne.org>
 *  Copyright (C) 2010    Andrea Guzzo <xant@dyne.org>
 *
 *  studied on a  Pure Data Packet module
 *    by Tom Schouten <pdp@zzz.kotnet.org>
 *  to add support for Pete Warden's free video plugins
 *
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
 * "$Id: $"
 *
 */


#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h> // for snprintf()

#include <cstdlib>
#include <string>

#include <config.h>

#include "freeframe_freej.h"
#include "freeframe_instance.h"

#include "jutils.h"
#include "layer.h"

FACTORY_REGISTER_INSTANTIATOR(FilterInstance, FreeframeInstance, FreeframeInstance, core);

FreeframeInstance::FreeframeInstance()
    : FilterInstance() {
    core = 0;
}

FreeframeInstance::FreeframeInstance(FreeframePtr fr)
    : FilterInstance(fr) {
}

FreeframeInstance::~FreeframeInstance() {
    if(core) {
        auto freeframe = DynamicPointerCast<Freeframe>(proto);
        freeframe->plugmain(FF_DEINSTANTIATE, NULL, core);
    }
}

void FreeframeInstance::init(FilterPtr fr) {
    FilterInstance::init(fr);
}

bool FreeframeInstance::apply(LayerPtr lay) {
    if(!FilterInstance::apply(lay)) {
        return false;
    }
    auto freeframe = DynamicPointerCast<Freeframe>(proto);
    auto &geo = lay->getGeometry();
    VideoInfoStruct vidinfo;
    vidinfo.frameWidth = geo.w;
    vidinfo.frameHeight = geo.h;
    vidinfo.orientation = 1;
    vidinfo.bitDepth = FF_CAP_32BITVIDEO;
    core = freeframe->plugmain(FF_INSTANTIATE, &vidinfo, 0).ivalue;
    return core != FF_FAIL;
}

uint32_t *FreeframeInstance::_process(double time, uint32_t *inframe) {
    FilterInstance::_process(time, inframe);
    jmemcpy(outframe, inframe, bytesize);
    auto freeframe = DynamicPointerCast<Freeframe>(proto);
    freeframe->plugmain(FF_PROCESSFRAME, (void*)outframe, core);
    return outframe;
}

