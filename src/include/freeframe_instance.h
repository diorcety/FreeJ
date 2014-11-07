/*  FreeJ - Freeframe wrapper
 *  (c) Copyright 2008 - 2009 Denis Roio <jaromil@dyne.org>
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
 *
 */

#ifndef __FREEFRAME_INSTANCE_H__
#define __FREEFRAME_INSTANCE_H__

#include "linklist.h"
#include "freeframe.h"
#include "filter.h"
#include "factory.h"

FREEJ_FORWARD_PTR(FreeframeInstance)
class FreeframeInstance : public FilterInstance {
    friend class FreeframeParameterInstance;
    friend class Freeframe;

public:
    FreeframeInstance();
    FreeframeInstance(FreeframePtr fr);
    virtual ~FreeframeInstance();
    virtual void init(FilterPtr fr);
    virtual bool apply(LayerPtr lay);

protected:
    virtual uint32_t *_process(double time, uint32_t *inframe);
    
private:
    DWORD core;
    inline FilterPtr getProto() {
       return proto;
    };

    FACTORY_ALLOWED
};

FREEJ_FORWARD_PTR(FreeframeParameter)
class FreeframeParameter: public Parameter {
public:
    FreeframeParameter(Type param_type, const std::string &name, const std::string &description);
    virtual ~FreeframeParameter();
};

FREEJ_FORWARD_PTR(FreeframeParameterInstance)
class FreeframeParameterInstance: public ParameterInstance {

private:
    FreeframeInstanceWeakPtr freior;

public:
    FreeframeParameterInstance(FreeframeInstancePtr freior, FreeframeParameterPtr parameter);

    virtual ~FreeframeParameterInstance();
};

#endif
