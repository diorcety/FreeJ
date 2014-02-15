/*  FreeJ
 *  (c) Copyright 2009 Denis Roio aka jaromil <jaromil@dyne.org>
 *
 * This source code  is free software; you can  redistribute it and/or
 * modify it under the terms of the GNU Public License as published by
 * the Free Software  Foundation; either version 3 of  the License, or
 * (at your option) any later version.
 *
 * This source code is distributed in the hope that it will be useful,
 * but  WITHOUT ANY  WARRANTY; without  even the  implied  warranty of
 * MERCHANTABILITY or FITNESS FOR  A PARTICULAR PURPOSE.  Please refer
 * to the GNU Public License for more details.
 *
 * You should  have received  a copy of  the GNU Public  License along
 * with this source code; if  not, write to: Free Software Foundation,
 * Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */


#include <joy_ctrl.h>
#include <callbacks_js.h>
#include <jsparser_data.h>
#include <config.h>
#include <generator_layer.h>


class JoyControllerJS: public JoyController {
public:
    virtual int axismotion(int device, int axis, int value);
    virtual int ballmotion(int device, int ball, int xrel, int yrel);
    virtual int hatmotion(int device, int hat, int value);
    virtual int button_down(int device, int button);
    virtual int button_up(int device, int button);
};


int JoyControllerJS::axismotion(int device, int axis, int value) {
    return JSCall("axismotion", 3, "iii", device, axis, value);
}

int JoyControllerJS::ballmotion(int device, int ball, int xrel, int yrel) {
    return JSCall("ballmotion", 4, "iiii", device, ball, xrel, yrel);
}

int JoyControllerJS::hatmotion(int device, int hat, int value) {
    return JSCall("hatmotion", 3, "iii",  device, hat, value);
}

int JoyControllerJS::button_down(int device, int button) {
    return JSCall("button", 3, "iic", device, button, 1);
}

int JoyControllerJS::button_up(int device, int button) {
    return JSCall("button", 3, "iic", device, button, 0);
}

/////// Javascript JoystickController
JS(js_joy_ctrl_constructor);

DECLARE_CLASS_GC("JoystickController", js_joy_ctrl_class, js_joy_ctrl_constructor, js_ctrl_gc);

JSFunctionSpec js_joy_ctrl_methods[] = {
#ifdef HAVE_LINUX
    {"init_rumble",    js_joy_init_rumble,     1},
    {"rumble",         js_joy_rumble,          1},
#endif
    {0}
};

JS(js_joy_ctrl_constructor) {
    func("%u:%s:%s", __LINE__, __FILE__, __FUNCTION__);
    char excp_msg[MAX_ERR_MSG + 1];

    JoyController *joy = new JoyControllerJS();

    // initialize with javascript context
    if(! joy->init(global_environment)) {
        sprintf(excp_msg, "failed initializing joystick controller");
        goto error;
    }

    // assign instance into javascript object
    if(! JS_SetPrivate(cx, obj, (void*)joy)) {
        sprintf(excp_msg, "failed assigning joystick controller to javascript");
        goto error;
    }

    // assign the real js object
    joy->jsobj = obj;

    joy->javascript = true;

    *rval = OBJECT_TO_JSVAL(obj);
    return JS_TRUE;

error:
    JS_ReportErrorNumber(cx, JSFreej_GetErrorMessage, NULL,
                         JSSMSG_FJ_CANT_CREATE, __func__, excp_msg);
    //    cx->newborn[GCX_OBJECT] = NULL;
    // same omissis as in callbacks_js.h
    delete joy;
    return JS_FALSE;
}

#ifdef HAVE_LINUX
// joystick rumble is supported only under linux so far...

JS(js_joy_init_rumble) {
    func("%s", __PRETTY_FUNCTION__);

    JoyController *joy = (JoyController*) JS_GetPrivate(cx, obj);
    if(!joy) JS_ERROR("JOY code data is NULL");

    JS_CHECK_ARGC(1);

    char *devfile = js_get_string(argv[0]);

    if(joy->init_rumble(devfile))
        act("Joystick controller opened rumble device %s", devfile);

    return JS_TRUE;
}

JS(js_joy_rumble) {
    func("%s", __PRETTY_FUNCTION__);

    JoyController *joy = (JoyController*) JS_GetPrivate(cx, obj);
    if(!joy) JS_ERROR("JOY code data is NULL");

    JS_CHECK_ARGC(1);

    uint16_t val;
    JS_ValueToUint16(cx, argv[0], &val);

    joy->rumble(val);

    return JS_TRUE;
}

#endif
