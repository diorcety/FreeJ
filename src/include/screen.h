/*  FreeJ
 *  (c) Copyright 2001 Denis Roio aka jaromil <jaromil@dyne.org>
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
 * "$Id$"
 *
 */

#ifndef __SCREEN_H__
#define __SCREEN_H__

#include <inttypes.h>

#include <layer.h>

class ViewPort {
 public:
  ViewPort() {};
  virtual ~ViewPort() {};
 
  /* i keep all the following functions pure virtual to deny the
     runtime resolving of methods between parent and child, which
     would burden our performance */ 
  virtual bool init(int width, int height) =0;
  virtual void blit(Layer *layer) =0;
  virtual void crop(Layer *layer) =0;
  virtual void show() =0;
  virtual void clear() =0;
  virtual void *get_surface() =0; // returns direct pointer to video memory
  virtual void fullscreen() =0;
  virtual bool lock() =0;
  virtual bool unlock() =0;
  int w, h;
  int bpp;
  int size, pitch;


  // deprecated 
  virtual void *coords(int x, int y) =0;
  
};

#endif