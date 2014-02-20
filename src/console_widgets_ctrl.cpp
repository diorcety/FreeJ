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

#include <config.h>

#include <slang.h>

#include <slw.h>

#include <slang_console_ctrl.h>
#include <console_widgets_ctrl.h>

#include <context.h>
#include <layer.h>
#include <blitter.h>
#include <video_encoder.h>

#include <jutils.h>


SlwTitle::SlwTitle()
    : SLangWidget() {
    env = NULL;
}

SlwTitle::~SlwTitle() {
}

bool SlwTitle::init() {

    snprintf(title, 256,
             " %s version %s | set the veejay free! | %s | ", PACKAGE, VERSION, PACKAGE_URL);
    titlelen = strlen(title);
    initialized = true;
    return(true);
}

bool SlwTitle::feed(int key) {
    return(false);
}

bool SlwTitle::refresh() {
    color = TITLE_COLOR + 20;
    putnch(title, 0, 0, titlelen);
    /* this is RASTA SOFTWARE! */
    color = 32;
    putnch((char*)"RAS", titlelen, 0, 3);
    color = 34;
    putnch((char*)"TAS", titlelen + 3, 0, 3);
    color = 33;
    putnch((char*)"OFT", titlelen + 6, 0, 3);
    color = PLAIN_COLOR;

    return(true);
}

////////////////////////////////////////////////



SlwSelector::SlwSelector()
    : SLangWidget() {

    env = NULL;
    tmp = NULL;

}

SlwSelector::~SlwSelector() {
    if(tmp) free(tmp);
}

bool SlwSelector::init() {
    tmp = (char*)calloc(w, sizeof(char));
    initialized = true;
    return true;
}

bool SlwSelector::feed(int key) {
    Entry *le, *fe;

    bool res = false;

    ViewPort *screen = env->mSelectedScreen;
    if(!screen) {
        ::error("no screen currently selected");
        return false;
    }

    if(screen->layers.size() > 0) { // there are layers

        res = true;

        // get the one selected
        le = screen->mSelectedLayer;
        if(!le) {
            screen->mSelectedLayer = screen->layers.begin();
        }

        fe = ((Layer*)le)->mSelectedFilter;
        if(!screen) {
            ::error("no filter currently selected");
            return false;
        }

        // switch over operations and perform
        switch(key) {

        case SL_KEY_UP:

            if(!fe) break;  // no filter

            fe = fe->prev; // take the upper one
            ((Layer*)le)->mSelectedFilter = (FilterInstance*)fe;

            break;

        case SL_KEY_DOWN:

            if(!fe) {
                fe = ((Layer*)le)->filters.begin();
                if(!fe) break;  // no filters
                else ((Layer*)le)->mSelectedFilter = (FilterInstance*)fe;
            } else if(fe->next) {
                fe = fe->next;
                ((Layer*)le)->mSelectedFilter = (FilterInstance*)fe;
            }
            break;

        case SL_KEY_LEFT:

            if(!fe) { // no filter selected, move across layers

                // move to the previous or the other end
                if(!le->prev)
                    le = screen->layers.end();
                else
                    le = le->prev;

                // select only this layer
                screen->mSelectedLayer = ((Layer*)le);

            } else { // a filter is selected: move across filter parameters

                // TODO

            }

            break;

        case SL_KEY_RIGHT:

            if(!fe) { // no filter selected, move across layers

                // move to the next layer or the other end
                if(!le->next)
                    le = screen->layers.begin();
                else le = le->next;

                // select only the current
                screen->mSelectedLayer = ((Layer*)le);

            } else { // move across filter parameters

                // TODO

            }
            break;

        case SL_KEY_PPAGE:
        case KEY_PLUS:
            if(fe) fe->up();
            else le->up();
            break;

        case SL_KEY_NPAGE:
        case KEY_MINUS:
            if(fe) fe->down();
            else le->down();
            break;

        case SL_KEY_DELETE:
        case KEY_CTRL_D:
            if(fe) {
                fe->rem(); // WARN: instances are not freed
//	delete fe; // XXX this crashes
            } else {
                //	le->rem();
                //	((Layer*)le)->close();
                env->rem_layer((Layer*)le);
            }
            break;

        case KEY_SPACE:
            if(fe) ((FilterInstance*)fe)->active =
                    !((FilterInstance*)fe)->active;
            else ((Layer*)le)->active =
                    !((Layer*)le)->active;
            break;

        default:
            res = false;
            break;
        }
    }
    return(res);
}

