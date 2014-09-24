/*  FreeJ - FilterInstance base class
 *
 *  Copyright (C) 2001-2010 Denis Roio <jaromil@dyne.org>
 *  Copyright (C) 2010    Andrea Guzzo <xant@dyne.org>
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

   $Id:$

 */

#include <config.h>
#include <layer.h>
#include <filter.h>

#include <jutils.h>

FACTORY_REGISTER_INSTANTIATOR(FilterInstance, FilterInstance, FilterInstance, core);


FilterInstance::FilterInstance()
    : Entry() {
    core = NULL;
    intcore = 0;
    outframe = NULL;
    active = false;
}

FilterInstance::FilterInstance(FilterPtr fr)
    : Entry() {
    core = NULL;
    intcore = 0;
    outframe = NULL;
    active = false;
    init(fr);
}

FilterInstance::~FilterInstance() {
    func("~FilterInstance");

    if(proto)
        proto->destruct(SharedFromThis());

    if(outframe)
        free(outframe);

}

void FilterInstance::init(FilterPtr fr) {
    func("initializing instance for filter %s", fr->getName().c_str());
    proto = fr;
    setName(proto->name);
    fr->init_parameters(parameters);
    active = true;
}

uint32_t *FilterInstance::process(float fps, uint32_t *inframe) {
    if(!proto) {
        error("void filter instance was called for process: %p", this);
        return inframe;
    }
    proto->update(SharedFromThis(), fps, inframe, outframe);
    return outframe;
}


bool FilterInstance::apply() {
    bool ret = false;
    if(proto) {
        if(auto layer=this->layer.lock()) {
             ret = proto->apply(layer, SharedFromThis());
        }
    }
    return ret;
}

void FilterInstance::set_layer(LayerPtr lay) {
    layer = lay;
}

LayerPtr FilterInstance::get_layer() {
    return layer.lock();
}

bool FilterInstance::inuse() {
    return layer.lock() != NULL;
}

