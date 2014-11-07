/*  FreeJ
 *  (c) Copyright 2001-2002 Denis Rojo aka jaromil <jaromil@dyne.org>
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

#include <string.h>

#include "layer.h"
#include "blitter.h"
#include "blit.h"
#include "blit_instance.h"
#include "filter.h"
#include "iterator.h"
#include "closure.h"

#include "context.h"
#include "config.h"

#include <algorithm>
#include <assert.h>

#include "jutils.h"

//#include "fps.h"

Layer::Layer()
    : Entry() {
    func("%s this=%p", __PRETTY_FUNCTION__, this);
    active = false;
    hidden = false;
    fade = false;
    use_audio = false;
    need_crop = true;
#ifdef WITH_AUDIO
    audio = NULL;
#endif
    opened = false;
    bgcolor = 0;
    name = "???";
    filename[0] = 0;
    buffer = NULL;
    is_native_sdl_surface = false;

    zoom_x = 1.0;
    zoom_y = 1.0;
    rotate = 0.0;
    zooming = false;
    rotating = false;

    antialias = false;
    
    current_blit = NULL;

    null_feeds = 0;
    max_null_feeds = 10;

    priv_data = NULL;
}

Layer::~Layer() {
    func("%s this=%p", __PRETTY_FUNCTION__, this);

    active = false;
}

void *Layer::get_data() {
    return priv_data;
}

void Layer::set_data(void *data) {
    priv_data = data;
}

bool Layer::init(int wdt, int hgt, int bpp) {

    geo.init(wdt, hgt, bpp);

    func("initialized %s layer %ix%i", getName().c_str(), geo.w, geo.h);

    if(!geo.getByteSize()) {
        // if  the   size  is  still  unknown   at  init  then   it  is  the
        // responsability for the layer implementation to create the buffer
        // (see for instance text_layer)
        act("initialized %s layer with dynamic size", getName().c_str());

    }

    // call the specific layer's implementation _init()
    return _init();
}

char *Layer::get_blit() {

    if(!current_blit) {
        error("no blit currently selected for layer %s", name.c_str());
        return((char*)"unknown");
    }
    return (char*)current_blit->getName().c_str(); // TODO remove that cast
}

bool Layer::set_blit(const char *bname) {
    auto screen = this->screen.lock();
    if(screen) {
        auto blitter = screen->getBlitter();
        LinkList<Blit> &list = blitter->getBlits();
        LinkList<Blit>::iterator it = std::find_if(list.begin(), list.end(), [&] (BlitPtr &b) {
                                                             return b->getName() == bname;
                                                         });

        if(it == list.end()) {
            error("blit %s not found in screen %s", bname, screen->getName().c_str());
            return(false);
        }
        BlitPtr b = *it;

        func("blit for layer %s set to %s", name.c_str(), b->getName().c_str());

        current_blit = b->new_instance(); // start using
        need_crop = true;
        blitter->crop(SharedFromThis(Layer), screen);
        act("blit %s set for layer %s", current_blit->getName().c_str(), name.c_str());
    } else {
        warning("can't set blit for layer %s: not added on any screen yet", name.c_str());
        return(false);
    }

    return(true);
}

void Layer::blit() {
    if(!buffer) {
        // check threshold of tolerated null feeds
        // deactivate the layer when too many
        func("feed returns NULL on layer %s", getName().c_str());
        null_feeds++;
        if(null_feeds > max_null_feeds) {
            warning("layer %s has no video, deactivating", getName().c_str());
            active = false;
            return;
        }

    } else {

        null_feeds = 0;

        if(auto screen = this->screen.lock()) {
            screen->blit(SharedFromThis(Layer));
        }
    }
}

bool Layer::cafudda(double time) {
    if(!opened) return false;

    if(!fade)
        if(!active || hidden)
            return false;

    do_iterators();


    void *tmp_buf;

    // process all registered operations
    // and signal to the synchronous waiting feed()
    // includes parameter changes for layer
    tmp_buf = feed(time);

    // check if feed returned a NULL buffer
    if(tmp_buf) {
        // process filters on the feed buffer
        tmp_buf = do_filters(time, tmp_buf);
    }

    // we add  a memcpy at the end  of the layer pipeline  this is not
    // that  expensive as leaving  the lock  around the  feed(), which
    // slows down the whole engine in case the layer is slow. -jrml
    buffer = tmp_buf;

    return(true);
}

void *Layer::do_filters(double time, void *tmp_buf) {
    std::for_each(filters.begin(), filters.end(), [&](FilterInstancePtr filt) {
                      tmp_buf = (void*) filt->process(time, (uint32_t*)tmp_buf);
                  });
    return tmp_buf;
}

int Layer::do_iterators() {

    /* process thru iterators */
    LinkList<Iterator> &list = iterators;
    LinkList<Iterator>::iterator it = list.begin();
    while(it != list.end()) {
        IteratorPtr iter = *it;
        int res = iter->cafudda(); // if cafudda returns -1...
        if(res < 0) {
            it = list.erase(it);
            if(it == list.end()) {
                if(fade) { // no more iterations, fade out deactivates layer
                    fade = false;
                    active = false;
                }
            }
        } else {
            ++it;
        }
    }
    return(1);
}

void Layer::set_filename(const char *f) {
    const char *p = f + strlen(f);
    while(*p != '/' && (p >= f))
        p--;
    strncpy(filename, p + 1, 256);
}

