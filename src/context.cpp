/*  FreeJ
 *  (c) Copyright 2001 - 2007 Denis Roio <jaromil@dyne.org>
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
 */

#include "config.h"
#include <inttypes.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <assert.h>
#include "context.h"
#include "blitter.h"
#include "controller.h"

#include "video_encoder.h"
#include "audio_collector.h"
#include "fps.h"

#include <signal.h>
#include <errno.h>

#include <algorithm>

#include "fastmemcpy.h"

#include "jutils.h"

#ifdef WITH_FFMPEG
#include "video_layer.h"
#endif

#include "impl_layers.h"
#include "impl_video_encoders.h"
#include "factory.h"

void fsigpipe(int Sig);
int got_sigpipe;

bool Context::factory_initialized = false;

static void init_factory() {
    Factory<Controller>::set_default_classtype("KeyboardController", "sdl"); // singleton
    Factory<Controller>::set_default_classtype("MouseController", "sdl"); // singleton
    Factory<Controller>::set_default_classtype("TriggerController", "core"); // singleton
    Factory<Controller>::set_default_classtype("OscController", "core");

    Factory<ViewPort>::set_default_classtype("Screen", "sdl"); // singleton

    Factory<Layer>::set_default_classtype("GeometryLayer", "basic");
    Factory<Layer>::set_default_classtype("MovieLayer", "ffmpeg");
    Factory<Layer>::set_default_classtype("GeneratorLayer", "ff_f0r");
    Factory<Layer>::set_default_classtype("ImageLayer", "sdl");
#ifdef WITH_UNICAP
    Factory<Layer>::set_default_classtype("CamLayer", "unicap");
#endif
#ifdef WITH_OPENCV
    Factory<Layer>::set_default_classtype("CamLayer", "opencv");
#endif
#ifdef WITH_V4L
    Factory<Layer>::set_default_classtype("CamLayer", "v4l2");
#endif

#ifdef WITH_CAIRO
    Factory<Layer>::set_default_classtype("VectorLayer", "cairo");
#endif
#ifdef WITH_TEXTLAYER
    Factory<Layer>::set_default_classtype("TextLayer", "truetype");
#endif
#ifdef WITH_FREI0R
    Factory<Filter>::set_default_classtype("Frei0rFilter", "core");
#endif
#ifdef WITH_FREEFRAME
    Factory<Filter>::set_default_classtype("FreeframeFilter", "core");
#endif
    Factory<FilterInstance>::set_default_classtype("FilterInstance", "core");
}

void * run_context(void * data) {
    ContextPtr *contextPtr = (ContextPtr *)data;
    ContextPtr context = *contextPtr;
    context->start();
    delete contextPtr;
    pthread_exit(NULL);
}

Context::Context() {
    func("%s this=%p", __PRETTY_FUNCTION__, this);

    //audio           = NULL;

    /* initialize fps counter */
    //  framecount      = 0;
    clear_all       = false;
    quit            = false;
    pause           = false;
    save_to_file    = false;
    poll_events     = true;

    layers_description = (char*)
                         " .  - ImageLayer for image files (png, jpeg etc.)\n"
                         " .  - GeometryLayer for scripted vectorial primitives\n"
#ifdef WITH_V4L
                         " .  - Video4Linux devices as of BTTV cards and webcams\n"
                         " .    you can specify the size  /dev/video0%160x120\n"
#endif
#ifdef WITH_FFMPEG
                         " .  - MovieLayer for movie files, urls and firewire devices\n"
#endif
#if defined WITH_TEXTLAYER
                         " .  - TextLayer for text rendered with freetype2 library\n"
#endif
#ifdef WITH_FLASH
                         " .  - FlashLayer for SWF flash v.3 animations\n"
#endif
#ifdef WITH_OPENCV
                         " .  - OpenCV for camera capture\n"
#endif
                         "\n";

    screens_description = (char*)
                          " .  - [ sdl   ] screen - Simple Directmedia Layer\n"
                          " .  - [ soft  ] screen - internal shared buffer\n"
#ifdef WITH_OPENGL
                          " .  - [ sdlgl ] screen - SDL 3d opengl\n"
                          " .  - [ gl    ] screen - 3d opengl surface\n"
#endif
#ifdef WITH_AALIB
                          " .  - [ aa    ] screen - Ascii (aalib) text output\n"
#endif
                          "\n";

    if(!factory_initialized)
        init_factory();

    assert(init());

}