bool SlwSelector::refresh() {
    int sellayercol = 0, layercol, pos;

    ViewPort *screen = env->mSelectedScreen;
    if(!screen) {
        ::error("no screen currently selected");
        return false;
    }

    /* print info the selected layer */
    blank();

    // also put info from encoders, if active
    // so far supported only one encoder
    VideoEncoder *enc = screen->encoders.begin();
    if(enc) {
        snprintf(tmp, w, "Stream: video %u kb/s : audio %u kb/s : encoded %u kb",
                 enc->video_kbps, enc->audio_kbps, enc->bytes_encoded / 1024);
        putnch(tmp, 1, 0, 0);
    }

    layer = screen->mSelectedLayer;
    if(layer) {
        snprintf(tmp, w, "Layer: %s blit: %s [%.0f] geometry x%i y%i w%u h%u",
                 layer->get_filename(), layer->current_blit->getName().c_str(), layer->current_blit->value,
                 layer->geo.x, layer->geo.y, layer->geo.w, layer->geo.h);
        //    SLsmg_erase_eol();
    } else {
        sprintf(tmp, "No Layer selected");
    }
    ///////////////
    // layer print
    color = LAYERS_COLOR;
    putnch(tmp, 1, 1, 0);


    if(screen->layers.size()) {
        Layer *l = screen->layers.begin();
        //    int color;
        int tmpsize = 0;
        layercol = 0;

        //////////////////
        // layer list
        //    SLsmg_gotorc(4,1);

        /* take layer selected and first */
        if(layer)
            filter = layer->mSelectedFilter;

        while(l) { /* draw the layer's list */
            layercol += tmpsize + 4;
            //      SLsmg_set_color(LAYERS_COLOR);
            //      SLsmg_write_string((char *)" -> ");
            color = LAYERS_COLOR;
            putnch((char*)" ->", layercol, 2, 3);

            if(l == layer && !filter) color += 20;
            if(l->fade | l->active) color += 10;

            //      snprintf(tmp, w, " -> %s",l->getName().c_str());
            tmpsize = strlen(l->getName().c_str());
            putnch((char*)l->getName().c_str(), layercol + 4, 2, tmpsize);
            // save position of selected layer
            if(l == layer) sellayercol = layercol;

            l = (Layer *)l->next;
        }


    }


    if(layer) {
        FilterInstance *f;

        filter = layer->mSelectedFilter;

//     SLsmg_gotorc(3,1);
//     SLsmg_set_color(FILTERS_COLOR);
//     SLsmg_write_string((char *)"Filter: ");
        if(!filter) {
            snprintf(tmp, w, "No Filter selected");
            //      SLsmg_erase_eol();
        } else {
            if(filter->proto)
                snprintf(tmp, w, "Filter: %s :: %s",
                         filter->getName().c_str(), filter->proto->description());

            //      SLsmg_set_color(FILTERS_COLOR+10);

            //      SLsmg_erase_eol();
        }

        f = layer->filters.begin();
        pos = 4;
        while(f) {

//       SLsmg_set_color(PLAIN_COLOR);
//       SLsmg_gotorc(pos,0);
//       SLsmg_erase_eol();

            color = FILTERS_COLOR;
//       SLsmg_gotorc(pos,layercol);
            if(f == filter) color += 20;
            if(f->active) color += 10;
//       SLsmg_set_color (color);
            putnch((char*)f->getName().c_str(), sellayercol + 4, pos, 0);
            pos++;
            f = (FilterInstance*)f->next;
        }

//     SLsmg_set_color(PLAIN_COLOR);
//     for(;pos<5;pos++) {
//       SLsmg_gotorc(pos,0);
//       SLsmg_erase_eol();
//     }

    } else snprintf(tmp, w, "No Filter selected");

    color = FILTERS_COLOR;
    putnch(tmp, 1, 3, 0);
    return true;
}

