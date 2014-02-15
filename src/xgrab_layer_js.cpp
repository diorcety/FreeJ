/*  FreeJ
 *  (c) Copyright 2007 C. Rudorff <goil@dyne.org>
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
 * "$Id: xgrab_layer.cpp 922 2007-09-05 19:17:31Z mrgoil $"
 *
 * some parts shameless stolen from avidemux GUI_xvDraw.cpp
 */

#include <callbacks_js.h>
#include <jsparser_data.h>

#include <xgrab_layer.h>

#ifdef WITH_XGRAB

class XGrabLayerJS: public XGrabLayer {
    XGrabLayerJS();
};

XGrabLayerJS::XGrabLayerJS() {
    jsclass = &js_xgrab_class;
}

DECLARE_CLASS_GC("XGrabLayer", js_xgrab_class, js_xgrab_constructor, js_layer_gc);
JS_CONSTRUCTOR("XGrabLayer", js_xgrab_constructor, XGrabLayerJS);

JSFunctionSpec js_xgrab_methods[] = {
    ENTRY_METHODS  ,
    {"open",      js_xgrab_open,  1},
    {"close",     js_xgrab_close, 1},
    {0}
};
#if 0
//JS_CONSTRUCTOR("ViewPort",js_xgrab_constructor,XGrabLayer);
JS(js_xgrab_constructor) {
    func("%u:%s:%s", __LINE__, __FILE__, __FUNCTION__);

    XGrabLayer *xg = new XGrabLayer();

    // initialize with javascript context
    if(! xg->init(cx, obj)) {
        error("failed initializing xgrab");
        delete xg;
        return JS_FALSE;
    }
    if(argc == 1) {
        jsint winid = js_get_int(argv[0]);
        if(!JS_NewNumberValue(cx, xg->open(winid), rval)) {
            error("failed initializing xgrab controller");
            delete xg;
            return JS_FALSE;
        }
    }
    // assign instance into javascript object
    if(! JS_SetPrivate(cx, obj, (void*)xg)) {
        error("failed assigning xgrab controller to javascript");
        delete xg;
        return JS_FALSE;
    }
    //*rval = OBJECT_TO_JSVAL(obj);
    return JS_TRUE;
}
#endif
JS(js_xgrab_open) {
    func("%u:%s:%s", __LINE__, __FILE__, __FUNCTION__);
    GET_LAYER(XGrabLayer);

    if(argc == 0) {
        return JS_NewNumberValue(cx, lay->open(), rval);
    }

    if(argc == 1) {
        jsint winid = js_get_int(argv[0]);
        return JS_NewNumberValue(cx, lay->open(winid), rval);
    }
    JS_ERROR("Wrong number of arguments");
}
JS(js_xgrab_close) {
    GET_LAYER(XGrabLayer);
    lay->close();
    return JS_TRUE;
}

#endif //WITH_XGRAG
