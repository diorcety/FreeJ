%module(directors="1") freej
#pragma SWIG nowarn=322

%include <std_shared_ptr.i>
%include <std_string.i>

%include "freej_sharedptr.i"

%{
#include "context.h"
#include "linklist.h"
#include "entity.h"
#include "screen.h"

#include "layer.h"
#include "blitter.h"
#include "fps.h"
#include "controller.h"
#include "callback.h"
#include "logging.h"

#include "sdl_screen.h"
#include "soft_screen.h"
#include "sdlgl_screen.h"
#include "gl_screen.h"

#include "generator_layer.h"
#include "text_layer.h"
#include "video_layer.h"
#include "geo_layer.h"
#include "opencv_cam_layer.h"
#include "xscreensaver_layer.h"

#include "sdl_controller.h"
#include "kbd_ctrl.h"
#include "trigger_ctrl.h"
#include "midi_ctrl.h"
#include "vimo_ctrl.h"
#include "joy_ctrl.h"
#include "mouse_ctrl.h"
#include "wiimote_ctrl.h"
#include "osc_ctrl.h"
#ifdef WITH_AUDIO
#include "audio_collector.h"
#endif
#include "console_ctrl.h"


%}

//we need this for ifdefs in included headers
%include <config.h>

//we need this for macro definitions appearing in included headers
%include <factory.h>

%import "inttypes.i"

//ditch some of the defines we have that don't need to be exposed to the user
%ignore THREADSAFE;
%ignore MAX_ERR_MSG;
%ignore MAX_COMPLETION;
%ignore MAX_HEIGHT;
%ignore MAX_WIDTH;
//from screen.h
%ignore rchan;
%ignore red_bitmask;
%ignore bchan;
%ignore blue_bitmask;
%ignore gchan;
%ignore green_bitmask;
%ignore achan;
%ignore alpha_bitmask;

%immutable layers_description;
%immutable Parameter::description;

%ignore Linklist::operator[];
%ignore Linklist::getLock;
%ignore Layer::parameters;
%ignore Controller::listeners;
%ignore ViewPort::encoders;
%ignore ViewPort::layers;
%ignore Layer::filters;
%ignore Blitter::blitlist;
%ignore Blit::parameters;
%ignore FilterInstance::parameters;
%ignore Context::controllers;
%ignore Context::filters;
%ignore Context::generators;
%ignore Context::screens;

%apply unsigned long { uint16_t };

/* Macros that can be redefined for other languages */
/* freej_entry_typemap_in: to be able to map an Entry* to TypeName* */
#define freej_entry_typemap_in(TypeName)


/* Language specific typemaps */
#if defined(SWIGPYTHON)
  %include "pypre.i"
#elif defined(SWIGRUBY)
  %include "rbpre.i"
#elif defined(SWIGJAVA)
  %include "javapre.i"
#elif defined(SWIGPERL)
  %include "perlpre.i"
#endif

/* Entry/Derived typemaps so we can use entries when the children
   are required - Should not be needed any more..

freej_entry_typemap_in(Filter);
freej_entry_typemap_in(Layer);
freej_entry_typemap_in(Controller);
freej_entry_typemap_in(Encoder);
*/

/* for Linklist.search (note normally you want to add
   support for dict like access for specific languages) */
%apply int * OUTPUT { int *idx };

/* This dont compile... */
%ignore Layer::layer_gc;
%ignore Controller::JSCall;
/* ControllerListener is meant to be used internally by the Controller
   and shouldn't be accessed directly */
%ignore ControllerListener;
%ignore JSyncThread;

/* Now the freej headers.. */
%include "entity.h"
%include "freej.h"
%include "jutils.h"
%include "context.h"
%include "screen.h"
%template(ScreenFactory) Factory<ViewPort>;

#ifdef WITH_AUDIO
%include "audio_collector.h"
#endif

%ignore DumbCallback;
%feature("director") DumbCall;
%include "callback.h"

/* swig on amd64 doesn't play nice with va_args, just ignore vprintlog because
 * we don't need it in bindings */
%ignore Logger::vprintlog;
%ignore GlobalLogger::vprintlog;
%feature("director") WrapperLogger;
%include "logging.h"

%include "entity.h"
%include "linklist.h"
%template(EntryLinkList) Linklist<Entry>;

%include "parameter.h"
%template(ParameterLinkList) Linklist<Parameter>;

%include "filter.h"
%template(FilterLinkList) Linklist<Filter>;
%include "filter_instance.h"
%template(FilterInstanceLinkList) Linklist<FilterInstance>;

%include "blitter.h"
%include "plugger.h"
// %include "jsync.h"


// layers
%include "fps.h"
%template(LayerLinkList) Linklist<Layer>;
%include "layer.h"

%include "generator_layer.h"
%include "text_layer.h"
%include "video_layer.h"
%include "geo_layer.h"
%include "opencv_cam_layer.h"
%include "xscreensaver_layer.h"


// screens...
%template(ScreenLinkList) Linklist<ViewPort>;
%include "screen.h"

%include "sdl_screen.h"
%include "soft_screen.h"
#if defined(WITH_OPENGL)
%include "sdlgl_screen.h"
%include "gl_screen.h"
#endif
#if defined(WITH_COCOA)
%include "CVScreen.h"
#endif
			 
// controllers
%feature("director") Controller;
%include "controller.h"
%template(ControllerLinkList) Linklist<Controller>;
// extends virtual methods of the Controllers to be overloadable (as callbacks)

// this is needed because glues Controller class (expected by context) with some
// controller classes below
%feature("director") SdlController;
%include "sdl_controller.h"

%feature("director") KbdController;
%include "kbd_ctrl.h"

%feature("director") TriggerController;
%include "trigger_ctrl.h"

%feature("director") JoyController;
%include "joy_ctrl.h"

%feature("director") MidiController;
%include "midi_ctrl.h"

%feature("director") MouseController;
%include "mouse_ctrl.h"

//%feature("director") OscController;
//%include "osc_ctrl.h"

%feature("director") WiiController;
%include "wiimote_ctrl.h"

%feature("director") VimoController;
%include "vimo_ctrl.h"

%feature("director") ConsoleController;
%include "console_ctrl.h"

/* Language specific extensions */
#if defined(SWIGPYTHON)
  %include "pypost.i"
#elif defined(SWIGRUBY)
  %include "rbpost.i"
#elif defined(SWIGLUA)
  %include "luapost.i"
#endif

// SWIGPERL5, SWIGRUBY, SWIGJAVA, SWIGLUA...
