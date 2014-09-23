/*  FreeJ OSC controller
 *  (c) Copyright 2008 Denis Rojo <jaromil@dyne.org>
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
 * "$Id:$"
 *
 */

#include <callbacks_js.h>

#include <osc_ctrl.h>

#include <algorithm>

class JsCommand : public Entry {
public:
    // name is function
    jsval function;
    char *format;
    int argc;
    jsval *argv;

};

static void osc_error_handler(int num, const char *msg, const char *path) {
    error("OSC server error %d in path %s: %s\n", num, path, msg);
}

static int osc_command_handler(const char *path, const char *types,
                               lo_arg **argv, int argc,
                               void *data, void *user_data) {

    OscController *osc = (OscController*)user_data;

    func("OSC call path %s type %s", path, types);

    LockedLinkList<Entry> list = osc->commands_handled.getLock();

    LockedLinkList<Entry>::iterator it = std::find_if(list.begin(), list.end(), [&] (Entry*cmd) {
        return cmd->getName() == path;
    });

    // check that path is handled
    if(it == list.end()) {
        warning("OSC path %s called, but no method is handling it", path);
        return -1;
    }

    OscCommand *cmd = (OscCommand *)*it;
    func("OSC path handled by %s", cmd->js_cmd);

    // check that types are matching
    if(strcmp(types, cmd->proto_cmd) != 0) {
        error("OSC path %s called with wrong types: \"%s\" instead of \"%s\"",
              cmd->getName().c_str(), types, cmd->proto_cmd);
        return -1;
    }


    func("OSC call to %s with argc %u", cmd->js_cmd, argc);

    // TODO: arguments are not supported
    // the code below correctly parses them, but then
    // the jsval is not valid as such in JS_CallFunction

    JsCommand *jscmd = new JsCommand();
    jscmd->setName(cmd->js_cmd);
    jscmd->format = cmd->proto_cmd;
    jscmd->argc = argc;
    jscmd->argv = (jsval*)calloc(argc + 1, sizeof(jsval));

    // put values into a jsval array
    int c;
    for(c = 0; c < argc; c++) {
        switch(types[c]) {
        case 'i':
            func("OSC arg %u is int: %i", c, argv[c]->i32);
            JS_NewNumberValue(osc->jsenv, (double)argv[c]->i32, &jscmd->argv[c]);
            break;
        case 'f':
            func("OSC arg %u is float: %.2f", c, argv[c]->f);
            JS_NewNumberValue(osc->jsenv, (double)argv[c]->f, &jscmd->argv[c]);
            //      jsargv[c] = DOUBLE_TO_JSVAL((double)argv[c]->f);
            // TODO
            break;
        case 's': {
            func("OSC arg %u is string: %s", c, argv[c]);
            JSString *tmp = JS_NewStringCopyZ(osc->jsenv, (const char*)argv[c]);
            jscmd->argv[c] = STRING_TO_JSVAL(tmp);
            //      JS_NewString
            //      jsargv[c] = STRING_TO_JSVAL(argv[c]->s);
            // TODO
            break;
        }
        default:
            error("OSC unrecognized type '%c' in arg %u of path %s",
                  types[c], c, cmd->getName().c_str());
        }
    }

    osc->commands_pending.getLock().push_back(jscmd);

    return 1;
}

// Implementation for JS

class OscControllerJS : public OscController {
public:
    virtual int dispatch();
};

int OscControllerJS::dispatch() {
    int c = 0;
    int res;
    // execute pending comamnds (javascript calls)
    LockedLinkList<Entry> list = commands_pending.getLock();
    while(list.size()) {
        JsCommand *jscmd = (JsCommand *)list.front();
        //    int res = JS_CallFunctionValue
        //      (jsenv, jsobj, jscmd->function, jscmd->argc, jscmd->argv, &ret);

        func("OSC controller dispatching %s(%s)", jscmd->getName().c_str(), jscmd->format);
        res = JSCall(jscmd->getName().c_str(), jscmd->argc, jscmd->argv);
        if(res) func("OSC dispatched call to %s", jscmd->getName().c_str());
        else error("OSC failed JSCall to %s", jscmd->getName().c_str());


        free(jscmd->argv); // must free previous callod on argv
        delete jscmd;
        list.pop_front();
        c++;
    }
    return c;
}

