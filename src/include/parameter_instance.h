/*  FreeJ
 *  (c) Copyright 2001 Denis Roio aka jaromil <jaromil@dyne.org>
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

#ifndef __parameter_instance_h__
#define __parameter_instance_h__


#include "parameter.h"

FREEJ_FORWARD_PTR(ParameterInstance)
class ParameterInstance: public Entry {
public:
    ParameterInstance(ParameterPtr fr);
    virtual ~ParameterInstance();

    virtual bool set(void *val);

    void update();

    virtual void *get();
    ///< calling  function should  do correct type-casting
    ///< according to the parameter type

    bool parse(char *p);
    Parameter::Type getType() const;

protected:
    virtual void _update();
    void *value;
    bool changed; ///< can be used externally by application caller
    ParameterPtr proto;
};

#endif