Context::~Context() {
    func("%s this=%p", __PRETTY_FUNCTION__, this);

    reset();


    //   invokes JSGC and all gc call on our JSObjects
    //  if(js) js->reset();

    notice("cu on %s", PACKAGE_URL);
}

//
// Factory-related methods
//


//
// End of Factory-related methods
//

bool Context::add_screen(ViewPortPtr scr) {
    if(!scr->isInitialized()) {
        error("can't add screen %s - not initialized yet", scr->getName().c_str());
        error("use init( width, height, bits_per_pixel )");
        return false;
    }
    screens.push_front(scr);
    func("screen %s successfully added", scr->getName().c_str());
    act("screen %s now on top", scr->getName().c_str());

    return(true);
}

bool Context::rem_screen(ViewPortPtr scr) {
    LinkList<ViewPort>::iterator screenIt = std::find(screens.begin(), screens.end(), scr);
    if(screenIt == screens.end()) {
        return false;
    }

    screens.erase(screenIt);
    func("screen %s successfully removed", scr->getName().c_str());

    return(true);
}

LinkList<ViewPort>& Context::getScreens() {
    return screens;
}

LinkList<Controller>& Context::getControllers() {
    return controllers;
}

LinkList<Filter>& Context::getFilters() {
    return filters;
}

LinkList<Filter>& Context::getGenerators() {
    return generators;
}

bool Context::init() {

    notice("Initializing the FreeJ engine");

    // a fast benchmark to select the best memcpy to use
    find_best_memcpy();

#ifdef WITH_FFMPEG
    /** init ffmpeg libraries: register all codecs, demux and protocols */
    av_register_all();
    /** make ffmpeg silent */
    av_log_set_level(AV_LOG_QUIET);
    //av_log_set_level(AV_LOG_DEBUG);
    act("FFmpeg initialized all codec and format");
#endif


    // register SIGPIPE signal handler (stream error)
    got_sigpipe = false;
    if(signal(SIGPIPE, fsigpipe) == SIG_ERR) {
        error("Couldn't install SIGPIPE handler");
        //   exit (0); lets not be so drastical...
    }

    // refresh the list of available plugins
    LinkList<Filter> &&new_filters = plugger.getFilters();
    filters.insert(filters.end(), new_filters.begin(), new_filters.end());

    LinkList<Filter> &&new_generators = plugger.getGenerators();
    generators.insert(generators.end(), new_generators.begin(), new_generators.end());

    return true;
}

void Context::start() {
    quit = false;
    running = true;
    while(!quit) {
        cafudda(1.0);
    }
    running = false;
}

void Context::stop() {
    quit = true;
}

void Context::start_threaded() {
    if(!running)
        pthread_create(&cafudda_thread, 0, run_context, new SharedPtr<Context>(SharedFromThis(Context)));
}

/*
 * Main loop called fps_speed times a second
 */
void Context::cafudda(double secs) {
    timelapse.setRatio(secs);
    timelapse.start();

    ///////////////////////////////
    //// process controllers
    if(poll_events)
        handle_controllers();

    ///////////////////////////////

    /////////////////////////////
    // blit layers on screens
    std::for_each(screens.begin(), screens.end(), [&](ViewPortPtr scr) {
                      if(clear_all) scr->clear();

                      // Change resolution if needed
                      scr->handle_resize();

                      scr->cafudda(timelapse.getTime());

                      scr->blit_layers();

                      // show the new painted screen
                      scr->show();
                  });
}

#define SDL_KEYEVENTMASK (SDL_KEYDOWNMASK|SDL_KEYUPMASK)

void Context::handle_controllers() {
    int res;

    event.type = SDL_NOEVENT;

    SDL_PumpEvents();

    // peep if there are quit or fullscreen events
    res = SDL_PeepEvents(&event, 1, SDL_PEEKEVENT, SDL_KEYEVENTMASK | SDL_QUITMASK);
    if(res < 0) warning("SDL_PeepEvents error");

    // force quit when SDL does
    if(event.type == SDL_QUIT) {
        quit = true;
        return;
    }

    // fullscreen switch (ctrl-f)
    if(event.type == SDL_KEYDOWN)
        if(event.key.state == SDL_PRESSED)
            if(event.key.keysym.mod & KMOD_CTRL)
                if(event.key.keysym.sym == SDLK_f) {
                    // WTF?
                    //ViewPortPtr scr = mSelectedScreen;
                    //scr->fullscreen();
                    //res = SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_KEYEVENTMASK | SDL_QUITMASK);
                    //if(res < 0) warning("SDL_PeepEvents error");
                }

    std::for_each(controllers.begin(), controllers.end(), [] (ControllerPtr ctrl) {
                      if(ctrl->active)
                          ctrl->poll();
                  });

    // flushes all events that are leftover
    while(SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_ALLEVENTS) > 0)
        continue;
    memset(&event, 0x0, sizeof(SDL_Event));

}