/////// Javascript OscController
JS(js_osc_ctrl_constructor);

DECLARE_CLASS("OscController", js_osc_ctrl_class, js_osc_ctrl_constructor)

JS(js_osc_ctrl_start);
JS(js_osc_ctrl_stop);
JS(js_osc_ctrl_add_method);
JS(js_osc_ctrl_send_to);
JS(js_osc_ctrl_send);
//JS(js_osc_ctrl_rem_method);

JSFunctionSpec js_osc_ctrl_methods[] = {
    {"start",      js_osc_ctrl_start,      0},
    {"stop",       js_osc_ctrl_stop,       0},
    {"add_method", js_osc_ctrl_add_method, 3},
    {"send_to",    js_osc_ctrl_send_to,    2},
    {"send",       js_osc_ctrl_send,       8},
    //  {"rem_method", js_osc_ctrl_rem_method, 2},
    {0}
};

JS(js_osc_ctrl_constructor) {
    func("%u:%s:%s", __LINE__, __FILE__, __FUNCTION__);
    char excp_msg[MAX_ERR_MSG + 1];
    char *port;

    OscController *osc = (OscController *)new OscControllerJS();
    //JS_SetContextThread(cx);
    JS_BeginRequest(cx);
    // assign instance into javascript object
    if(!JS_SetPrivate(cx, obj, (void*)osc)) {
        sprintf(excp_msg, "failed assigning OSC controller to javascript");
        goto error;
    }
    // initialize with javascript context
    if(!osc->init(global_environment)) {
        sprintf(excp_msg, "failed initializing OSC controller");
        goto error;
    }

    // assign the real js object (DEPRECATED)
    // TODO - use listeners
    osc->jsobj = obj;
    osc->jsenv = cx;

    osc->javascript = true;

    port = js_get_string(argv[0]);
    strncpy(osc->port, port, 64);

    osc->srv = lo_server_thread_new(osc->port, osc_error_handler);

    // register method handler
    // here we register only one method handler
    // as we use our own marshaller instead of liblo's
    lo_server_thread_add_method(osc->srv, NULL, NULL, osc_command_handler, osc);

    notice("OSC controller created: %s", lo_server_thread_get_url(osc->srv));

    *rval = OBJECT_TO_JSVAL(obj);
    JS_EndRequest(cx);
    //JS_ClearContextThread(cx);
    return JS_TRUE;

error:

    JS_ReportErrorNumber(cx, JSFreej_GetErrorMessage, NULL,
                         JSSMSG_FJ_CANT_CREATE, __func__, excp_msg);
    //  cx->newborn[GCX_OBJECT] = NULL;
    JS_EndRequest(cx);
    //JS_ClearContextThread(cx);
    delete osc;
    return JS_FALSE;
}

JS(js_osc_ctrl_start) {
    func("%u:%s:%s argc: %u", __LINE__, __FILE__, __FUNCTION__, argc);
    //JS_SetContextThread(cx);
    JS_BeginRequest(cx);
    OscController *osc = (OscController *)JS_GetPrivate(cx, obj);
    if(!osc)
        JS_ERROR("OSC core data is NULL");
    JS_EndRequest(cx);
    //JS_ClearContextThread(cx);
    lo_server_thread_start(osc->srv);

    act("OSC controller listening on port %s", osc->port);

    return JS_TRUE;
}

JS(js_osc_ctrl_stop) {
    func("%u:%s:%s argc: %u", __LINE__, __FILE__, __FUNCTION__, argc);
    //JS_SetContextThread(cx);
    JS_BeginRequest(cx);
    OscController *osc = (OscController *)JS_GetPrivate(cx, obj);
    if(!osc)
        JS_ERROR("OSC core data is NULL");
    JS_EndRequest(cx);
    //JS_ClearContextThread(cx);
    lo_server_thread_stop(osc->srv);

    act("OSC controller stopped");

    return JS_TRUE;
}

