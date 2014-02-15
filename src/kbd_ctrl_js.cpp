/*
 *  kbd_ctrl_js.cpp
 *  freej
 *
 *  Created by xant on 8/22/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include <callbacks_js.h>
#include <jsparser_data.h>
#include <kbd_ctrl.h>
#include <config.h>

class KbdControllerJS: public KbdController {
public:
    virtual int key_event(const char *state, bool shift, bool ctrl, bool alt, bool num, const char *keyname);
};

int KbdControllerJS::key_event(const char *state, bool shift, bool ctrl, bool alt, bool num, const char *keyname) {
    Uint16 uni[] = {keysym->unicode, 0};
    //#snprintf(uni, 2, "X %s X", (char*)&keysym->unicode);
    // universal call
    if(JSCall("key", 7, "buusWuu",
              event.key.state,
              keysym->scancode,
              keysym->sym,
              SDL_GetKeyName(keysym->sym),
              uni,
              keysym->mod,
              event.key.which
             ))
        return 1; // returned true, we are done!

    //Uint16 keysym->unicode
    //char * SDL_GetKeyName(keysym->sym);
    //func("KB u: %u / ks: %s", keysym->unicode, SDL_GetKeyName(keysym->sym));

    // funcname = "state_[shift_][ctrl_][alt_][num_]keyname"
    if(strlen(keyname)) {
        sprintf(funcname, "%s_%s%s%s%s%s",
                state,
                (shift ? "shift_" : ""),
                (ctrl ?  "ctrl_"  : ""),
                (alt ?   "alt_"   : ""),
                (num ?   "num_"   : ""),
                keyname);

        func("%s calling method %s()", __func__, funcname);
        jsval fval = JSVAL_VOID;
        return JSCall(funcname, 0, &fval);
    }
    return 0;
}

JSFunctionSpec js_kbd_ctrl_methods[] = {
// idee: dis/enable repeat
    {0}
};

/////// Javascript KeyboardController
JS(js_kbd_ctrl_constructor);

DECLARE_CLASS("KeyboardController", js_kbd_ctrl_class, js_kbd_ctrl_constructor);

/* XXX - this is exactly the same code we have in trigger_ctrl.cpp ...
         we should try to avoid duplicating code around */
JS(js_kbd_ctrl_constructor) {
    func("%u:%s:%s", __LINE__, __FILE__, __FUNCTION__);

    KbdController *kbd = new KbdControllerJS();
    if(!kbd)
        return JS_FALSE;

    JS_BeginRequest(cx);
    // initialize with javascript context
    if(!kbd->initialized) {
        if(! kbd->init(global_environment)) {
            error("failed initializing keyboard controller");
            JS_EndRequest(cx);
            return JS_FALSE;
        }
        // mark that this controller was initialized by javascript
        kbd->javascript = true;
    }

    // assign instance into javascript object
    if(!JS_SetPrivate(cx, obj, (void*)kbd)) {
        error("failed assigning kbd controller to javascript");
        JS_EndRequest(cx);
        return JS_FALSE;
    }

    *rval = OBJECT_TO_JSVAL(obj);
    kbd->add_listener(new ControllerListener(cx, obj));
    JS_EndRequest(cx);
    return JS_TRUE;
}
