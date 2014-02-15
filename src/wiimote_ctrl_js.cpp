/*  FreeJ WiiMote controller
 *  (c) Copyright 2008 Denis Rojo <jaromil@dyne.org>
 *
 * based on libcwiid by L. Donnie Smith
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
 * "$Id: $"
 *
 */

#include <callbacks_js.h>
#include <jsparser_data.h>

#include <wiimote_ctrl.h>

#ifdef WITH_CWIID

class WiiControllerJS: public WiiController {
public:
    virtual void connect_event();
    virtual void disconnect_event();
    virtual void error_event(WiiError err);

    virtual void accel_event(double x, double y, double z);

    virtual void ir_event(unsigned int source, unsigned int x, unsigned int y,
                          unsigned int size);

    virtual void button_event(unsigned int button, bool state,
                              unsigned int mask, unsigned int old_mask);
};

void WiiControllerJS::connect_event() {
    JSCall("connect", 1, "b", 1);
}

void WiiControllerJS::disconnect_event() {
    JSCall("disconnect", 1, "b", 1);
}

void WiiControllerJS::error_event(WiiError err) {
    JSCall("error", 1, "u", err);
}

void WiiControllerJS::accel_event(double x, double y, double z) {
    JSCall("acceleration", 3, "ddd", x, y, z);
}

void WiiControllerJS::ir_event(unsigned int source, unsigned int x,
                             unsigned int y, unsigned int size) {
    JSCall("ir", 4, "iuui", source, x, y, size);
}

void WiiControllerJS::button_event(unsigned int button, bool state,
                                 unsigned int mask, unsigned int old_mask) {
    JSCall("button", 4, "ubuu", button, state, mask, old_mask);
}

/////// Javascript WiiController
JS(js_wii_ctrl_constructor) {
    func("%u:%s:%s", __LINE__, __FILE__, __FUNCTION__);
    char excp_msg[MAX_ERR_MSG + 1];

    WiiController *wii = new WiiControllerJS();
    // initialize with javascript context
    if(! wii->init(global_environment)) {
        sprintf(excp_msg, "failed initializing WiiMote controller");
        goto error;
    }

    // assign instance into javascript object
    if(! JS_SetPrivate(cx, obj, (void*)wii)) {
        sprintf(excp_msg, "failed assigning WiiMote controller to javascript");
        goto error;
    }

    // assign the real js object
    wii->jsobj = obj;
    wii->javascript = true;

    notice("WiiMote controller attached");

    *rval = OBJECT_TO_JSVAL(obj);
    return JS_TRUE;

error:
    JS_ReportErrorNumber(cx, JSFreej_GetErrorMessage, NULL,
                         JSSMSG_FJ_CANT_CREATE, __func__, excp_msg);
    //  cx->newborn[GCX_OBJECT] = NULL;
    delete wii;
    return JS_FALSE;
}

JS(js_wii_ctrl_open) {
    func("%u:%s:%s argc: %u", __LINE__, __FILE__, __FUNCTION__, argc);
    WiiController *wii = (WiiController *)JS_GetPrivate(cx, obj);
    if(!wii) JS_ERROR("Wii core data is NULL");

    if(argc > 0) {
        char *addr = js_get_string(argv[0]);
        wii->open(addr);
    } else {
        wii->open();
    }

    return JS_TRUE;
}

JS(js_wii_ctrl_close) {
    func("%u:%s:%s argc: %u", __LINE__, __FILE__, __FUNCTION__, argc);
    WiiController *wii = (WiiController *)JS_GetPrivate(cx, obj);
    if(!wii) JS_ERROR("Wii core data is NULL");

    wii->close();

    return JS_TRUE;
}

JS(js_wii_ctrl_battery) {
    func("%u:%s:%s argc: %u", __LINE__, __FILE__, __FUNCTION__, argc);
    WiiController *wii = (WiiController *)JS_GetPrivate(cx, obj);
    if(!wii) JS_ERROR("Wii core data is NULL");

    double value = wii->battery();
    return JS_NewNumberValue(cx, value, rval);
}

JS(js_wii_ctrl_x) {
    func("%u:%s:%s argc: %u", __LINE__, __FILE__, __FUNCTION__, argc);
    WiiController *wii = (WiiController *)JS_GetPrivate(cx, obj);
    if(!wii) JS_ERROR("Wii core data is NULL");

    double value = wii->x();
    return JS_NewNumberValue(cx, value, rval);
}

JS(js_wii_ctrl_y) {
    func("%u:%s:%s argc: %u", __LINE__, __FILE__, __FUNCTION__, argc);
    WiiController *wii = (WiiController *)JS_GetPrivate(cx, obj);
    if(!wii) JS_ERROR("Wii core data is NULL");

    double value = wii->y();
    return JS_NewNumberValue(cx, value, rval);
}

