/*  C++ Entity class
 *
 *  (c) Copyright 2001-2004 Denis Roio aka jaromil <jaromil@dyne.org>
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
 * "$Id$"
 *
 */

#include <stdlib.h>
#include <string.h>

#include "jutils.h"
#include "entity.h"
#include "linklist.h"

Entry::Entry(const std::string& name, const std::string& description) {
    this->name = name;
    this->description = description;
}

Entry::~Entry() {
}

const std::string &Entry::getName() const {
    return name;
}

const std::string &Entry::getDescription() const {
    return description;
}
