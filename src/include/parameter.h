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

#ifndef __parameter_h__
#define __parameter_h__


#include <linklist.h>

FREEJ_FORWARD_PTR(Layer)
FREEJ_FORWARD_PTR(FilterInstance)

FREEJ_FORWARD_PTR(Parameter)

typedef void (layer_param_f)(LayerPtr lay, ParameterPtr param, int idx);
typedef void (filter_param_f)(FilterInstancePtr filt, ParameterPtr param, int idx);

class Parameter : public Entry {
    friend class Iterator;
    // TODO: different iterator beahaviour for different parameter types
public:
    enum Type {
        /* Parameter type for boolean values */
        BOOL,
        /* Parameter type for doubles */
        NUMBER,
        /* Parameter type for color */
        COLOR,
        /* Parameter type for position */
        POSITION,
        /* Parameter type for string */
        STRING
    };

    Parameter(Type param_type);
    ~Parameter();

    virtual bool set(void *val);

    virtual void update();

    virtual void *get();
    ///< calling  function should  do correct type-casting
    ///< according to the parameter type

    bool parse(char *p);

    Type type;
public:
    void *value;
    void *min_value;
    void *max_value;
    size_t value_size;

    bool changed; ///< can be used externally by application caller
    float multiplier; ///< multiplier to adjust the value on set (none if 1.0)

};

#endif
