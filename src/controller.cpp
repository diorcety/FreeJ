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

#include <controller.h>
#include <linklist.h>
#include <context.h>

#ifdef WITH_JAVASCRIPT
#include <jsparser.h>
#endif //WITH_JAVASCRIPT

Controller::Controller() {
    func("%s this=%p", __PRETTY_FUNCTION__, this);
    initialized = active = false;
    indestructible = false;
    javascript = false;
}

Controller::~Controller() {
    func("%s %s (%p)", __PRETTY_FUNCTION__, name.c_str(), this);
    ControllerListener *listener = listeners.begin();
    while(listener) {
        delete listener;
        listener = listeners.begin();
    }
}

bool Controller::init(Context *freej) {
#ifdef WITH_JAVASCRIPT
    if(freej->js) {
        // the object is set to global, but should be overwritten
        // in every specific object constructor with the "obj" from JS
        // XXX - set initial value to NULL instead of creating a fake useless object
        jsenv = freej->js->global_context;
        jsobj = freej->js->global_object;
    }
#endif //WITH_JAVASCRIPT

    initialized = true;
    return(true);
}

bool Controller::add_listener(ControllerListener *listener) {
    listeners.append(listener);
    return true;
}

void Controller::reset() {
    active = false;
    ControllerListener *listener = listeners.begin();
    while(listener) {
        delete listener;
        listener = listeners.begin();
    }
}

ControllerListener::~ControllerListener() {

}

bool ControllerListener::frame() {
#ifdef WITH_JAVASCRIPT
    jsval ret = JSVAL_VOID;
    JSBool res;

    JS_SetContextThread(jsenv);
    JS_BeginRequest(jsenv);

    if(!frameFunc) {
        res = JS_GetProperty(jsenv, jsobj, "frame", &frameFunc);
        if(!res || JSVAL_IS_VOID(frameFunc)) {
            error("method frame not found in TriggerController");
            JS_ClearContextThread(jsenv);
            JS_EndRequest(jsenv);
            return false;
        }
    }
    res = JS_CallFunctionValue(jsenv, jsobj, frameFunc, 0, NULL, &ret);
    JS_EndRequest(jsenv);
    JS_ClearContextThread(jsenv);
    if(res == JS_FALSE) {
        error("trigger call frame() failed, deactivate ctrl");
        //active = false;
        return false;
    }
    return true;
#else //WITH_JAVASCRIPT
    return false;
#endif //WITH_JAVASCRIPT
}

