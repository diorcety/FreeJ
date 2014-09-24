/*  FreeJ
 *  Copyright (C) 2001-2010 Denis Roio     <jaromil@dyne.org>
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
 */

#ifndef __filter_h__
#define __filter_h__

#include <config.h>

#include <parameter.h>
#include <filter_instance.h>

FREEJ_FORWARD_PTR(Layer)
#ifdef WITH_FREI0R
FREEJ_FORWARD_PTR(Freior)
#endif
#ifdef WITH_COCOA
FREEJ_FORWARD_PTR(CVFilter)
#endif
FREEJ_FORWARD_PTR(Freeframe)

template <class T> class Linklist;

FREEJ_FORWARD_PTR(Filter)
class Filter : public Entry {
    friend class FilterInstance;
public:
    // supported filter types
    enum Type {
#ifdef WITH_FREI0R
        FREIOR = 0,
#endif
#ifdef WITH_COCOA
        COREIMAGE = 1,
#endif
        FREEFRAME = 2
    };
    Filter();
    virtual ~Filter();

    virtual FilterInstancePtr new_instance();

    virtual FilterInstancePtr apply(LayerPtr lay);
    virtual bool apply(LayerPtr lay, FilterInstancePtr instance);

    virtual const char *description();
    virtual const char *author();

    virtual int get_parameter_type(int i);

    virtual char *get_parameter_description(int i);

    virtual int type() = 0;

    bool initialized;
    bool active;
    bool inuse;

protected:
    virtual void destruct(FilterInstancePtr inst);
    virtual void update(FilterInstancePtr inst, double time, uint32_t *inframe, uint32_t *outframe);
    virtual void apply_parameters(FilterInstancePtr inst);
    virtual void init_parameters(Linklist<Parameter> &parameters) = 0;
    int bytesize;

};

#endif
