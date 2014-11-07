/*  FreeJ - Parameter class implementation
 *  (c) Copyright 2007-2009 Denis Rojo <jaromil@dyne.org>
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

   $Id$

 */

#include "config.h"
#include "parameter.h"

#include <stdio.h>
#include <stdlib.h>
#include "jutils.h"

Parameter::Parameter(Parameter::Type param_type, const std::string &name, const std::string &description, float multiplier)
    : Entry() {
    value_size = 0;
    switch(param_type) {
    case Parameter::BOOL:
        value_size = sizeof(bool);
        break;
    case Parameter::NUMBER:
        value_size = sizeof(double);
        break;
    case Parameter::COLOR:
        value_size = sizeof(double) * 3;
        break;
    case Parameter::POSITION:
        value_size = sizeof(double) * 2;
        break;
    case Parameter::STRING:
        value_size = sizeof(char) * 512;
        break;
    default:
        error("parameter initialized with unknown type: %u", param_type);
    }
    if(value_size) {
        min_value = calloc(1, value_size);
        max_value = calloc(1, value_size);
    }
    this->multiplier = multiplier;

    type = param_type;

    this->name = name;
    this->description = description;
}

Parameter::~Parameter() {
    free(min_value);
    free(max_value);
}

Parameter::Type Parameter::getType() const {
    return type;
}