JS(js_osc_ctrl_add_method) {
    func("%u:%s:%s argc: %u", __LINE__, __FILE__, __FUNCTION__, argc);

    //JS_SetContextThread(cx);
    JS_BeginRequest(cx);
    JS_CHECK_ARGC(3);

    OscController *osc = (OscController *)JS_GetPrivate(cx, obj);
    if(!osc) JS_ERROR("OSC core data is NULL");
    JS_EndRequest(cx);
    //JS_ClearContextThread(cx);
    char *osc_cmd = js_get_string(argv[0]);

    char *proto_cmd = js_get_string(argv[1]);

    char *js_cmd = js_get_string(argv[2]);

    // queue metods in commands_handled linklist
    OscCommand *cmd = new OscCommand();
    cmd->setName(osc_cmd);
    strncpy(cmd->proto_cmd, proto_cmd, 128);
    strncpy(cmd->js_cmd, js_cmd, 512);
    osc->commands_handled.getLock().push_back(cmd);

    act("OSC method \"%s\" with args \"%s\" binded to %s",
        osc_cmd, proto_cmd, js_cmd);

    return JS_TRUE;
}

JS(js_osc_ctrl_send_to) {
    func("%u:%s:%s argc: %u", __LINE__, __FILE__, __FUNCTION__, argc);

    warning("%s TODO", __PRETTY_FUNCTION__);
    //JS_SetContextThread(cx);
    JS_BeginRequest(cx);
    JS_CHECK_ARGC(2);

    OscController *osc = (OscController *)JS_GetPrivate(cx, obj);
    if(!osc)
        JS_ERROR("OSC core data is NULL");

    JS_EndRequest(cx);
    //JS_ClearContextThread(cx);
    char *host = js_get_string(argv[0]);
    char *port = js_get_string(argv[1]);

    if(osc->sendto) lo_address_free(osc->sendto);
    osc->sendto = lo_address_new(host, port);

    act("OSC controller sends messages to %s port %s", host, port);

    return JS_TRUE;
    // TODO
}

JS(js_osc_ctrl_send) {
    func("%u:%s:%s argc: %u", __LINE__, __FILE__, __FUNCTION__, argc);
    //JS_SetContextThread(cx);
    JS_BeginRequest(cx);

    JS_CHECK_ARGC(2);

    OscController *osc = (OscController *)JS_GetPrivate(cx, obj);
    if(!osc)
        JS_ERROR("OSC core data is NULL");
    JS_EndRequest(cx);
    //JS_ClearContextThread(cx);
    // minimum arguments: path and type
    char *path = js_get_string(argv[0]);
    char *type = js_get_string(argv[1]);

    func("generating OSC message path %s type %s", path, type);
    // we use the internal functions:
    // int lo_send_message_from
    // (lo_address a, lo_server from, const char *path, lo_message msg)

    osc->outmsg = lo_message_new();

    // put values into a jsval array
    unsigned int c;
    for(c = 2; c < argc; c++) {

        switch(type[c]) {
        case 'i': {
            jsint i = js_get_int(argv[c]);
            func("OSC add message arg %i with value %i", c, i);
            lo_message_add_int32(osc->outmsg, i);
        }
        break;
        case 'f': {
            jsdouble f = js_get_double(argv[c]);
            func("OSC add message arg %u with value %.2f", c, f);
            lo_message_add_float(osc->outmsg, (float)f);
        }
        break;
        case 's': {
            char *s = js_get_string(argv[c + 1]);
            func("OSC add message arg %u with value %s", c, s);
            lo_message_add_string(osc->outmsg, s);
        }
        break;
        default:
            error("OSC unrecognized type '%c' in arg %u", type[c], c);
        }

    } // foreach type in format

    lo_send_message_from((void*)osc->sendto, osc->srv, path, osc->outmsg);
    lo_message_free(osc->outmsg);

    return(JS_TRUE);

}

//JS(js_osc_ctrl_rem_method) {

// remove methods from commands_pending linklist

//}