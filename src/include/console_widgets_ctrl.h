/*  FreeJ - S-Lang console
 *
 *  (c) Copyright 2004-2009 Denis Roio <jaromil@dyne.org>
 *
 * This source code  is free software; you can  redistribute it and/or
 * modify it under the terms of the GNU Public License as published by
 * the Free Software  Foundation; either version 3 of  the License, or
 * (at your option) any later version.
 *
 * This source code is distributed in the hope that it will be useful,
 * but  WITHOUT ANY  WARRANTY; without  even the  implied  warranty of
 * MERCHANTABILITY or FITNESS FOR  A PARTICULAR PURPOSE.  Please refer
 * to the GNU Public License for more details.
 *
 * You should  have received  a copy of  the GNU Public  License along
 * with this source code; if  not, write to: Free Software Foundation,
 * Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef __CONSOLE_WIDGETS_H__
#define __CONSOLE_WIDGETS_H__

#include <slw.h>


FREEJ_FORWARD_PTR(Context)
FREEJ_FORWARD_PTR(Layer)
FREEJ_FORWARD_PTR(FilterInstance)

FREEJ_FORWARD_PTR(SlwTitle)
class SlwTitle : public SLangWidget {
public:
    SlwTitle(const ContextPtr &env);
    ~SlwTitle();

    bool init();
    bool feed(int key);
    bool refresh();

private:
    ContextPtr env;
    char title[256];
    int titlelen;

};

FREEJ_FORWARD_PTR(SlwSelector)
class SlwSelector : public SLangWidget {
public:

    SlwSelector(const ContextPtr &env);
    ~SlwSelector();

    bool init();
    bool feed(int key);
    bool refresh();

private:
    ContextPtr env;
    LayerPtr layer;
    FilterInstancePtr filter;

    char *tmp;
};


#endif


