/*  FreeJ - Frei0r wrapper
 *
 *  Copyright (C) 2007-2010 Denis Roio <jaromil@dyne.org>
 *  Copyright (C) 2010    Andrea Guzzo   <xant@xant.net>
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

#include <config.h>

#ifdef WITH_FREI0R

#include "parameter_instance.h"
#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h> // for snprintf()

#include <cstdlib>
#include <string>
#include <algorithm>

#include "frei0r_freej.h"
#include "frei0r_instance.h"
#include "layer.h"
#include "jutils.h"

FACTORY_REGISTER_INSTANTIATOR(FilterInstance, FreiorInstance, FreiorInstance, core);

FreiorParameter::FreiorParameter(Type param_type, const std::string &name, const std::string &description, int index) : Parameter(param_type, name, description) {
    this->index = index;
}

FreiorParameterInstance::FreiorParameterInstance(FreiorInstancePtr freior, FreiorParameterPtr parameter): ParameterInstance(parameter) {
    this->freior = freior;
}

FreiorParameterInstance::~FreiorParameterInstance() {
}

bool FreiorParameterInstance::set(void *value) {
    auto f = freior.lock();
    auto filter = DynamicPointerCast<Freior>(f->getProto());
    auto param = DynamicPointerCast<FreiorParameter>(proto);
    auto idx = param->getIndex();
    auto type = param->getType();

    func("set_frei0r_param callback on %s for parameter %s at pos %u", f->getName().c_str(), param->getName().c_str(), idx);

    switch(type) {

    // idx-1 because frei0r's index starts from 0
    case F0R_PARAM_BOOL:
        func("bool value is %s", (*(bool*)value == true) ? "true" : "false");
        (*filter->f0r_set_param_value)(f->core, new f0r_param_bool(*(bool*)value), idx - 1);
        break;

    case F0R_PARAM_DOUBLE:
        func("number value is %g", *(double*)value);
        (*filter->f0r_set_param_value)(f->core, new f0r_param_double(*(double*)value), idx - 1);
        break;

    case F0R_PARAM_COLOR: {
        f0r_param_color color;
        color.r = ((double*)value)[0];
        color.g = ((double*)value)[1];
        color.b = ((double*)value)[2];
        (*filter->f0r_set_param_value)(f->core, &color, idx - 1);
    }
    break;

    case F0R_PARAM_POSITION: {
        f0r_param_position position;
        position.x = ((double*)value)[0];
        position.y = ((double*)value)[1];
        (*filter->f0r_set_param_value)(f->core, &position, idx - 1);
    }
    break;

    default:
        error("Unrecognized parameter type %u for set_parameter_value", type);
        return false;
    }
    return true;
}

void *FreiorParameterInstance::get() {
    auto f = freior.lock();
    auto filter = DynamicPointerCast<Freior>(f->getProto());
    auto param = DynamicPointerCast<FreiorParameter>(proto);
    auto idx = param->getIndex();
    auto type = proto->getType();

    func("get_frei0r_param callback on %s for parameter %s at pos %u", f->getName().c_str(), param->getName().c_str(), idx);
    switch(type) {

    // idx-1 because frei0r's index starts from 0
    case F0R_PARAM_BOOL:
        (*filter->f0r_get_param_value)(f->core, (f0r_param_t)value, idx - 1);
        func("bool value is %s", (*(bool*)value == true) ? "true" : "false");
        break;

    case F0R_PARAM_DOUBLE:
        (*filter->f0r_get_param_value)(f->core, (f0r_param_t)value, idx - 1);
        func("number value is %g", *(double*)value);
        break;

    case F0R_PARAM_COLOR: {
        f0r_param_color color;
        (*filter->f0r_get_param_value)(f->core, (f0r_param_t)&color, idx - 1);
        ((double*)value)[0] = (double)color.r;
        ((double*)value)[1] = (double)color.g;
        ((double*)value)[2] = (double)color.b;
    }
    break;

    case F0R_PARAM_POSITION: {
        f0r_param_position position;
        (*filter->f0r_get_param_value)(f->core, (f0r_param_t)&position, idx - 1);
        ((double*)value)[0] = (double)position.x;
        ((double*)value)[1] = (double)position.y;
    }
    break;

    default:
        error("Unrecognized parameter type %u for get_parameter_value", type);
    }
    return value;
}


FreiorInstance::FreiorInstance()
    : FilterInstance() {
    core = NULL;
}

FreiorInstance::FreiorInstance(FreiorPtr fr)
    : FilterInstance(fr) {
}

FreiorInstance::~FreiorInstance() {
    if(core) {
        auto freior = DynamicPointerCast<Freior>(proto);
        freior->f0r_destruct((f0r_instance_t*)core);
    }
}

void FreiorInstance::init(FilterPtr fr) {
    FilterInstance::init(fr);
    auto freior = DynamicPointerCast<Freior>(fr);
    auto &list = freior->parameters;
    // Get the list of params.
    std::for_each(list.begin(), list.end(), [&] (FreiorParameterPtr param) {
        parameters.push_back(MakeShared<FreiorParameterInstance>(SharedFromThis(FreiorInstance), param));
    });
}

bool FreiorInstance::apply(LayerPtr lay) {
    if(!FilterInstance::apply(lay)) {
        return false;
    }
    auto freior = DynamicPointerCast<Freior>(proto);
    auto &geo = lay->getGeometry();
    core = freior->f0r_construct(geo.w, geo.h);
    return core != NULL;
}

uint32_t *FreiorInstance::_process(double time, uint32_t *inframe) {
    FilterInstance::_process(time, inframe);
    auto freior = DynamicPointerCast<Freior>(proto);
    freior->f0r_update((f0r_instance_t*)core, time, inframe, outframe);
    return outframe;
}



#endif // WITH_FREI0R
