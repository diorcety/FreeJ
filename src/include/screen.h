/*  FreeJ
 *  (c) Copyright 2001 - 2010 Denis Roio <jaromil@dyne.org>
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

/**
   @file screen.h
   @brief FreeJ generic Screen interface
 */

#ifndef __SCREEN_H__
#define __SCREEN_H__

#include <inttypes.h>
#include <config.h>
#include <SDL.h>


#include <closure.h>
#include <sharedptr.h>
#include <linklist.h>
#include <ringbuffer.h>

#include <layer.h>
#include <blitter.h>


template <class T> class LinkList;

///////////////////////
// GLOBAL COLOR MASKING

#ifdef WITH_COCOA
#define red_bitmask   (uint32_t)0x0000ff00
#define rchan         2
#define green_bitmask (uint32_t)0x00ff0000
#define gchan         1
#define blue_bitmask  (uint32_t)0xff000000
#define bchan         0
#define alpha_bitmask (uint32_t)0x000000ff
#define achan         3
#else
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
#define red_bitmask   (uint32_t)0x00ff0000
#define rchan         2
#define green_bitmask (uint32_t)0x0000ff00
#define gchan         1
#define blue_bitmask  (uint32_t)0x000000ff
#define bchan         0
#define alpha_bitmask (uint32_t)0xff000000
#define achan         3
#else
// I got much better performance with same bitmasks on my ppc
// maybe runtime detect these like SDL testvidinfo.c?
#define red_bitmask   (uint32_t)0x00ff0000
#define rchan         1
#define green_bitmask (uint32_t)0x0000ff00
#define gchan         2
#define blue_bitmask  (uint32_t)0x000000ff
#define bchan         3
#define alpha_bitmask (uint32_t)0xff000000
#define achan         0
#endif
#endif

FREEJ_FORWARD_PTR(JackClient)
FREEJ_FORWARD_PTR(Layer)
FREEJ_FORWARD_PTR(Geometry)
FREEJ_FORWARD_PTR(VideoEncoder)

/**
   This class provides a generic interface common to all different
   output screens present in FreeJ: its methods and properties are
   affecting the visualization of mixed results in output.

   @brief Output screen where results are visualized
 */

FREEJ_FORWARD_PTR(ViewPort)
class ViewPort : public Entry {

public:
    ViewPort();
    virtual ~ViewPort();

    bool init(int w = 0, int h = 0, int bpp = 0); ///< general initialization

private:
    bool initialized;
public:
    inline bool isInitialized() {
        return initialized;
    }

    enum fourcc { RGBA32, BGRA32, ARGB32 }; ///< pixel formats understood
    virtual fourcc get_pixel_format() = 0; ///< return the pixel format

    virtual void *get_surface() = 0; ///< returns direct pointer to video memory

    virtual void *coords(int x, int y) = 0;
    ///< returns pointer to pixel (slow! use it once and then move around)

    virtual void blit(LayerPtr src) = 0; ///< operate the blit

    void blit_layers();

    virtual bool add_layer(LayerPtr lay); ///< add a new layer to the screen
    virtual void rem_layer(LayerPtr lay); ///< remove a layer from the screen
#ifdef WITH_AUDIO
    virtual bool add_audio(JackClientPtr jcl); ///< connect layer to audio output
    //virtual void rem_audio(JackClientPtr lay); ///< disconnect layer from the screen
#endif

    LinkList<Layer> &getLayers();

private:

    LinkList<Layer> layers; ///< linked list of registered layers

public:

    bool add_encoder(VideoEncoderPtr enc); ///< add a new encoder for the screen
    //bool rem_encoder(VideoEncoderPtr enc); ///< remove a encoder from the screen
    LinkList<VideoEncoder>& getEncoders();
private:
    LinkList<VideoEncoder> encoders; ///< linked list of registered encoders

public:
#ifdef WITH_GD
    void save_frame(char *file); ///< save a screenshot of the current frame into file
#endif
    void cafudda(double secs); ///< run the engine for seconds or one single frame pass

    void handle_resize();
    virtual void resize(int w, int h) final; //TODO REMOVE final

    virtual void do_resize(int resize_w, int resize_h) = 0;
    virtual void show() = 0;
    virtual void clear() = 0;

    virtual void fullscreen() = 0;
    virtual bool lock() {
        return(true);
    };
    virtual bool unlock() {
        return(true);
    };

    void reset();

    inline BlitterPtr getBlitter() const {
        return blitter;
    }

public:
    inline const Geometry& getGeometry() const {
        return geo;
    }

protected:
    BlitterPtr blitter; ///< Blitter interface for this Layer
    Geometry geo;

    bool resizing;
    int resize_w;
    int resize_h;

#ifdef WITH_AUDIO
private:
    ringbuffer_t *audio; ///< FIFO ringbuffer for audio
    long unsigned int  *m_SampleRate; // pointer to JACKd's SampleRate (add_audio)
    
public:
    inline ringbuffer_t* getAudio() {
        return audio;
    }
    inline const long unsigned int *getSampleRate() {
        return m_SampleRate;
    }
#endif

    // opengl special blit
    bool opengl;

    bool indestructible;
public:
    inline bool isIndestructible() {
        return indestructible;
    }
protected:
    virtual bool _init() = 0; ///< implemented initialization

};

#endif
