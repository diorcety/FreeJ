/*  FreeJ
 *  (c) Copyright 2006 Denis Roio aka jaromil <jaromil@dyne.org>
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

 * Virtual controller class to be inherited by other controllers

 */

#include "controller.h"
#include "linklist.h"
#include "context.h"

Controller::Controller() {
    func("%s this=%p", __PRETTY_FUNCTION__, this);
    initialized = active = false;
    indestructible = false;
    javascript = false;
}

Controller::~Controller() {
    func("%s %s (%p)", __PRETTY_FUNCTION__, name.c_str(), this);
}

bool Controller::init(ContextPtr freej) {
    initialized = true;
    return(true);
}

bool Controller::add_listener(ControllerListenerPtr listener) {
    LockedLinkList<ControllerListener>(listeners).push_back(listener);
    return true;
}

void Controller::reset() {
    active = false;
    LockedLinkList<ControllerListener> list = LockedLinkList<ControllerListener>(listeners);
    list.clear();
}

ControllerListener::~ControllerListener() {

}

bool ControllerListener::frame() {
    return false;
}

