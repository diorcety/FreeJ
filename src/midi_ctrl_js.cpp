/*  FreeJ Midi Controller
 *  (c) Copyright 2006-2007 Christoph Rudorff <goil@dyne.org>
 *
 * code inspired by seqdemo.c by Matthias Nagorni
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
 */

#include <jsparser.h>
#include <callbacks_js.h> // javascript
#include <jsparser_data.h>

#include <midi_ctrl.h>

#ifdef WITH_ALSA

class MidiControllerJS : public MidiController {
public:
    virtual int event_ctrl(int channel, int param, int value);
    virtual int event_pitch(int channel, int param, int value);
    virtual int event_noteon(int channel, int note, int velocity);
    virtual int event_noteoff(int channel, int note, int velocity);
    virtual int event_pgmchange(int channel, int param, int value);
};

int MidiControllerJS::event_ctrl(int channel, int param, int value) {
    func("midi Control event on Channel\t%2d: %5d %5d (param/value)", channel, param, value);
    if(jsenv == NULL) {
        error("Midi handle action: jsobj is null");
        return(0);
    }
    jsval js_data[] = { channel, param, value };
    JSCall("event_ctrl", 3, js_data);
    return(1);
}

int MidiControllerJS::event_pitch(int channel, int param, int value) {
    func("midi Pitchbender event on Channel\t%2d: %5d %5d   ",  channel, param, value);
    if(jsenv == NULL) {
        error("Midi handle action: jsobj is null");
        return(0);
    }
    jsval js_data[] = { channel, param, value };
    JSCall("event_pitch", 3, js_data);
    return(1);
}

int MidiControllerJS::event_noteon(int channel, int note, int velocity) {
    func("midi Note On event on Channel\t%2d: %5d %5d      ", channel, note, velocity);
    if(jsenv == NULL) {
        error("Midi handle action: jsobj is null");
        return(0);
    }
    jsval js_data[] = { channel, note, velocity };
    JSCall("event_noteon", 3, js_data);
    return(1);
}

int MidiControllerJS::event_noteoff(int channel, int note, int velocity) {
    func("midi Note Off event on Channel\t%2d: %5d      ", channel, note);

    if(jsenv == NULL) {
        error("Midi handle action: jsobj is null");
        return(0);
    }

    jsval js_data[] = { channel, note, velocity };
    JSCall("event_noteoff", 3, js_data);
    return(1);
}

int MidiControllerJS::event_pgmchange(int channel, int param, int value) {
    func("midi PGM change event on Channel\t%2d: %5d %5d ", channel, param, value);
    if(jsenv == NULL) {
        error("Midi handle action: jsobj is null");
        return(0);
    }
    jsval js_data[] = { channel, param, value };
    JSCall("event_pgmchange", 3, js_data);
    return(1);
}

JS(js_midi_ctrl_constructor);

DECLARE_CLASS_GC("MidiController", js_midi_ctrl_class, js_midi_ctrl_constructor, js_ctrl_gc)

JS(midi_connect);
JS(midi_connect_from);
JSFunctionSpec js_midi_ctrl_methods[] = {
    {"connect_from", midi_connect_from, 3},
    //    {"connect", midi_connect, 4 },
    {0}
};

JS(js_midi_ctrl_constructor) {
    func("%u:%s:%s", __LINE__, __FILE__, __FUNCTION__);
    MidiController *midi = new MidiControllerJS();
    // assign instance into javascript object
    // initialize with javascript context
    if(!midi->init(global_environment)) {
        error("failed initializing midi controller");
        delete midi;
        return JS_FALSE;
    }
    if(!JS_SetPrivate(cx, obj, (void*)midi)) {
        error("failed assigning midi controller to javascript");
        delete midi;
        return JS_FALSE;
    }

    // assign the real js object
    midi->jsobj = obj;
    midi->javascript = true;

    *rval = OBJECT_TO_JSVAL(obj);
    return JS_TRUE;
}

JS(midi_connect_from) {
    func("%u:%s:%s argc: %u", __LINE__, __FILE__, __FUNCTION__, argc);
    JS_CHECK_ARGC(3);
    int res = 0;

    MidiController *midi = (MidiController *) JS_GetPrivate(cx, obj);
    if(!midi) {
        error("%u:%s:%s :: Midi core data is NULL",
              __LINE__, __FILE__, __FUNCTION__);
        return JS_FALSE;
    }

    // int snd_seq_connect_to(snd_seq_t * seq, int myport, int dest_client, int dest_port)
    jsint myport = js_get_int(argv[0]);
    jsint dest_client = js_get_int(argv[1]);
    jsint dest_port = js_get_int(argv[2]);

    res = midi->connect_from(int(myport), int(dest_client), int(dest_port));

    return JS_NewNumberValue(cx, res, rval);
}

#endif //WITH_ALSA
