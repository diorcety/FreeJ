/*  FreeJ
 *  Frei0r/FreeFrame genereator layer
 *  (c) Copyright 2007 - 2009 Denis Roio <jaromil@dyne.org>
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
 */

#include <stdlib.h>

#include "config.h"

#include "generator_layer.h"
#include "frei0r_freej.h"
#include "freeframe_freej.h"
#include "fps.h"

#include "jutils.h"
#include "context.h"
#include <algorithm>

// our objects are allowed to be created trough the factory engine
FACTORY_REGISTER_INSTANTIATOR(Layer, GeneratorLayer, GeneratorLayer, ff_f0r);

GeneratorLayer::GeneratorLayer()
    : Layer() {

    generator = NULL;
    generators = NULL;

    type = Layer::GENERATOR;
    name = "GEN";
    //jsclass = &gen0r_layer_class;
    //  set_filename("/particle generator");
    swap_buffer = NULL;
}

GeneratorLayer::~GeneratorLayer() {
    close();
    if(swap_buffer) {
        free(swap_buffer);
    }
}

void GeneratorLayer::register_generators(LinkList<Filter> *gens) {
    generators = gens;
    act("%u generators available", gens->size());
}

bool GeneratorLayer::open(const char *file) {
    func("%s - %s", __PRETTY_FUNCTION__, file);
    if(!generators) {
        error("No generators registered");
        return false;
    }

    LinkList<Filter> &list = *generators;
    LinkList<Filter>::iterator it = std::find_if(list.begin(), list.end(), [&] (FilterPtr &filter) {
                                                           return filter->getName() == file;
                                                       });
    if(it != list.end()) {
        error("generator not found: %s", file);
        return(false);
    }
    FilterPtr proto = *it;

    close();

    if(proto->type() == Filter::FREIOR) {
        generator = Factory<FilterInstance>::new_instance("FreiorInstance");
    } else if(proto->type() == Filter::FREEFRAME) {
        generator = Factory<FilterInstance>::new_instance("FreeframeInstance");
    } else {
        generator = Factory<FilterInstance>::new_instance("FilterInstance");
    }
    if(generator) {
        generator->init(proto);
    }
    generator->apply(SharedFromThis(GeneratorLayer));

    set_filename(file);
    opened = true;
    return true;
}

void GeneratorLayer::close() {
    generator = NULL;
    opened = false;
}

bool GeneratorLayer::_init() {
    if(!swap_buffer)
        swap_buffer = malloc(geo.getByteSize());
    else { // if changing context ensure we can handle its resolution
        swap_buffer = realloc(swap_buffer, geo.getByteSize());
    }
    return(true);
}

void *GeneratorLayer::feed(double time) {
    void *res;
    if(generator) {
        res = generator->process(time, NULL);
        jmemcpy(swap_buffer, res, geo.getByteSize());
    }
    return swap_buffer;
}

