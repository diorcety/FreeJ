/*  FreeJ
 *  (c) Copyright 2001 - 2010 Denis Roio <jaromil@dyne.org>
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

#include <context.h>
#include <screen.h>
#include <layer.h>
#include <video_encoder.h>
#ifdef WITH_AUDIO
#include <audio_jack.h>
#endif
#include <ringbuffer.h>

#include <video_layer.h>

#ifdef WITH_GD
#include <gd.h>
#endif

#include <algorithm>

LinkList<Blit> &get_linear_blits();

ViewPort::ViewPort()
    : Entry() {

    blitter = MakeShared<Blitter>();

    opengl = false;

    resize_w = 0;
    resize_h = 0;
    resizing = false;

    indestructible = false;

#ifdef WITH_AUDIO
    audio = NULL;
    m_SampleRate = NULL;
    indestructible = false;
    // if compiled with audio initialize the audio data pipe
//   audio = ringbuffer_create(1024 * 512);
    audio = ringbuffer_create(4096 * 512 * 8);
#endif

    LockedLinkList<Blit> blitterBlits = LockedLinkList<Blit>(blitter->getBlits());
    LockedLinkList<Blit> linearBlits = LockedLinkList<Blit>(get_linear_blits());
    blitterBlits.insert(blitterBlits.end(), linearBlits.begin(), linearBlits.end());
    if(linearBlits.size() > 0) {
        blitter->setDefaultBlit(linearBlits.front());
    }
}

ViewPort::~ViewPort() {
#ifdef WITH_AUDIO
    if(audio) ringbuffer_free(audio);
#endif
}

bool ViewPort::init(int w, int h, int bpp) {

    if(bpp != 32) {
        warning("FreeJ is forced to use 32bit pixel formats, hardcoded internally");
        warning("you are initializing a ViewPort with a different bpp value");
        warning("please submit a patch if you can make it :)");
        return false;
    }

    geo.init(w, h, bpp);
    initialized = _init();
    act("screen %s initialized with size %ux%u", name.c_str(), geo.w, geo.h);

    return initialized;

}

bool ViewPort::add_layer(LayerPtr lay) {
    func("%s", __PRETTY_FUNCTION__);

    if(lay->screen.lock()) {
        warning("passing a layer from a screen to another is not (yet) supported");
        return(false);
    }

    LockedLinkList<Layer> list = LockedLinkList<Layer>(layers);

    if(!lay->opened) {
        error("layer %s is not yet opened, can't add it", lay->getName().c_str());
        return(false);
    }

    lay->screen = SharedFromThis(ViewPort);
    lay->current_blit = blitter->default_blit->new_instance();
    
    // center the position
    //lay->geo.x = (screen->w - lay->geo.w)/2;
    //lay->geo.y = (screen->h - lay->geo.h)/2;
    //  screen->blitter->crop( lay, screen );
    list.push_front(lay);
    lay->active = true;
    func("layer %s added to screen %s", lay->getName().c_str(), name.c_str());
    return(true);
}

#ifdef WITH_AUDIO
bool ViewPort::add_audio(JackClientPtr jcl) {
    LockedLinkList<Layer> list = LockedLinkList<Layer>(layers);
    LockedLinkList<Layer>::iterator it = list.begin();
    if(it == list.end()) return false;

    VideoLayerPtr lay = DynamicPointerCast<VideoLayer>(*it);

    jcl->SetRingbufferPtr(audio, (int)lay->audio_samplerate, (int)lay->audio_channels);
    std::cerr << "------ audio_samplerate :" << lay->audio_samplerate \
              << " audio_channels :" << lay->audio_channels << std::endl;
    m_SampleRate = &jcl->m_SampleRate;
    return (true);
}

#endif

void ViewPort::rem_layer(LayerPtr lay) {
    LockedLinkList<Layer> list = LockedLinkList<Layer>(layers);
    LockedLinkList<Layer>::iterator it = std::find(list.begin(), list.end(), lay);
    if(it == list.end()) {
        error("layer %s is not inside this screen", lay->getName().c_str());
        return;
    }

    lay->screen.reset(); // symmetry
    list.erase(it);
    notice("removed layer %s (but still present as an instance)", lay->getName().c_str());
}

LinkList<Layer> &ViewPort::getLayers() {
    return layers;
}

void ViewPort::reset() {
    LockedLinkList<Layer>(layers).clear();
}

bool ViewPort::add_encoder(VideoEncoderPtr enc) {
    func("%s", __PRETTY_FUNCTION__);

    LockedLinkList<VideoEncoder> list = LockedLinkList<VideoEncoder>(encoders);

    func("initializing encoder %s", enc->getName().c_str());
    if(!enc->init(SharedFromThis(ViewPort))) {
        error("%s : failed initialization", __PRETTY_FUNCTION__);
        return(false);
    }
    func("initialization done");

    enc->start();

    list.push_back(enc);

    act("encoder %s added to screen %s", enc->getName().c_str(), name.c_str());
    return true;
}

#ifdef WITH_GD
void ViewPort::save_frame(char *file) {
    FILE *fp;
    gdImagePtr im;
    int *src;
    int x, y;

    im = gdImageCreateTrueColor(geo.w, geo.h);
    src = (int*)coords(0, 0);
    for(y = 0; y < geo.h; y++) {
        for(x = 0; x < geo.w; x++) {
            gdImageSetPixel(im, x, y, src[x] & 0x00FFFFFF);
            //im->tpixels[y][x] = src[x] & 0x00FFFFFF;
        }
        src += geo.w;
    }
    fp = fopen(file, "wb");
    gdImagePng(im, fp);
    fclose(fp);
}

#endif


void ViewPort::cafudda(double secs) {
    LockedLinkList<Layer> list = LockedLinkList<Layer>(layers);
    std::for_each(list.begin(), list.end(), [&](LayerPtr &lay) {
                      lay->cafudda(secs);
                  });
}

void ViewPort::blit_layers() {
    LockedLinkList<Layer> list = LockedLinkList<Layer>(layers);
    LockedLinkList<Layer>::reverse_iterator it = list.rbegin();
    std::for_each(list.rbegin(), list.rend(), [&](LayerPtr &lay) {
                      if(lay->buffer) {
                          if(lay->active & lay->opened) {
                              blit(lay);
                          }
                      }
                  });
    /////////// finish processing layers

}

void ViewPort::handle_resize() {
    lock();
    if(resizing) {
        do_resize(resize_w, resize_h);
    }
    unlock();

    if(resizing) {
        /* crop all layers to new screen size */
        LockedLinkList<Layer> list = LockedLinkList<Layer>(layers);
        std::for_each(list.begin(), list.end(), [&](LayerPtr &lay) {
                          blitter->crop(lay, SharedFromThis(ViewPort));
                      });
       resizing = false;
   }
}

void ViewPort::resize(int w, int h) {
    resize_w = w;
    resize_h = h;
    resizing = true;
}
