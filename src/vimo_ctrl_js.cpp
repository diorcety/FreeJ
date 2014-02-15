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
#include <jsparser_data.h>

#include <vimo_ctrl.h>

class ViMoControllerJS: public ViMoController {
 public:
    ~ViMoControllerJS();
    virtual void button(unsigned int button, bool state, unsigned int mask, unsigned int old_mask);
    virtual void inner_wheel(int direction, unsigned int history);
    virtual void outer_wheel(int speed, int old_speed);
};

ViMoControllerJS::~ViMoControllerJS() {
    if(jsobj)
        JS_SetPrivate(jsenv, jsobj, NULL);
    jsobj = NULL;
}

void ViMoControllerJS::button(unsigned int button, bool state, unsigned int mask, unsigned int old_mask) {
    JSCall("button", 4, "ubuu", button, state, mask, old_mask);
}

void ViMoControllerJS::inner_wheel(int direction, unsigned int history) {
    JSCall("wheel_i", 2, "iu", direction, history);
}

void ViMoControllerJS::outer_wheel(int speed, int old_speed) {
    JSCall("wheel_o", 2, "ii", speed, old_speed);
}

JS(js_vimo_ctrl_constructor);
DECLARE_CLASS("ViMoController", js_vimo_ctrl_class, js_vimo_ctrl_constructor);

JSFunctionSpec js_vimo_ctrl_methods[] = {
    {"open",	js_vimo_open,	0},
    {"close",	js_vimo_close,	0},
    {0}
};

JS(js_vimo_open) {
    ViMoController *vmc = (ViMoController *)JS_GetPrivate(cx, obj);
    if(!vmc) {
        error("%s core data NULL", __PRETTY_FUNCTION__);
        return JS_FALSE;
    }
    if(argc == 0) {
        return JS_NewNumberValue(cx, vmc->open(), rval);
    }
    if(argc == 1) {
        char *filename = js_get_string(argv[0]);
        return JS_NewNumberValue(cx, vmc->open(filename), rval);
    }
    JS_ERROR("Wrong number of arguments");
}

JS(js_vimo_close) {
    ViMoController *vmc = (ViMoController *)JS_GetPrivate(cx, obj);
    if(!vmc) {
        error("%s core data NULL", __PRETTY_FUNCTION__);
        return JS_FALSE;
    }
    vmc->close();
    return JS_TRUE;
}

JS(js_vimo_ctrl_constructor) {
    func("%u:%s:%s", __LINE__, __FILE__, __FUNCTION__);

    ViMoController *vimo = new ViMoControllerJS();

    // initialize with javascript context
    if(! vimo->init(global_environment)) {
        error("failed initializing ViMo controller");
        delete vimo;
        return JS_FALSE;
    }
    if(argc == 1) {
        char *filename = js_get_string(argv[0]);
        if(!vimo->open(filename)) {
            error("failed initializing ViMo controller");
            delete vimo;
            return JS_FALSE;
        }
    }

    // assign instance into javascript object
    if(! JS_SetPrivate(cx, obj, (void*)vimo)) {
        error("failed assigning ViMo controller to javascript");
        delete vimo;
        return JS_FALSE;
    }

    // assign the real js object
    vimo->jsobj = obj;
    vimo->javascript = true;


    *rval = OBJECT_TO_JSVAL(obj);
    return JS_TRUE;
}
