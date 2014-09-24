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
#include <algorithm>
#include <slw_log.h>


SLW_Log::SLW_Log()
    : SLangWidget() {

    setName("log");
    textconsole = NULL;
    scrolling = false;

}

SLW_Log::~SLW_Log() {
}

bool SLW_Log::init() {

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
    textconsole->widget = SharedFromThis();
    textconsole->w = w;
    textconsole->h = h;
    textconsole->cur_x = 0;
    textconsole->cur_y = 0;


    refresh();

    return true;
}

bool SLW_Log::feed(int key) {
    bool res = true;
    // interprets a keycode and perform the action (or write a letter)
    if(!key) return(false);

    LockedLinkList<Row> list = textconsole->rows.getLock();
    LockedLinkList<Row>::iterator it = std::find(list.begin(), list.end(), textconsole->vis_row_in);

    switch(key) {
    case KEY_PAGE_UP:
        if(it != list.begin()) {
            textconsole->vis_row_in = *(--it);
            it = std::find(list.begin(), list.end(), textconsole->cur_row);
            textconsole->cur_row = *(--it);

            scrolling = true;
            refresh();
        }
        break;
    case KEY_PAGE_DOWN:
        if(it != list.end() && (++it) != list.end()) {
            textconsole->vis_row_in = *(++it);
            it = std::find(list.begin(), list.end(), textconsole->cur_row);
            textconsole->cur_row = *(++it);

            if((++it) != list.end()) scrolling = false;
            refresh();
        }
        break;
    default:
        res = false;
        break;
        //		func("key [ %c : %u ] fed to log", key, key);
        //		refresh();
    }
    // TODO: scrolling pg-up down
    return(res);
}

bool SLW_Log::refresh() {
    RowPtr r;
    register int c;

    if(!textconsole->vis_row_in) return false;
    else r = textconsole->vis_row_in;

    LockedLinkList<Row> list = textconsole->rows.getLock();
    LockedLinkList<Row>::iterator it = std::find(list.begin(), list.end(), textconsole->vis_row_in);

    // tell the renderer to blank the surface for a refresh
    // this is a pure virtual function here
    blank();

    for(c = 0; c < h; c++ ) {

        if(r->text) {
            color = 1;
            if(r->text[1] == '*') color = 3;
            if(r->text[1] == '!') color = 2;
            if(r->text[1] == 'W') color = 4;
            putnch(r->text, 0, c, r->len);
        }

        if(it == list.end() || (++it) == list.end()) break;
        else r = *it;

    }

    if(textconsole->cur_row != r) {
        putnch((char*)"|",w-1,c-2,1);
        putnch((char*)"V",w-1,c-1,1);
    }
    return true;
}

void SLW_Log::append(const char *text) {
    LockedLinkList<Row> list = textconsole->rows.getLock();
    LockedLinkList<Row>::iterator it = std::find(list.begin(), list.end(), textconsole->vis_row_in);
    RowPtr r = MakeShared<Row>();
    int len = strlen(text);

    r->insert_string((char*)text, len);

    list.push_back( r );

    if(!scrolling) textconsole->cur_row = r;

    if(textconsole->cur_y < textconsole->h)
        textconsole->cur_y++;

    if(!scrolling & (textconsole->cur_y >= textconsole->h) )
        textconsole->vis_row_in = *(++it);
}

void SLW_Log::notice(const char *text, ...) {
    char msg[255];
    va_list arg;
    va_start(arg, text);

    // TODO: colors
    vsnprintf(msg, 254, text, arg);

    append(msg);

    va_end(arg);
}

void SLW_Log::error(const char *text, ...) {
    char msg[255];
    va_list arg;
    va_start(arg, text);

    // TODO: colors
    vsnprintf(msg, 254, text, arg);

    append(msg);

    va_end(arg);
}

void SLW_Log::act(const char *text, ...) {
    char msg[255];
    va_list arg;
    va_start(arg, text);

    // TODO: colors
    vsnprintf(msg, 254, text, arg);

    append(msg);

    va_end(arg);
}

void SLW_Log::warn(const char *text, ...) {
    char msg[255];
    va_list arg;
    va_start(arg, text);

    // TODO: colors
    vsnprintf(msg, 254, text, arg);

    append(msg);

    va_end(arg);
}

void SLW_Log::func(const char *text, ...) {
    char msg[255];
    va_list arg;
    va_start(arg, text);

    // TODO: colors
    vsnprintf(msg, 254, text, arg);

    append(msg);

    va_end(arg);
}

