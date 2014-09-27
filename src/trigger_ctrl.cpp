/*  FreeJ - Trigger controller
 *
 *  (c) Copyright 2007 Christoph Rudorff <goil@dyne.org>
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
 * "$Id: trigger_ctrl.cpp 881 2007-08-02 01:06:11Z mrgoil $"
 *
 */


#include <string.h>

#include <trigger_ctrl.h>

#include <config.h>
#include <algorithm>
#include <context.h>
#include <jutils.h>

TriggerController::TriggerController()
    : Controller() {
    setName("Trigger");
    /* we are going to be used as a singleton, so we don't want
       our instance to be destruncted before the program ends */
    indestructible = true;

}

TriggerController::~TriggerController() {
}

int TriggerController::poll() {

    if(javascript) {
        LockedLinkList<ControllerListener> list = listeners.getLock();
        std::for_each(list.begin(), list.end(), [&](ControllerListenerPtr listener){
            listener->frame();
        });
    }
    return dispatch();
}

int TriggerController::dispatch() {

    // if no javascript is initilized then this function
    // should be overridden by other binded languages

    /*
        if (res == JS_FALSE) {
          error("trigger call frame() failed, deactivate ctrl");
          active = false;
        }
     */

    return(1);
}

