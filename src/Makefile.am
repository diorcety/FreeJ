AM_CFLAGS = $(FREEJ_CFLAGS) -DPREFIX="\"$(prefix)\"" -DDATADIR="\"$(datadir)\""
AM_CXXFLAGS = $(FREEJ_CFLAGS) -DPREFIX="\"$(prefix)\"" -DDATADIR="\"$(datadir)\"" \
			  -DPACKAGE_DATA_DIR="\"$(PACKAGE_DATA_DIR)\"" \
			  -DPACKAGE_LIB_DIR="\"$(PACKAGE_LIB_DIR)\""

SUBDIRS = include

SOURCES = \
	context.cpp		layer.cpp	\
	fps.cpp		timelapse.cpp	blitter.cpp	\
	blit.cpp		blit_instance.cpp \
	sdl_blits.cpp		linear_blits.cpp \
	iterator.cpp		entity.cpp	\
	jsync.cpp		closure.cpp	\
	callback.cpp		cpu_accel.cpp	\
	plugin.cpp		plugger.cpp	\
	frei0r.cpp		frei0r_instance.cpp \
	freeframe.cpp		freeframe_instance.cpp \
	filter.cpp		parameter.cpp   \
	filter_instance.cpp	parameter_instance.cpp \
	jutils.cpp		fastmemcpy.cpp  \
	ringbuffer.cpp  	convertvid.cpp  \
	logging.cpp geometry.cpp color.cpp \
\
	tvfreq.c		unicap_layer.cpp \
	v4l2_layer.cpp \
	video_layer.cpp		image_layer.cpp \
	text_layer.cpp 		generator_layer.cpp \
	geo_layer.cpp		flash_layer.cpp \
	xscreensaver_layer.cpp   \
	xgrab_layer.cpp		opencv_cam_layer.cpp \
	cairo_layer.cpp		vroot.cpp \
\
	screen.cpp		\
	sdl_screen.cpp 		sdlgl_screen.cpp   \
	gl_screen.cpp		soft_screen.cpp    \
	aa_screen.cpp \
\
	controller.cpp  	kbd_ctrl.cpp	\
	mouse_ctrl.cpp		vimo_ctrl.cpp	\
	joy_ctrl.cpp		midi_ctrl.cpp   \
	trigger_ctrl.cpp 	osc_ctrl.cpp \
	wiimote_ctrl.cpp 	sdl_controller.cpp \
	video_encoder.cpp \
	impl_video_encoders.cpp 

CONSOLE_SOURCES =	console_ctrl.cpp  \
	console_calls_ctrl.cpp 	console_readline_ctrl.cpp \
	console_widgets_ctrl.cpp

if BUILD_AUDIO
SOURCES += audio_layer.cpp \
	audio_jack.cpp \
	audio_collector.cpp \
	theorautils.cpp \
	oggtheora_encoder.cpp
endif

lib_LTLIBRARIES = libfreej.la
libfreej_la_SOURCES = $(SOURCES)
libfreej_la_LIBADD  = $(FREEJ_STATIC_LIBS)
libfreej_la_LDFLAGS = $(FREEJ_LIBS) -Wl,-no-undefined -version-info $(FREEJ_VERSION_INFO)

if BUILD_CONSOLE
bin_PROGRAMS = freej
freej_SOURCES = freej.cpp
freej_SOURCES += $(CONSOLE_SOURCES)
freej_LDADD = libfreej.la $(X11_LIBS) $(FREEJ_CONSOLE_STATIC_LIBS)
endif
