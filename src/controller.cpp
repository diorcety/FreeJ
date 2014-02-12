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
#ifdef WITH_JAVASCRIPT
#include <jsparser.h>
#include <jsparser_data.h>
#include <callbacks_js.h>
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

#ifdef WITH_JAVASCRIPT
// other functions are pure virtual
JS(js_ctrl_constructor);
DECLARE_CLASS("Controller", js_ctrl_class, NULL);

JSFunctionSpec js_ctrl_methods[] = {
    {"activate", controller_activate, 0},
    {0}
};

JS(controller_activate) {
    Controller *ctrl = (Controller *) JS_GetPrivate(cx, obj);
    if(!ctrl) {
        error("%u:%s:%s :: Controller core data is NULL",    \
              __LINE__, __FILE__, __FUNCTION__);
        \
        return JS_FALSE;
        \
    }

    *rval = BOOLEAN_TO_JSVAL(ctrl->active);
    if(argc == 1) {
        jsint var = js_get_int(argv[0]);
        ctrl->active = var;
    }
    return JS_TRUE;
}
#endif //WITH_JAVASCRIPT

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

#ifdef WITH_JAVASCRIPT
int Controller::JSCall(const char *funcname, int argc, jsval *argv) {
    int res = 0;
    ControllerListener *listener = listeners.begin();
    while(listener) {
        // TODO - unregister listener if returns false
        if(listener->call(funcname, argc, argv))
            res++;
        listener = (ControllerListener *)listener->next;
    }
    return res;
}

int Controller::JSCall(const char *funcname, int argc, const char *format, ...) {
    int res = 0;
    jsval *argv;
    va_list args;
    ControllerListener *listener = listeners.begin();
    va_start(args, format);
    va_end(args);
    while(listener) {
        void *markp = NULL;
        JS_SetContextThread(listener->context());
        JS_BeginRequest(listener->context());
        argv = JS_PushArgumentsVA(listener->context(), &markp, format, args);
        JS_EndRequest(listener->context());
        JS_ClearContextThread(listener->context());
        // TODO - unregister listener if returns false
        if(listener->call(funcname, argc, argv))
            res++;
        listener = (ControllerListener *)listener->next;
    }
    return res;
}

#endif //WITH_JAVASCRIPT

// ControllerListener

#ifdef WITH_JAVASCRIPT
ControllerListener::ControllerListener(JSContext *cx, JSObject *obj) {
    jsenv = cx;
    jsobj = obj;
    frameFunc = (int)NULL;
}
#endif //WITH_JAVASCRIPT

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

#ifdef WITH_JAVASCRIPT
/* JSCall function by name, cvalues will be converted
 *
 * deactivates controller if any script errors!
 *
 * format values:
 * case 'b': BOOLEAN_TO_JSVAL((JSBool) va_arg(ap, int));
 * case 'c': INT_TO_JSVAL((uint16) va_arg(ap, unsigned int));
 * case 'i':
 * case 'j': js_NewNumberValue(cx, (jsdouble) va_arg(ap, int32), sp)
 * case 'u': js_NewNumberValue(cx, (jsdouble) va_arg(ap, uint32), sp)
 * case 'd':
 * case 'I': js_NewDoubleValue(cx, va_arg(ap, jsdouble), sp)
 * case 's': JS_NewStringCopyZ(cx, va_arg(ap, char *))
 * case 'W': JS_NewUCStringCopyZ(cx, va_arg(ap, jschar *))
 * case 'S': va_arg(ap, JSString *)
 * case 'o': OBJECT_TO_JSVAL(va_arg(ap, JSObject *)
 * case 'f':
 * fun = va_arg(ap, JSFunction *);
 *       fun ? OBJECT_TO_JSVAL(fun->object) : JSVAL_NULL;
 * case 'v': va_arg(ap, jsval);
 */
bool ControllerListener::call(const char *funcname, int argc, const char *format, ...) {
    va_list ap;
    jsval fval = JSVAL_VOID;
    jsval ret = JSVAL_VOID;

    func("%s try calling method %s.%s(argc:%i)", __func__, name.c_str(), funcname, argc);
    JS_SetContextThread(jsenv);
    JS_BeginRequest(jsenv);
    int res = JS_GetProperty(jsenv, jsobj, funcname, &fval);

    if(JSVAL_IS_VOID(fval)) {
        warning("method unresolved by JS_GetProperty");
    } else {
        jsval *argv;
        void *markp;

        va_start(ap, format);
        argv = JS_PushArgumentsVA(jsenv, &markp, format, ap);
        va_end(ap);

        res = JS_CallFunctionValue(jsenv, jsobj, fval, argc, argv, &ret);
        JS_PopArguments(jsenv, &markp);

        if(res) {
            if(!JSVAL_IS_VOID(ret)) {
                JSBool ok;
                JS_ValueToBoolean(jsenv, ret, &ok);
                if(ok) {  // JSfunc returned 'true', so event is done
                    JS_EndRequest(jsenv);
                    JS_ClearContextThread(jsenv);
                    return true;
                }
            }
        }
    }
    JS_EndRequest(jsenv);
    JS_ClearContextThread(jsenv);
    return false; // no callback, redo on next controller
}

/* less bloat but this only works with 4 byte argv values
 */
bool ControllerListener::call(const char *funcname, int argc, jsval *argv) {
    jsval fval = JSVAL_VOID;
    jsval ret = JSVAL_VOID;
    JSBool res;

    func("calling js %s.%s()", name.c_str(), funcname);
    JS_SetContextThread(jsenv);
    JS_BeginRequest(jsenv);
    res = JS_GetProperty(jsenv, jsobj, funcname, &fval);
    if(!res || JSVAL_IS_VOID(fval)) {
        // using func() instead of error() because this is not a real error condition.
        // controller could ask for unregistered functions ...
        // for instance in the case of a keyboardcontroller which propagates keystrokes
        // for unregistered keys
        func("method %s not found in %s controller", funcname, name.c_str());
        JS_EndRequest(jsenv);
        JS_ClearContextThread(jsenv);
        return(false);
    }

    res = JS_CallFunctionValue(jsenv, jsobj, fval, argc, argv, &ret);
    JS_EndRequest(jsenv);
    JS_ClearContextThread(jsenv);

    if(res == JS_FALSE) {
        error("%s : failed call", __PRETTY_FUNCTION__);
        return(false);
    }

    return(true);
}

#endif //WITH_JAVASCRIPT
