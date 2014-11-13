%module(directors="1") freej
#pragma SWIG nowarn=322

#ifdef IN_FREEJ
%include "macros.i"
#else
%include <freej_swig/macros.i>
#endif

%include FREEJ_SWIG_INCLUDE(headers.i)

%apply unsigned long { uint16_t };

/* Macros that can be redefined for other languages */
/* freej_entry_typemap_in: to be able to map an Entry* to TypeName* */
#define freej_entry_typemap_in(TypeName)

/* Language specific typemaps */
#if defined(SWIGPYTHON)
  %include FREEJ_SWIG_INCLUDE(pypre.i)
#elif defined(SWIGRUBY)
  %include FREEJ_SWIG_INCLUDE(rbpre.i)
#elif defined(SWIGJAVA)
  %include FREEJ_SWIG_INCLUDE(javapre.i)
#elif defined(SWIGLUA)
  %include FREEJ_SWIG_INCLUDE(javalua.i)
#elif defined(SWIGPERL)
  %include FREEJ_SWIG_INCLUDE(perlpre.i)
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
%include "std_string.i"
%include FREEJ_SWIG_INCLUDE(freej_sharedptr_pre.i)
%include FREEJ_SWIG_INCLUDE(freej_list_pre.i)

////////// IGNORES

%ignore Logger::vprintlog;
%ignore GlobalLogger::vprintlog;

////////// MAIN

//we need this for macro definitions appearing in included headers
%include FREEJ_INCLUDE(factory.h)

%import "inttypes.i"

/* Now the freej headers.. */
%include FREEJ_INCLUDE(entity.h)
%include FREEJ_INCLUDE(freej.h)
%include FREEJ_INCLUDE(jutils.h)
%include FREEJ_INCLUDE(jsync.h)
%include FREEJ_INCLUDE(context.h)
%include FREEJ_INCLUDE(screen.h)
%include FREEJ_INCLUDE(color.h)
%include FREEJ_INCLUDE(video_encoder.h)

#if defined(WITH_AUDIO)
%include FREEJ_INCLUDE(audio_collector.h)
#endif

%include FREEJ_INCLUDE(callback.h)
%include FREEJ_INCLUDE(logging.h)
%include FREEJ_INCLUDE(entity.h)
%include FREEJ_INCLUDE(linklist.h)
%include FREEJ_INCLUDE(parameter.h)
%include FREEJ_INCLUDE(parameter_instance.h)
%include FREEJ_INCLUDE(filter.h)
%include FREEJ_INCLUDE(frei0r_freej.h)
%include FREEJ_INCLUDE(freeframe_freej.h)
%include FREEJ_INCLUDE(filter_instance.h)
%include FREEJ_INCLUDE(frei0r_instance.h)
%include FREEJ_INCLUDE(freeframe_instance.h)

%include FREEJ_INCLUDE(blitter.h)
%include FREEJ_INCLUDE(plugger.h)
%include FREEJ_INCLUDE(fps.h)

#if defined(WITH_OGGTHEORA)
%include FREEJ_INCLUDE(oggtheora_encoder.h)
#endif

// layers...
%include FREEJ_INCLUDE(layer.h)
#if defined(WITH_AUDIO)
%include FREEJ_INCLUDE(audio_layer.h)
#endif
%include FREEJ_INCLUDE(cairo_layer.h)
%include FREEJ_INCLUDE(generator_layer.h)
#if defined(WITH_GOOM)
%include FREEJ_INCLUDE(goom_layer.h)
#endif
%include FREEJ_INCLUDE(text_layer.h)
%include FREEJ_INCLUDE(image_layer.h)
%include FREEJ_INCLUDE(video_layer.h)
%include FREEJ_INCLUDE(geo_layer.h)
#if defined(WITH_OPENCV)
%include FREEJ_INCLUDE(opencv_cam_layer.h)
#endif
%include FREEJ_INCLUDE(xscreensaver_layer.h)
#if defined(WITH_UNICAP)
%include FREEJ_INCLUDE(unicap_layer.h)
#endif
%include FREEJ_INCLUDE(v4l2_layer.h)
#if defined(WITH_XGRAB)
%include FREEJ_INCLUDE(xgrab_layer.h)
#endif

// screens...
%include FREEJ_INCLUDE(screen.h)
#if defined(WITH_AALIB)
%include FREEJ_INCLUDE(aa_screen.h)
#endif
%include FREEJ_INCLUDE(sdl_screen.h)
%include FREEJ_INCLUDE(soft_screen.h)
#if defined(WITH_OPENGL)
%include FREEJ_INCLUDE(sdlgl_screen.h)
%include FREEJ_INCLUDE(gl_screen.h)
#endif
#if defined(WITH_COCOA)
%include FREEJ_INCLUDE(CVScreen.h)
#endif

// controllers
%include FREEJ_INCLUDE(controller.h)
%include FREEJ_INCLUDE(sdl_controller.h)
%include FREEJ_INCLUDE(kbd_ctrl.h)
%include FREEJ_INCLUDE(trigger_ctrl.h)
%include FREEJ_INCLUDE(joy_ctrl.h)
%include FREEJ_INCLUDE(midi_ctrl.h)
%include FREEJ_INCLUDE(mouse_ctrl.h)
%include FREEJ_INCLUDE(wiimote_ctrl.h)
%include FREEJ_INCLUDE(vimo_ctrl.h)
#if defined(WITH_CONSOLE)
%include FREEJ_INCLUDE(console_ctrl.h)
#endif

//////////// POST

%template(ScreenFactory) Factory<ViewPort>;

%include FREEJ_SWIG_INCLUDE(freej_list_post.i)
%include FREEJ_SWIG_INCLUDE(freej_sharedptr_post.i)

/* Language specific extensions */
#if defined(SWIGPYTHON)
  %include FREEJ_SWIG_INCLUDE(pypost.i)
#elif defined(SWIGRUBY)
  %include FREEJ_SWIG_INCLUDE(rbpost.i)
#elif defined(SWIGJAVA)
  %include FREEJ_SWIG_INCLUDE(javapost.i)
#elif defined(SWIGLUA)
  %include FREEJ_SWIG_INCLUDE(luapost.i)
#elif defined(SWIGPERL)
  %include FREEJ_SWIG_INCLUDE(luaperl.i)
#endif

// SWIGPERL5, SWIGRUBY, SWIGJAVA, SWIGLUA...