bool Context::register_controller(ControllerPtr ctrl) {
    func("%u:%s:%s", __LINE__, __FILE__, __FUNCTION__);

    if(!ctrl) {
        error("%s called on a NULL object", __PRETTY_FUNCTION__);
        return false;
    }

    if(!ctrl->initialized) {
        func("initialising controller %s (%p)", ctrl->getName().c_str(), ctrl.get());

        ctrl->init(SharedFromThis(Context));

    } else
        warning("controller was already initialised on this context");

    func("controller %s initialized", ctrl->getName().c_str());

    ctrl->active = true;

    controllers.push_back(ctrl);

    act("registered %s controller", ctrl->getName().c_str());
    return true;
}

bool Context::rem_controller(ControllerPtr ctrl) {
    func("%s", __PRETTY_FUNCTION__);
    if(!ctrl) {
        error("%s called on a NULL object", __PRETTY_FUNCTION__);
        return false;
    }

    //  if(js) js->gc(); // ?!

    ctrl->active = false;
    controllers.remove(ctrl);
    act("removed controller %s", ctrl->getName().c_str());

    return true;
}

int Context::reset() {
    func("%s", __PRETTY_FUNCTION__);

    notice("FreeJ engine reset");


    {
        LinkList<Controller> &list = controllers;
        LinkList<Controller>::iterator it = list.begin();
        func("deleting %u controllers", list.size());
        while(it != list.end()) {
            ControllerPtr ctrl = *it;
            if(ctrl->indestructible) {
                ctrl->reset();
                ++it;
            } else {
                it = list.erase(it);
            }
        }
    }

    {
        LinkList<ViewPort> &list = screens;
        LinkList<ViewPort>::iterator it = list.begin();
        func("deleting %u screens", list.size());
        while(it != list.end()) {
            ViewPortPtr scr = *it;
            if(scr->isIndestructible()) {
                scr->reset();
                ++it;
            } else {
                it = list.erase(it);
            }
        }
    }
    return 1;
}

void fsigpipe(int Sig) {
    if(!got_sigpipe)
        warning("SIGPIPE - Problems streaming video :-(");
    got_sigpipe = true;
}

//////// implemented layers


