/*  FreeJ - Mouse controller
 *
 *  (c) Copyright 2008 Christoph Rudorff <goil@dyne.org>
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
 * "$Id: mouse_ctrl.cpp 881 2008-02-22 01:06:11Z mrgoil $"
 *
 */

#include "config.h"
#include <string.h>

#include "context.h"
#include "jutils.h"

#include "mouse_ctrl.h"

// our objects are allowed to be created trough the factory engine
FACTORY_REGISTER_INSTANTIATOR(Controller, MouseController, MouseController, sdl);


MouseController::MouseController()
    : SdlController() {
    name = "Mouse";
    indestructible = true;
}

MouseController::~MouseController() {
    active = false; // ungrab ... ;)
}

// activate is removed from controller
// bool active is operated directly
// this solves a problem with swig and virtual inheritance...
// hopefully removing the flush data here won't hurt!

// bool MouseController::activate(bool state) {
//      bool old = active;
//      active = state;
//      if (state == false) {
//              SDL_ShowCursor(1);
//              SDL_WM_GrabInput(SDL_GRAB_OFF);
//      }
//      return old;
// }

int MouseController::poll() {
    poll_sdlevents(SDL_MOUSEEVENTMASK); // calls dispatch() foreach SDL_Event
    return 0;
}

/*
   typedef struct{
   Uint8 type;  SDL_MOUSEMOTION
   Uint8 state; SDL_PRESSED or SDL_RELEASED
   Uint16 x, y;
   Sint16 xrel, yrel;
   } SDL_MouseMotionEvent;

   typedef struct{
   Uint8 type;  SDL_MOUSEBUTTONDOWN or SDL_MOUSEBUTTONUP
   Uint8 button; 1 - ...
   Uint8 state; SDL_PRESSED or SDL_RELEASED
   Uint16 x, y;
   } SDL_MouseButtonEvent;
 */

int MouseController::motion(int state, int x, int y, int xrel, int yrel) {
    return 0;
}

int MouseController::button(int button, int state, int x, int y) {
    return 0;
}

int MouseController::dispatch() {
    if(event.type == SDL_MOUSEMOTION) {
        SDL_MouseMotionEvent mm = event.motion;
        return motion(mm.state, mm.x, mm.y, mm.xrel, mm.yrel);
    } else { // MOUSE_BUTTON
        SDL_MouseButtonEvent mb = event.button;
        return button(mb.button, mb.state, mb.x, mb.y);
    }
}

void MouseController::grab(bool state) {
    if(state) {
        SDL_ShowCursor(0);
        SDL_WM_GrabInput(SDL_GRAB_ON);
    } else {
        SDL_ShowCursor(1);
        SDL_WM_GrabInput(SDL_GRAB_OFF);
    }
}