JS(js_wii_ctrl_z) {
    func("%u:%s:%s argc: %u", __LINE__, __FILE__, __FUNCTION__, argc);
    WiiController *wii = (WiiController *)JS_GetPrivate(cx, obj);
    if(!wii) JS_ERROR("Wii core data is NULL");

    double value = wii->z();
    return JS_NewNumberValue(cx, value, rval);
}

JS(js_wii_ctrl_actaccel) {
    func("%u:%s:%s argc: %u", __LINE__, __FILE__, __FUNCTION__, argc);
    WiiController *wii = (WiiController *)JS_GetPrivate(cx, obj);

    *rval = JSVAL_FALSE;
    if(argc == 1) {
        JSBool state;
        JS_ValueToBoolean(cx, argv[0], &state);
        *rval = BOOLEAN_TO_JSVAL(wii->set_accel_report(state));
    } else {
        *rval = BOOLEAN_TO_JSVAL(wii->get_accel_report());
    }
    return JS_TRUE;
}

JS(js_wii_ctrl_ir) {
    func("%u:%s:%s argc: %u", __LINE__, __FILE__, __FUNCTION__, argc);
    WiiController *wii = (WiiController *)JS_GetPrivate(cx, obj);

    *rval = JSVAL_FALSE;
    if(argc == 1) {
        JSBool state;
        JS_ValueToBoolean(cx, argv[0], &state);
        *rval = BOOLEAN_TO_JSVAL(wii->set_ir_report(state));
    } else {
        *rval = BOOLEAN_TO_JSVAL(wii->get_ir_report());
    }
    return JS_TRUE;
}

JS(js_wii_ctrl_actbutt) {
    func("%u:%s:%s argc: %u", __LINE__, __FILE__, __FUNCTION__, argc);
    WiiController *wii = (WiiController *)JS_GetPrivate(cx, obj);

    *rval = JSVAL_FALSE;
    if(argc == 1) {
        JSBool state;
        JS_ValueToBoolean(cx, argv[0], &state);
        *rval = BOOLEAN_TO_JSVAL(wii->set_button_report(state));
    } else {
        *rval = BOOLEAN_TO_JSVAL(wii->get_button_report());
    }
    return JS_TRUE;
}

JS(js_wii_ctrl_rumble) {
    WiiController *wii = (WiiController *)JS_GetPrivate(cx, obj);

    if(argc == 1) {
        JSBool state;
        JS_ValueToBoolean(cx, argv[0], &state);
        *rval = BOOLEAN_TO_JSVAL(wii->set_rumble(state));
    } else {
        *rval = BOOLEAN_TO_JSVAL(wii->get_rumble());
    }

    return JS_TRUE;
}

JS(js_wii_ctrl_actleds) {
    func("%u:%s:%s argc: %u", __LINE__, __FILE__, __FUNCTION__, argc);
    *rval = JSVAL_FALSE;
    WiiController *wii = (WiiController *)JS_GetPrivate(cx, obj);

    unsigned int led = 0;
    JSBool state = false;

    if(argc == 2) {
        JS_ValueToUint16(cx, argv[0], (uint16_t *)&led);
        JS_ValueToBoolean(cx, argv[1], &state);
        *rval = BOOLEAN_TO_JSVAL(wii->set_led(led, state));
    } else if(argc == 1) {
        JS_ValueToUint16(cx, argv[0], (uint16_t *)&led);
        *rval = BOOLEAN_TO_JSVAL(wii->get_led(led));
    }

    return JS_TRUE;
}

JS(js_wii_ctrl_dump) {
    func("%u:%s:%s argc: %u", __LINE__, __FILE__, __FUNCTION__, argc);
    WiiController *wii = (WiiController *)JS_GetPrivate(cx, obj);
    if(!wii) JS_ERROR("Wii core data is NULL");

    wii->dump();

    return JS_TRUE;
}

DECLARE_CLASS_GC("WiiController", js_wii_ctrl_class, js_wii_ctrl_constructor, js_ctrl_gc);

JSFunctionSpec js_wii_ctrl_methods[] = {
    {"open",           js_wii_ctrl_open,       1},
    {"close",          js_wii_ctrl_close,      0},
    {"battery",        js_wii_ctrl_battery,    0},
    {"x",              js_wii_ctrl_x,          0},
    {"y",              js_wii_ctrl_y,          0},
    {"z",              js_wii_ctrl_z,          0},
    {"toggle_accel",   js_wii_ctrl_actaccel,   0},
    {"toggle_ir",      js_wii_ctrl_ir,         0},
    {"toggle_buttons", js_wii_ctrl_actbutt,    0},
    {"toggle_rumble",  js_wii_ctrl_rumble,     0},
    {"toggle_led",     js_wii_ctrl_actleds,    2},
    {"dump",           js_wii_ctrl_dump,       0},

    {0}
};

#endif //WITH_CWIID