// TODO - use factory to create instances for each class of layers
//        (cam-input, videofile-input, whatever)
LayerPtr Context::open(char *file, int w, int h) {
    func("%s", __PRETTY_FUNCTION__);
    char *end_file_ptr, *file_ptr;
    FILE *tmp;
    LayerPtr nlayer;

    /* check that file exists */
    if(strncasecmp(file, "/dev/", 5) != 0
       && strncasecmp(file, "http://", 7) != 0
       && strncasecmp(file, "layer_", 6) != 0) {
        tmp = fopen(file, "r");
        if(!tmp) {
            error("can't open %s to create a Layer: %s",
                  file, strerror(errno));
            return NULL;
        } else fclose(tmp);
    }
    /* check file type, add here new layer types */
    end_file_ptr = file_ptr = file;
    end_file_ptr += strlen(file);
//  while(*end_file_ptr!='\0' && *end_file_ptr!='\n') end_file_ptr++; *end_file_ptr='\0';

    /* Usefull?  TODO REMOVE
    if(!w || !h) {
        // uses the size of currently selected screen
        ViewPortPtr screen = mSelectedScreen;
        w = screen->geo.w;
        h = screen->geo.h;
    }
    */

    /* ==== Unified caputure API (V4L & V4L2) */
    if(strncasecmp(file_ptr, "/dev/video", 10) == 0) {
        unsigned int uw, uh;
        while(end_file_ptr != file_ptr) {
            if(*end_file_ptr != '%') {

                // uses the size of currently selected screen
                uw = w;
                uh = h;
                end_file_ptr--;

            } else { /* size is specified */

                *end_file_ptr = '\0';
                end_file_ptr++;
                sscanf(end_file_ptr, "%ux%u", &uw, &uh);
                end_file_ptr = file_ptr;

            }
        }
        nlayer = Factory<Layer>::new_instance("CamLayer");
        if(!nlayer) {
            error("cannot open camera on %s", file_ptr);
            error("no implementation found for CamLayer in this FreeJ binary");
            return NULL;
        }
        if(!nlayer->init(uw, uh, 32)) {
            error("failed initialization of layer %s for %s", nlayer->getName().c_str(), file_ptr);
            return NULL;
        }
        if(nlayer->open(file_ptr)) {
            notice("video camera source opened");
            //  ((V4lGrabber*)nlayer)->init_width = w;
            //  ((V4lGrabber*)nlayer)->init_heigth = h;
        } else {
            error("create_layer : V4L open failed");
            nlayer = NULL;
        }

    } else /* VIDEO LAYER */

    if(((IS_VIDEO_EXTENSION(end_file_ptr)) | (IS_FIREWIRE_DEVICE(file_ptr)))) {
        func("is a movie layer");

#ifdef WITH_FFMPEG
        nlayer = Factory<Layer>::new_instance("MovieLayer");
        if(!nlayer->init()) {
            error("failed initialization of layer %s for %s", nlayer->getName().c_str(), file_ptr);
            return NULL;
        }
        if(!nlayer->open(file_ptr)) {
            error("create_layer : VIDEO open failed");
            nlayer = NULL;
        }
#else
        error("VIDEO and AVI layer support not compiled");
        act("can't load %s", file_ptr);
#endif
    } else     /* IMAGE LAYER */
    if((IS_IMAGE_EXTENSION(end_file_ptr))) {
//		strncasecmp((end_file_ptr-4),".png",4)==0)
        nlayer = MakeShared<ImageLayer>();
        if(!nlayer->init()) {
            error("failed initialization of layer %s for %s", nlayer->getName().c_str(), file_ptr);
            return NULL;
        }
        if(!nlayer->open(file_ptr)) {
            error("create_layer : IMG open failed");
            nlayer = NULL;
        }
    } else         /* TXT LAYER */
    if(strncasecmp((end_file_ptr - 4), ".txt", 4) == 0) {
#if defined WITH_TEXTLAYER
        nlayer = MakeShared<TextLayer>();

        if(!nlayer->init()) {
            error("failed initialization of layer %s for %s", nlayer->getName().c_str(), file_ptr);
            return NULL;
        }

        if(!nlayer->open(file_ptr)) {
            error("create_layer : TXT open failed");
            nlayer = NULL;
        }
#else
        error("TXT layer support not compiled");
        act("can't load %s", file_ptr);
        return(NULL);
#endif

    } else             /* XSCREENSAVER LAYER */
    if(strstr(file_ptr, "xscreensaver")) {
#ifdef WITH_XSCREENSAVER
        nlayer = MakeShared<XScreenSaverLayer>();

        if(!nlayer->init(w, h, 32)) {
            error("failed initialization of layer %s for %s", nlayer->getName().c_str(), file_ptr);
            return NULL;
        }

        if(!nlayer->open(file_ptr)) {
            error("create_layer : XScreenSaver open failed");
            nlayer = NULL;
        }
#else
        error("no xscreensaver layer support");
        act("can't load %s", file_ptr);
        return(NULL);
#endif
    }  else if(strncasecmp(file_ptr, "layer_goom", 10) == 0) {

#ifdef WITH_GOOM
        nlayer = MakeShared<GoomLayer>();

        if(!nlayer->init(this)) {
            error("failed initialization of layer %s for %s", nlayer->getName().c_str(), file_ptr);
            return NULL;
        }
#else
        error("goom layer not supported");
        return(NULL);
#endif


    }  else if(strncasecmp(end_file_ptr - 4, ".swf", 4) == 0) {

#ifdef WITH_FLASH
        nlayer = MakeShared<FlashLayer>();
        if(!nlayer->init()) {
            error("failed initialization of layer %s for %s", nlayer->getName().c_str(), file_ptr);
            return NULL;
        }

        if(!nlayer->open(file_ptr)) {
            error("create_layer : SWF open failed");
            nlayer = NULL;
        }
#else
        error("flash layer not supported");
        return(NULL);
#endif

    } else if(strcasecmp(file_ptr, "layer_opencv_cam") == 0) {
#ifdef WITH_OPENCV
        func("creating a cam layer using OpenCV");
        nlayer = MakeShared<OpenCVCamLayer>();
        if(!nlayer->init()) {
            error("failed initialization of webcam with OpenCV");
            return NULL;
        }
#else
        error("opencv layer not supported");
        return(NULL);
#endif
    }

    if(!nlayer)
        error("can't create a layer with %s", file);
    else
        func("create_layer successful, returns %p", nlayer.get());
    return nlayer;
}

bool Context::isClearAll() const {
    return clear_all;
}
void Context::setClearAll(bool clear) {
    clear_all = clear;
}
