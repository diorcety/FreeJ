/*  FreeJ
 *
 *  Copyright (C) 2001-2010 Denis Roio <jaromil@dyne.org>
 *  Copyright (C) 2010    Andrea Guzzo <xant@xant.net>
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

#ifndef __filter_instance_h__
#define __filter_instance_h__

#include <config.h>

#include <parameter.h>
#include <linklist.h>
#include <stdint.h>
#include <factory.h>

FREEJ_FORWARD_PTR(Filter)

FREEJ_FORWARD_PTR(FilterInstance)
class FilterInstance : public Entry {
    friend class Filter;

public:
    FilterInstance();
    FilterInstance(FilterPtr fr);
    virtual ~FilterInstance();

    virtual void init(FilterPtr fr);
    virtual uint32_t *process(float fps, uint32_t *inframe);

    virtual bool apply();

    virtual bool inuse();

    LayerPtr get_layer();

    // XXX - most of this stuff should be private or protected
    FilterPtr proto;

    bool active;

    unsigned intcore;
    void *core;

    uint32_t *outframe;

    Linklist<Parameter> parameters;

protected:
    void set_layer(LayerPtr lay);

private:
    LayerWeakPtr layer;

    FACTORY_ALLOWED
};

#endif
