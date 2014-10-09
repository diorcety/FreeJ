/*  FreeJ - Freeframe wrapper
 *  (c) Copyright 2008 - 2009 Denis Roio <jaromil@dyne.org>
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

#ifndef __FREEFRAME_FREEJ_H__
#define __FREEFRAME_FREEJ_H__

#include <linklist.h>
#include <freeframe.h>
#include <filter.h>
#include <factory.h>

class Filter;

FREEJ_FORWARD_PTR(Freeframe)
class Freeframe : public Filter {
    friend class FreeframeInstance;
    friend class GeneratorLayer;
#ifdef WITH_COCOA
    friend class CVF0rLayer;
#endif
public:

    Freeframe();
    virtual ~Freeframe();

    virtual int type();

    int open(char *file);

    virtual FilterInstancePtr new_instance();
    inline const PlugInfoStruct *getInfo() const {
        return info;
    }
    
protected:
    void print_info();
    PlugInfoStruct *info;
    VideoInfoStruct vidinfo;
    bool opened;

    // Interface function pointers.
    plugMainType *plugmain;
    
private:
    // dlopen handle
    void *handle;
    // full .so file path
    char filename[512];
    FACTORY_ALLOWED
};


#endif
