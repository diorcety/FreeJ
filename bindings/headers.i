%define CINCLUDE(a)
%{
#include a
%}
%enddef

%include FREEJ_INCLUDE(config.h)

CINCLUDE(FREEJ_INCLUDE(context.h))
CINCLUDE(FREEJ_INCLUDE(linklist.h))
CINCLUDE(FREEJ_INCLUDE(entity.h))
CINCLUDE(FREEJ_INCLUDE(screen.h))
CINCLUDE(FREEJ_INCLUDE(color.h))
CINCLUDE(FREEJ_INCLUDE(video_encoder.h))
#if defined(WITH_OGGTHEORA)
CINCLUDE(FREEJ_INCLUDE(oggtheora_encoder.h))
#endif

CINCLUDE(FREEJ_INCLUDE(layer.h))
CINCLUDE(FREEJ_INCLUDE(blitter.h))
CINCLUDE(FREEJ_INCLUDE(fps.h))
CINCLUDE(FREEJ_INCLUDE(controller.h))
CINCLUDE(FREEJ_INCLUDE(callback.h))
CINCLUDE(FREEJ_INCLUDE(logging.h))
CINCLUDE(FREEJ_INCLUDE(filter.h))
CINCLUDE(FREEJ_INCLUDE(frei0r_freej.h))
CINCLUDE(FREEJ_INCLUDE(freeframe_freej.h))
CINCLUDE(FREEJ_INCLUDE(filter_instance.h))
CINCLUDE(FREEJ_INCLUDE(frei0r_instance.h))
CINCLUDE(FREEJ_INCLUDE(freeframe_instance.h))

#if defined(WITH_AALIB)
CINCLUDE(FREEJ_INCLUDE(aa_screen.h))
#endif
CINCLUDE(FREEJ_INCLUDE(sdl_screen.h))
CINCLUDE(FREEJ_INCLUDE(soft_screen.h))
CINCLUDE(FREEJ_INCLUDE(sdlgl_screen.h))
CINCLUDE(FREEJ_INCLUDE(gl_screen.h))

CINCLUDE(FREEJ_INCLUDE(cairo_layer.h))
CINCLUDE(FREEJ_INCLUDE(generator_layer.h))
#if defined(WITH_GOOM)
CINCLUDE(FREEJ_INCLUDE(goom_layer.h))
#endif
CINCLUDE(FREEJ_INCLUDE(text_layer.h))
CINCLUDE(FREEJ_INCLUDE(image_layer.h))
CINCLUDE(FREEJ_INCLUDE(video_layer.h))
CINCLUDE(FREEJ_INCLUDE(geo_layer.h))
#if defined(WITH_OPENCV)
CINCLUDE(FREEJ_INCLUDE(opencv_cam_layer.h))
#endif
CINCLUDE(FREEJ_INCLUDE(xscreensaver_layer.h))
#if defined(WITH_UNICAP)
CINCLUDE(FREEJ_INCLUDE(unicap_layer.h))
#endif
CINCLUDE(FREEJ_INCLUDE(v4l2_layer.h))
#if defined(WITH_XGRAB)
CINCLUDE(FREEJ_INCLUDE(xgrab_layer.h))
#endif

CINCLUDE(FREEJ_INCLUDE(sdl_controller.h))
CINCLUDE(FREEJ_INCLUDE(kbd_ctrl.h))
CINCLUDE(FREEJ_INCLUDE(trigger_ctrl.h))
CINCLUDE(FREEJ_INCLUDE(midi_ctrl.h))
CINCLUDE(FREEJ_INCLUDE(vimo_ctrl.h))
CINCLUDE(FREEJ_INCLUDE(joy_ctrl.h))
CINCLUDE(FREEJ_INCLUDE(mouse_ctrl.h))
CINCLUDE(FREEJ_INCLUDE(wiimote_ctrl.h))
CINCLUDE(FREEJ_INCLUDE(osc_ctrl.h))
#if defined(WITH_AUDIO)
CINCLUDE(FREEJ_INCLUDE(audio_collector.h))
CINCLUDE(FREEJ_INCLUDE(audio_layer.h))
#endif

#if defined(WITH_CONSOLE)
CINCLUDE(FREEJ_INCLUDE(console_ctrl.h))
#endif
