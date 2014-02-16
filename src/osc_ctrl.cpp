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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <config.h>

#include <osc_ctrl.h>

#include <context.h>
#include <jutils.h>

FACTORY_REGISTER_INSTANTIATOR(Controller, OscController, OscController, core);

/* convert a big endian 32 bit string to an int for internal use */
//#ifdef ARCH_X86
/*
   static int toInt(const char* b) {
   return
    (( (int) b[3]) & 0xff)        +
    ((((int) b[2]) & 0xff) << 8)  +
    ((((int) b[1]) & 0xff) << 16) +
    ((((int) b[0]) & 0xff) << 24) ;
   }
   //#else

   static int toInt(const char *b)
   {
   return
    (( (int) b[0]) & 0xff)        +
    (( (int) b[1]  & 0xff) << 8)  +
    ((((int) b[2]) & 0xff) << 16) +
    ((((int) b[3]) & 0xff) << 24) ;
   }
   //#endif
 */

OscController::OscController()
    : Controller() {

    srv = NULL;
    sendto = NULL;

    setName("OscCtrl");
}

OscController::~OscController() {

    if(srv)
        lo_server_thread_free(srv);

}

int OscController::poll() {
    // check if there are pending commands
    if(commands_pending.len() > 0)
        return dispatch();
    else
        return 0;
}

int OscController::dispatch() {
    return 0;
}

