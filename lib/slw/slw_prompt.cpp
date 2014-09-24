/*  S-Lang console widgets
 *
 *  (C) Copyright 2004-2008 Denis Rojo <jaromil@dyne.org>
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
 */

#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <slw_prompt.h>


SLW_Prompt::SLW_Prompt()
    : SLangWidget() {

    setName("prompt");
    textconsole = NULL;

}

SLW_Prompt::~SLW_Prompt() {
}

bool SLW_Prompt::init() {

    if(!console.lock()) {
        fprintf(stderr, "can't initialize widget '%s': not placed on console", name.c_str());
        return false;
    }

    // default widget settings
    initialized = true;
    visible = true;
    cursor = false;
    can_focus = true;


    // create the private structure where to hold text
    textconsole = MakeShared<SLW_TextConsole>();
    textconsole->widget = SharedFromThis(SLW_Prompt);
    textconsole->w = w;
    textconsole->h = h;
    textconsole->cur_x = 0;
    textconsole->cur_y = 0;


    refresh();

    return true;
}

bool SLW_Prompt::feed(int key) {
    switch(key) {
    case 0: return false;
    default:
        textconsole->cur_row->insert_char(key);
        textconsole->cur_x++;
    }
    refresh();
    return true;
}

bool SLW_Prompt::refresh() {
    RowPtr r = textconsole->cur_row;

    blank_row(1);

    putnch(r->text, 0, 0, r->len);

    return true;
}

