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

#include <algorithm>

#include <context.h>
#include <layer.h>
#include <blitter.h>
#include <video_encoder.h>

#include <jutils.h>


SlwTitle::SlwTitle()
    : SLangWidget() {
    env = NULL;
    name = "console title";
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
    name = "layer & filter selector";
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
    bool res = false;

    ViewPortPtr screen = env->mSelectedScreen;
    if(!screen) {
        ::error("no screen currently selected");
        return false;
    }

    LockedLinkList<Layer> layerList = LockedLinkList<Layer>(screen->layers);
    LockedLinkList<Layer>::iterator layerIt = std::find(layerList.begin(), layerList.end(), screen->mSelectedLayer);
    if(!layerList.empty()) { // there are layers

        res = true;

        // get the one selected
        if(!screen->mSelectedLayer) {
            screen->mSelectedLayer = layerList.front();
        }
        LayerPtr le = screen->mSelectedLayer;


        LockedLinkList<FilterInstance> filterList = LockedLinkList<FilterInstance>(le->filters);
        LockedLinkList<FilterInstance>::iterator filterIt = std::find(filterList.begin(), filterList.end(), le->mSelectedFilter);

        // switch over operations and perform
        switch(key) {

        case SL_KEY_UP:
            if(filterIt == filterList.begin()) {
                break;  // no filter
            }
            le->mSelectedFilter = *(--filterIt);
            break;

        case SL_KEY_DOWN:
            if(filterIt == filterList.end()) {
                break;
            }
            ++filterIt;
            if(filterIt == filterList.end()) {
                break;
            }
            le->mSelectedFilter = *filterIt;
            break;

        case SL_KEY_LEFT:

            if(filterIt == filterList.end()) { // no filter selected, move across layers
                if(layerIt == layerList.begin()) {
                    break;  // no filter
                }
                screen->mSelectedLayer = *(--layerIt);
            } else { // a filter is selected: move across filter parameters

                // TODO

            }

            break;

        case SL_KEY_RIGHT:

            if(filterIt == filterList.end()) { // no filter selected, move across layers
                if(layerIt == layerList.end()) {
                    break;
                }
                ++layerIt;
                if(layerIt == layerList.end()) {
                    break;
                }
                screen->mSelectedLayer = *layerIt;

            } else { // move across filter parameters

                // TODO

            }
            break;

        case SL_KEY_PPAGE:
        case KEY_PLUS:
            if(filterIt != filterList.end()) {
                (*filterIt)->up();
            } else {
                (*layerIt)->up();
            }
            break;

        case SL_KEY_NPAGE:
        case KEY_MINUS:
            if(filterIt != filterList.end()) {
                (*filterIt)->down();
            } else {
                (*layerIt)->down();
            }
            break;

        case SL_KEY_DELETE:
        case KEY_CTRL_D:
            if(filterIt != filterList.end()) {
                filterIt = filterList.erase(filterIt);
                if(filterIt != filterList.end()) {
                    le->mSelectedFilter = *filterIt;
                } else {
                    le->mSelectedFilter = NULL;
                }
                //fe->rem(); // WARN: instances are not freed
                //	delete fe; // XXX this crashes
            } else {
                //	le->rem();
                //	((Layer*)le)->close();
                env->rem_layer(DynamicPointerCast<Layer>(le));
            }
            break;

        case KEY_SPACE:
            if(filterIt != filterList.end()) {
                (*filterIt)->active = !(*filterIt)->active;
            } else {
                (*layerIt)->active = !(*layerIt)->active;
            }
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

    ViewPortPtr screen = env->mSelectedScreen;
    if(!screen) {
        ::error("no screen currently selected");
        return false;
    }

    /* print info the selected layer */
    blank();

    // also put info from encoders, if active
    // so far supported only one encoder
    LockedLinkList<VideoEncoder> encoderList = LockedLinkList<VideoEncoder>(screen->encoders);
    LockedLinkList<VideoEncoder>::iterator it = encoderList.begin();
    if(it != encoderList.end()) {
        VideoEncoderPtr enc = *it;
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


    LockedLinkList<Layer> layerList = LockedLinkList<Layer>(screen->layers);
    if(!layerList.empty()) {
        //    int color;
        int tmpsize = 0;
        layercol = 0;

        //////////////////
        // layer list
        //    SLsmg_gotorc(4,1);

        /* take layer selected and first */
        if(layer) {
            filter = layer->mSelectedFilter;
            std::for_each(layerList.begin(), layerList.end(), [&](LayerPtr l) {
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
                          });
        }
    }


    if(layer) {
        filter = layer->mSelectedFilter;

//     SLsmg_gotorc(3,1);
//     SLsmg_set_color(FILTERS_COLOR);
//     SLsmg_write_string((char *)"Filter: ");
        if(!filter) {
            snprintf(tmp, w, "No Filter selected");
            //      SLsmg_erase_eol();
        } else {
            snprintf(tmp, w, "Filter: %s :: %s", filter->getName().c_str(), filter->getDescription().c_str());

            //      SLsmg_set_color(FILTERS_COLOR+10);

            //      SLsmg_erase_eol();
        }


        LockedLinkList<FilterInstance> filterList = LockedLinkList<FilterInstance>(layer->filters);
        pos = 4;
        std::for_each(filterList.begin(), filterList.end(), [&] (FilterInstancePtr f) {
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
                      });

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

