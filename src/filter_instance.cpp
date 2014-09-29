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
#include <algorithm>

FilterInstance::FilterInstance() {
    outframe = NULL;
    active = false;
}

FilterInstance::FilterInstance(FilterPtr fr)
    : FilterInstance() {
    init(fr);
}

FilterInstance::~FilterInstance() {
    func("~FilterInstance");

    if(outframe)
        free(outframe);
}

void FilterInstance::init(FilterPtr fr) {
    func("initializing instance for filter %s", fr->getName().c_str());
    proto = fr;
    name = proto->name;
    active = true;
}

LayerPtr FilterInstance::get_layer() {
    return layer.lock();
}

bool FilterInstance::inuse() {
    return layer.lock() != NULL;
}

uint32_t *FilterInstance::process(double time, uint32_t *inframe) {
    if(!active) {
        return inframe;
    }
    return _process(time, inframe);
}

uint32_t *FilterInstance::_process(double time, uint32_t *inframe) {
    LockedLinkList<ParameterInstance> list = LockedLinkList<ParameterInstance>(parameters);

    std::for_each(list.begin(), list.end(), [&] (ParameterInstancePtr param) {
        param->update();
    });
    return outframe;
}


bool FilterInstance::apply(LayerPtr lay) {
    auto &geo = lay->getGeometry();
    errno = 0;
    outframe = (uint32_t*) calloc(geo.getByteSize(), 1);
    if(errno != 0) {
        error("calloc outframe failed (%i) applying filter %s", errno, name.c_str());
        error("Filter %s cannot be instantiated", name.c_str());
        return false;
    }

    bytesize = geo.getByteSize();

    act("initialized filter %s on layer %s", name.c_str(), lay->getName().c_str());

    layer = lay;

    return true;
}