void Layer::set_position(int x, int y) {
    geo.x = x;
    geo.y = y;
    need_crop = true;
}

int Layer::get_x_position() const {
    return geo.x;
}

void Layer::set_x_position(int x) {
    set_position(x, geo.y);
}

int Layer::get_y_position() const {
    return geo.y;
}

void Layer::set_y_position(int y) {
    set_position(geo.x, y);
}

void Layer::set_zoom(double x, double y) {
    if((x == 1) && (y == 1)) {
        zooming = false;
        zoom_x = zoom_y = 1.0;
        act("%s layer %s zoom deactivated", name.c_str(), filename);
    } else {
        zoom_x = x;
        zoom_y = y;
        zooming = true;
        func("%s layer %s zoom set to x%.2f y%.2f", name.c_str(), filename, zoom_x, zoom_y);
    }
    need_crop = true;
}

void Layer::set_rotate(double angle) {
    if(!angle) {
        rotating = false;
        rotate = 0;
        act("%s layer %s rotation deactivated", name.c_str(), filename);
    } else {
        rotate = angle;
        rotating = true;
        func("%s layer %s rotation set to %.2f", name.c_str(), filename, rotate);
    }
    need_crop = true;
}

void Layer::fit(bool maintain_aspect_ratio) {
    double width_zoom, height_zoom;
    int new_x = 0;
    int new_y = 0;
    auto screen = this->screen.lock();

    if(!screen) {
        error("Cannot fit layer without a screen, add layer to a screen first");
        return;
    }

    const Geometry &screen_geo = screen->getGeometry();
    width_zoom = (double)screen_geo.w / geo.w;
    height_zoom = (double)screen_geo.h / geo.h;
    if(maintain_aspect_ratio) {
        //to maintain the aspect ratio we simply zoom to the smaller of the
        //two zoom values
        if(width_zoom > height_zoom) {
            width_zoom = height_zoom;
        } else {
            height_zoom = width_zoom;
        }
    }
    set_zoom(width_zoom, height_zoom);
    // reposition layer upper corner
    new_x = ((double)(width_zoom * geo.w - geo.w) / 2.0);
    new_y = ((double)(height_zoom * geo.h - geo.h) / 2.0);

    // center layer
    new_x += ((double)(screen_geo.w - width_zoom * geo.w) / 2.0);
    new_y += ((double)(screen_geo.h - height_zoom * geo.h) / 2.0);
    set_position(new_x, new_y);
}

bool Layer::_init() {
    // Don't do anything in base implementation
    return true;
}

bool Layer::open(const char *file) {
    // do nothing , our subclass shoud override this method if they implement an open
    func("base Layer::open(%s) called passing", file);
    return false;
}

void Layer::close() {
    // do nothing , our subclass shoud override this method if they implement an open
    func("base Layer::close() called passing");
    return;
}

bool Layer::isActive() const {
    return active;
}

BlitInstancePtr Layer::getCurrentBlit() const {
    return current_blit;
}

LinkList<ParameterInstance>& Layer::getParameters() {
    return parameters;
}

LinkList<FilterInstance>& Layer::getFilters() {
    return filters;
}

Layer::Type Layer::getType() const {
    return type;
}

bool Layer::up() {
    auto screen = this->screen.lock();

    if(!screen) {
        error("Cannot up layer without a screen, add layer to a screen first");
        return false;
    }

    LinkList<Layer> &layerList = screen->getLayers();
    LinkList<Layer>::iterator layerIt = std::find(layerList.begin(), layerList.end(), SharedFromThis(Layer));
    assert(layerIt != layerList.end());
    if(layerIt == layerList.begin()) {
        error("Cannot up layer, it is already the first layer");
        return false;
    }
    LinkList<Layer>::iterator newLayerIt = layerIt--;
    layerList.splice(newLayerIt, layerList, layerIt);
    return true;
}

bool Layer::down() {
    auto screen = this->screen.lock();

    if(!screen) {
        error("Cannot down layer without a screen, add layer to a screen first");
        return false;
    }

    LinkList<Layer> &layerList = screen->getLayers();
    LinkList<Layer>::iterator layerIt = std::find(layerList.begin(), layerList.end(), SharedFromThis(Layer));
    assert(layerIt != layerList.end());
    LinkList<Layer>::iterator newLayerIt = layerIt++;
    if(newLayerIt == layerList.end()) {
        error("Cannot down layer, it is already the last layer");
        return false;
    }
    layerList.splice(newLayerIt, layerList, layerIt);
    return true;
}

bool Layer::move(int pos) {
    auto screen = this->screen.lock();

    if(!screen) {
        error("Cannot move layer without a screen, add layer to a screen first");
        return false;
    }

    LinkList<Layer> &layerList = screen->getLayers();
    if(pos < 0 || (size_t)pos >= layerList.size()) {
        error("Cannot move layer to an invalid position");
    }

    LinkList<Layer>::iterator layerIt = std::find(layerList.begin(), layerList.end(), SharedFromThis(Layer));
    assert(layerIt != layerList.end());
    LinkList<Layer>::iterator newLayerIt = layerList.begin();
    std::advance(newLayerIt, pos);

    layerList.splice(newLayerIt, layerList, layerIt);

    return true;
}
