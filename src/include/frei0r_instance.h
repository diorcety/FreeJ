/*  FreeJ - Frei0r wrapper
 *  (c) Copyright 2007 Denis Rojo <jaromil@dyne.org>
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
 */

#ifndef __FREI0R_INSTANCE_H__
#define __FREI0R_INSTANCE_H__

#include <config.h>
#ifdef WITH_FREI0R

#include <vector>

#include <linklist.h>
#include <frei0r.h>
#include <filter.h>
#include <factory.h>

FREEJ_FORWARD_PTR(FreiorInstance)
class FreiorInstance : public FilterInstance {
    friend class FreiorParameterInstance;

public:
    FreiorInstance();
    FreiorInstance(FreiorPtr fr);
    ~FreiorInstance();
    
    virtual void init(FilterPtr fr);
    virtual bool apply(LayerPtr lay);

protected:
    virtual uint32_t *_process(double time, uint32_t *inframe);

private:
    void *core;
    inline FilterPtr getProto() {
       return proto;
    };

    FACTORY_ALLOWED
};

FREEJ_FORWARD_PTR(FreiorParameter)
class FreiorParameter: public Parameter {
private:
    int index;

public:
    FreiorParameter(Type param_type, const std::string &name, const std::string &description, int index);

    inline int getIndex() {
        return index;
    }
};

FREEJ_FORWARD_PTR(FreiorParameterInstance)
class FreiorParameterInstance: public ParameterInstance {

private:
    FreiorInstanceWeakPtr freior;

public:
    FreiorParameterInstance(FreiorInstancePtr freior, FreiorParameterPtr parameter);

    ~FreiorParameterInstance();

    virtual bool set(void *value);
    virtual void* get();
};

#endif // WITH_FREI0R

#endif
