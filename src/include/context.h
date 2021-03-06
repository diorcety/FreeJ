/*  FreeJ
 *  (c) Copyright 2001-2007 Denis Rojo aka jaromil <jaromil@dyne.org>
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
 */


/**
   @file context.h FreeJ main engine context environment

   @desc this is the main FreeJ engine, containing the main loop
   cafudda() and referencing the tree of instantiated objects
 */

#ifndef __context_h__
#define __context_h__

#include <inttypes.h>
#include <unistd.h>
#include <stdlib.h>


// this header makes freej dependent from SDL
// it is here because of  SDL_PollEvent done in Context
#include <SDL/SDL.h>

//#include "linklist.h"
#include "layer.h"
// #include <osd.h>
//#include "controller.h"
#include "plugger.h"
#include "screen.h"
#include "shouter.h"

#include "factory.h"
#include "config.h"
#include <string>
#include "timelapse.h"

FREEJ_FORWARD_PTR(Controller)
FREEJ_FORWARD_PTR(JsParser)
FREEJ_FORWARD_PTR(AudioCollector)
FREEJ_FORWARD_PTR(VideoEncoder)
FREEJ_FORWARD_PTR(FreejDaemon)

/* maximum height & width supported by context */
#define MAX_HEIGHT 1024
#define MAX_WIDTH 768

FREEJ_FORWARD_PTR(Context)
class Context : public EnableSharedFromThis<Context> {
private:

    static bool factory_initialized;
    /* doublesize calculation */
    uint64_t **doubletab;
    Uint8 *doublebuf;
    int dcy, cy, cx;
    uint64_t eax;
    /* ---------------------- */

    // parts of the cafudda process
    void handle_resize();
    void handle_controllers();

    pthread_t cafudda_thread;
    bool running;

    // Factories
    //static Factory<Layer> layer_factory; // Layer Factory
    // Default layer types
    /*
       inline void default_layertypes()
       {
       default_layertypes_map.insert(FIdPair("GeometryLayer", "basic"));
       }

       //static Factory<Controller> controller_factory; // Controller Factory
       // Default controller types
       inline void default_controllertypes()
       {
       default_controllertypes_map.insert(FIdPair("KeyboardController", "sdl"));
       }
     */

public:

    Context();
    virtual ~Context();

    bool init(); ///< initialise the engine

    //  void close();
public:
    void cafudda(); ///< run the engine for seconds or one single frame pass
    void cafudda(double secs); ///< run the engine for seconds or one single frame pass
    void start(); ///< start the engine and loop until quit is false
    void stop();
    void begin();
    void end();
    void delay();
    bool isQuitting() const;
    void start_threaded(); ///< start the engine in a thread, looping until quit is false

    bool register_controller(ControllerPtr ctrl);
    bool rem_controller(ControllerPtr ctrl);

    int reset(); ///< clear the engine and deletes all registered objects

private:

    bool quit;

    bool pause;

    bool save_to_file;

    //  Osd osd; ///< On Screen Display

    SDL_Event event;
    bool poll_events;

public:
    bool add_screen(ViewPortPtr scr); ///< add a new screen
    bool rem_screen(ViewPortPtr src); ///< remove a screen

    LinkList<ViewPort>& getScreens();

    LinkList<Controller>& getControllers();

    LinkList<Filter>& getFilters();

    LinkList<Filter>& getGenerators();

    bool isClearAll() const;
    void setClearAll(bool clear);

private:
    LinkList<ViewPort> screens; ///< linked list of registered screens

    LinkList<Controller> controllers; ///< linked list of registered interactive controllers

    LinkList<Filter> filters; ///< linked list of registered filters

    LinkList<Filter> generators; ///< linked list of registered generators

    Plugger plugger; ///< filter plugins host

    bool clear_all;

    FPS fps;
    Timelapse timelapse;

public:
    inline double getFps() {
        return this->fps.get();
    }

    inline void setFps(double fps) {
        this->fps.set(fps);
    }

    inline double getTimeRatio() {
        return this->timelapse.getRatio();
    }

    inline void setTimeRatio(double ratio) {
        this->timelapse.setRatio(ratio);
    }

    inline double getCurrentFps() {
        return this->fps.getCurrent();
    }

//TODO
public:
    char *layers_description; ///< string describing available layer types
    char *screens_description; ///< string describing available screen types

    LayerPtr open(char *file, int w = 0, int h = 0); ///< creates a layer from a filename, detecting its type
};

#endif
