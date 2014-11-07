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

#include "config.h"

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

FACTORY_REGISTER_INSTANTIATOR(Filter, Freior, Frei0rFilter, core);

Freior::Freior()
    : Filter() {
    handle = NULL;
    opened = false;
    name = "Unknown";
}

Freior::~Freior() {
    if(handle) {
        dlclose(handle);
    }
}

int Freior::open(char *file) {
    void *sym;
#ifdef HAVE_FREEBSD
    const char *err;
#else
    char *err;
#endif

    if(opened) {
        error("Freior object %p has already opened file %s", this, filename);
        return 0;
    }

    // clear up the errors if there were any
    dlerror();

#if 0
    if(!dlopen_preflight(file)) {
        warning("plugin '%s' failed: %s", file, dlerror());
        return 0;
    }
#endif
    // Create dynamic handle to the library file.
    handle = dlopen(file, RTLD_LAZY);
    if(!handle) {
        warning("can't dlopen plugin: %s", file);
        return 0;
    }

    // try the frei0r symbol
    sym = dlsym(handle, "f0r_init");

    // return if error
    if(dlerror()) {
        //    func("not a valid frei0r plugin: %s", file);
        // don't forget to close
        dlclose(handle);
        handle = NULL;
        return 0;
    }

    // Get interface function pointers
    *(void**)(&f0r_init) = sym;  // dlsym(handle, "f0r_init");
    *(void**)(&f0r_get_plugin_info) = dlsym(handle, "f0r_get_plugin_info");
    err = dlerror();
    if(err)
        warning("%s in frei0r plugin %s", err, file);
    *(void**)(&f0r_get_param_info) = dlsym(handle, "f0r_get_param_info");
    err = dlerror();
    if(err)
        warning("%s in frei0r plugin %s", err, file);
    *(void**)(&f0r_construct) = dlsym(handle, "f0r_construct");
    err = dlerror();
    if(err)
        warning("%s in frei0r plugin %s", err, file);
    *(void**)(&f0r_destruct) = dlsym(handle, "f0r_destruct");
    err = dlerror();
    if(err)
        warning("%s in frei0r plugin %s", err, file);
    *(void**)(&f0r_set_param_value) = dlsym(handle, "f0r_set_param_value");
    err = dlerror();
    if(err)
        warning("%s in frei0r plugin %s", err, file);
    *(void**)(&f0r_get_param_value) = dlsym(handle, "f0r_get_param_value");
    err = dlerror();
    if(err)
        warning("%s in frei0r plugin %s", err, file);
    *(void**)(&f0r_update) = dlsym(handle, "f0r_update");
    // XXX - don't output an error since f0r_update() is exported only by generators
    //       and it won't be found when opening filters
    //err = dlerror(); if (err) warning("%s in frei0r plugin %s", err, file);

    // get the info on the plugin
    (*f0r_get_plugin_info)(&info);

    opened = true;
    snprintf(filename, 255, "%s", file);

    init();

    if(get_debug() > 2)
        print_info();

    return 1;

}

void Freior::init() {
    f0r_init();

    name = info.name;
    description = info.explanation;
    author = info.author;

    // Get the list of params.
    for(int i = 0; i < info.num_params; ++i) {
        f0r_param_info_t param_info;
        (f0r_get_param_info)(&param_info, i);
        parameters.push_back(MakeShared<FreiorParameter>(
            (Parameter::Type)param_info.type,
            param_info.name,
            param_info.explanation,
            i
        ));
    }
}

void Freior::print_info() {
    notice("Name             : %s", info.name);
    act("%s", info.explanation);
    switch(info.plugin_type) {
    case F0R_PLUGIN_TYPE_FILTER:
        act("Type             : Filter");
        break;
    case F0R_PLUGIN_TYPE_SOURCE:
        act("Type             : Source");
        break;
    case F0R_PLUGIN_TYPE_MIXER2:
        act("Type             : Mixer2");
        break;
    case F0R_PLUGIN_TYPE_MIXER3:
        act("Type             : Mixer3");
        break;
    default:
        error("Unrecognized plugin type");
    }
    act("Author           : %s", info.author);
    act("Parameters [%i total]", parameters.size());
    std::for_each(this->parameters.begin(), this->parameters.end(), [&] (FreiorParameterPtr param) {
        char tmp[256];
        snprintf(tmp, 255, "  [%i] %s ", param->getIndex(), param->getName().c_str());
        switch(param->getType()) {
        case F0R_PARAM_BOOL:
            act("%s (bool) %s", tmp, param->getDescription().c_str());
            break;
        case F0R_PARAM_DOUBLE:
            act("%s (double) %s", tmp, param->getDescription().c_str());
            break;
        case F0R_PARAM_COLOR:
            act("%s (color) %s", tmp, param->getDescription().c_str());
            break;
        case F0R_PARAM_POSITION:
            act("%s (position) %s", tmp, param->getDescription().c_str());
            break;
        case F0R_PARAM_STRING:
            act("%s (string) %s", tmp, param->getDescription().c_str());
            break;
        default:
            error("%s Unrecognized info type.", tmp);
        }
    });
}

int Freior::type() {
    return Filter::FREIOR;
}

FilterInstancePtr Freior::new_instance() {
    FreiorInstancePtr instance = DynamicPointerCast<FreiorInstance>(Factory<FilterInstance>::new_instance("FreiorInstance"));
    if(instance)
        instance->init(SharedFromThis(Filter));
    return instance;
}

#endif // WITH_FREI0R

