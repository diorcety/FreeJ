%module(directors="1") freej
#pragma SWIG nowarn=322

%{
#include "context.h"
#include "linklist.h"
#include "entity.h"
#include "screen.h"
#include "color.h"
#include "video_encoder.h"
#if defined(WITH_OGGTHEORA)
#include "oggtheora_encoder.h"
#endif

#include "layer.h"
#include "blitter.h"
#include "fps.h"
#include "controller.h"
#include "callback.h"
#include "logging.h"
#include "filter.h"
#include "frei0r_freej.h"
#include "freeframe_freej.h"
#include "filter_instance.h"
#include "frei0r_instance.h"
#include "freeframe_instance.h"

#if defined(WITH_AALIB)
#include "aa_screen.h"
#endif
#include "sdl_screen.h"
#include "soft_screen.h"
#include "sdlgl_screen.h"
#include "gl_screen.h"

#include "cairo_layer.h"
#include "generator_layer.h"
#if defined(WITH_GOOM)
#include "goom_layer.h"
#endif
#include "text_layer.h"
#include "image_layer.h"
#include "video_layer.h"
#include "geo_layer.h"
#if defined(WITH_OPENCV)
#include "opencv_cam_layer.h"
#endif
#include "xscreensaver_layer.h"
#if defined(WITH_UNICAP)
#include "unicap_layer.h"
#endif
#include "v4l2_layer.h"
#if defined(WITH_XGRAB)
#include "xgrab_layer.h"
#endif

#include "sdl_controller.h"
#include "kbd_ctrl.h"
#include "trigger_ctrl.h"
#include "midi_ctrl.h"
#include "vimo_ctrl.h"
#include "joy_ctrl.h"
#include "mouse_ctrl.h"
#include "wiimote_ctrl.h"
#include "osc_ctrl.h"
#if defined(WITH_AUDIO)
#include "audio_collector.h"
#include "audio_layer.h"
#endif

#if defined(WITH_CONSOLE)
#include "console_ctrl.h"
#endif
%}

///////// PRE

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

%feature("director") DumbCall;
%feature("director") WrapperLogger;
%feature("director") Controller;
%feature("director") SdlController;
%feature("director") KbdController;
%feature("director") TriggerController;
%feature("director") JoyController;
%feature("director") MidiController;
%feature("director") MouseController;
%feature("director") MouseController;
%feature("director") WiiController;
%feature("director") VimoController;

//we need this for ifdefs in included headers
%include "config.h"

%include "std_string.i"
%include "freej_sharedptr_pre.i"
%include "freej_list_pre.i"

////////// IGNORES

%ignore Logger::vprintlog;
%ignore GlobalLogger::vprintlog;

////////// MAIN

//we need this for macro definitions appearing in included headers
%include "factory.h"

%import "inttypes.i"

/* Now the freej headers.. */
%include "entity.h"
%include "freej.h"
%include "jutils.h"
%include "jsync.h"
%include "context.h"
%include "screen.h"
%include "color.h"
%include "video_encoder.h"

#if defined(WITH_AUDIO)
%include "audio_collector.h"
#endif

%include "callback.h"
%include "logging.h"
%include "entity.h"
%include "linklist.h"
%include "parameter.h"
%include "parameter_instance.h"
%include "filter.h"
%include "frei0r_freej.h"
%include "freeframe_freej.h"
%include "filter_instance.h"
%include "frei0r_instance.h"
%include "freeframe_instance.h"

%include "blitter.h"
%include "plugger.h"
%include "fps.h"

#if defined(WITH_OGGTHEORA)
%include "oggtheora_encoder.h"
#endif

// layers...
%include "layer.h"
#if defined(WITH_AUDIO)
%include "audio_layer.h"
#endif
%include "cairo_layer.h"
%include "generator_layer.h"
#if defined(WITH_GOOM)
%include "goom_layer.h"
#endif
%include "text_layer.h"
%include "image_layer.h"
%include "video_layer.h"
%include "geo_layer.h"
#if defined(WITH_OPENCV)
%include "opencv_cam_layer.h"
#endif
%include "xscreensaver_layer.h"
#if defined(WITH_UNICAP)
%include "unicap_layer.h"
#endif
%include "v4l2_layer.h"
#if defined(WITH_XGRAB)
%include "xgrab_layer.h"
#endif

// screens...
%include "screen.h"
#if defined(WITH_AALIB)
%include "aa_screen.h"
#endif
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
%include "controller.h"
%include "sdl_controller.h"
%include "kbd_ctrl.h"
%include "trigger_ctrl.h"
%include "joy_ctrl.h"
%include "midi_ctrl.h"
%include "mouse_ctrl.h"
%include "wiimote_ctrl.h"
%include "vimo_ctrl.h"
#if defined(WITH_CONSOLE)
%include "console_ctrl.h"
#endif

//////////// POST

%template(ScreenFactory) Factory<ViewPort>;

%include "freej_list_post.i"
%include "freej_sharedptr_post.i"

/* Language specific extensions */
#if defined(SWIGPYTHON)
  %include "pypost.i"
#elif defined(SWIGRUBY)
  %include "rbpost.i"
#elif defined(SWIGLUA)
  %include "luapost.i"
#endif

// SWIGPERL5, SWIGRUBY, SWIGJAVA, SWIGLUA...
