/*  FreeJ - blitter layer component
 *
 *  (c) Copyright 2004-2009 Denis Roio aka jaromil <jaromil@dyne.org>
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

#include "blit.h"
#include "blit_instance.h"
#include <algorithm>


BlitInstance::BlitInstance() {
}

BlitInstance::BlitInstance(BlitPtr fr) {
}

void BlitInstance::init(BlitPtr fr) {
    func("initializing instance for blit %s", fr->getName().c_str());
    proto = fr;
    name = proto->name;
    active = true;
    
    LinkList<Parameter> &list = proto->parameters;
    // Get the list of params.
    std::for_each(list.begin(), list.end(), [&] (ParameterPtr param) {
        parameters.push_back(MakeShared<ParameterInstance>(param));
    });
}


BlitInstance::~BlitInstance() {
}

