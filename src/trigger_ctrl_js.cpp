/*  FreeJ
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
 * "$Id: freej.cpp 654 2005-08-18 16:52:47Z jaromil $"
 *
 */

#include <callbacks_js.h> // javascript
#include <jsparser.h>
#include <jsparser_data.h>

#include <trigger_ctrl.h>

JS(js_trigger_ctrl_constructor);

DECLARE_CLASS("TriggerController", js_trigger_ctrl_class, js_trigger_ctrl_constructor)

JSFunctionSpec js_trigger_ctrl_methods[] = {
    {0}
};

FACTORY_REGISTER_INSTANTIATOR(Controller, TriggerController, TriggerController, core);

JS(js_trigger_ctrl_constructor) {
    func("%u:%s:%s", __LINE__, __FILE__, __FUNCTION__);
    int check_thread;

    TriggerController *trigger = (TriggerController *)Factory<Controller>::get_instance("TriggerController");
    if(!trigger)
        return JS_FALSE;

    check_thread = JS_GetContextThread(cx);
    if(!check_thread)
        JS_SetContextThread(cx);
    JS_BeginRequest(cx);
    // initialize with javascript context
    if(!trigger->initialized) {
        if(!trigger->init(global_environment)) {
            error("failed initializing keyboard controller");
            JS_EndRequest(cx);
            if(!check_thread)
                JS_ClearContextThread(cx);
            return JS_FALSE;
        }
        // mark that this controller was initialized by javascript
        trigger->javascript = true;
    }

    // assign instance into javascript object
    if(!JS_SetPrivate(cx, obj, (void*)trigger)) {
        error("failed assigning trigger controller to javascript");
        JS_EndRequest(cx);
        if(!check_thread)
            JS_ClearContextThread(cx);
        return JS_FALSE;
    }

    *rval = OBJECT_TO_JSVAL(obj);
    trigger->add_listener(new ControllerListener(cx, obj));
    JS_EndRequest(cx);
    if(!check_thread)
        JS_ClearContextThread(cx);
    return JS_TRUE;
}
