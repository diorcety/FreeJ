
#include "config.h"
#include <stdlib.h>


#include "jutils.h"
#include "blitter.h"
#include "blit.h"
#include "parameter_instance.h"
#include "linear_blits.h"

#include <SDL_imageFilter.h>

AbstractLinearBlitInstance::AbstractLinearBlitInstance(LinearBlitPtr proto) : proto(proto) {
    
}

void AbstractLinearBlitInstance::operator()(LayerPtr layer) {
    ViewPortPtr scr = getScreen();
    //if(layer->need_crop)
    crop(layer, scr);

    uint32_t *pscr, *play;  // generic blit buffer pointers

    pscr = (uint32_t*) getSurface() + scr_offset;
    play = (uint32_t*) layer->buffer   + lay_offset;

    // iterates the blit on each horizontal line
    for(int16_t c = lay_height; c > 0; c--) {

        proto->fct
            ((void*)play, (void*)pscr,
            lay_bytepitch, // * src->geo.bpp>>3,
           getParameters());

        // strides down to the next line
        pscr += scr_stride + lay_pitch;
        play += lay_stride + lay_pitch;

    }
}

void AbstractLinearBlitInstance::crop(LayerPtr lay, ViewPortPtr scr) {
    /*
    func("crop on layer %s x%i y%i w%i h%i for blit %s",
         lay->getName().c_str(), lay->geo.x, lay->geo.y,
         lay->geo.w, lay->geo.h, getName().c_str());

    // assign the right pointer to the *geo used in crop
    // we use the normal geometry if not roto|zoom
    // otherwise the layer::geo_rotozoom
    Geometry *geo;
    const Geometry &scr_geo = scr->getGeometry();
    if(lay->rotating | lay->zooming) {
        geo = &lay->geo_rotozoom;

        // shift up/left to center rotation
        geo->x = lay->geo.x - (geo->w - lay->geo.w) / 2;
        geo->y = lay->geo.y - (geo->h - lay->geo.h) / 2;

    } else {
        geo = &lay->geo;
    }

    //////////////////////

    lay_pitch =  geo->w; // how many pixels to copy each row
    lay_height = geo->h; // how many rows we should copy

    scr_stride_up = 0; // rows to jump before starting to blit on screen
    scr_stride_sx = 0; // screen pixels stride on the left of each row
    scr_stride_dx = 0; // screen pixels stride on the right of each row

    lay_stride_up = 0; // rows to jump before starting to blit layer
    lay_stride_sx = 0; // how many pixels stride on the left of each row
    lay_stride_dx = 0; // how many pixels stride on the right of each row

    // BOTTOM
    if(geo->y + geo->h > scr_geo.h) {
        if(geo->y > scr_geo.h) {   // out of screen
            geo->y = scr_geo.h + 1; // don't go far
            //TODO ? lay->hidden = true;
            return;
        } else { // partially out
            lay_height -= (geo->y + geo->h) - scr_geo.h;
        }
    }

    // LEFT
    if(geo->x < 0) {
        if(geo->x + geo->w < 0) {   // out of screen
            geo->x = -(geo->w + 1);   // don't go far
            //TODO ? lay->hidden = true;
            return;
        } else { // partially out
            lay_stride_sx += -geo->x;
            lay_pitch -= -geo->x;
        }
    } else { // inside
        scr_stride_sx += geo->x;
    }

    // UP
    if(geo->y < 0) {
        if(geo->y + geo->h < 0) {  // out of screen
            geo->y = -(geo->h + 1);   // don't go far
            //TODO ? lay->hidden = true;
            return;
        } else { // partially out
            lay_stride_up += -geo->y;
            lay_height -= -geo->y;
        }
    } else { // inside
        scr_stride_up += geo->y;
    }

    // RIGHT
    if(geo->x + geo->w > scr_geo.w) {
        if(geo->x > scr_geo.w) {   // out of screen
            geo->x = scr_geo.w + 1; // don't go far
            //TODO ? lay->hidden = true;
            return;
        } else { // partially out
            lay_pitch -= (geo->x + geo->w) - scr_geo.w;
            lay_stride_dx += (geo->x + geo->w) - scr_geo.w;
        }
    } else { // inside
        scr_stride_dx += scr_geo.w - (geo->x + geo->w);
    }

    //TODO ? lay->hidden = false;

    lay_stride = lay_stride_dx + lay_stride_sx; // sum strides
    // precalculate upper left starting offset for layer
    lay_offset = (lay_stride_sx +
                     (lay_stride_up * geo->w));

    scr_stride = scr_stride_dx + scr_stride_sx; // sum strides
    // precalculate upper left starting offset for screen
    scr_offset = (scr_stride_sx +
                     (scr_stride_up * scr_geo.w));
    // calculate bytes per row
    lay_bytepitch = lay_pitch * 4;

    //lay->need_crop = false;
    */
}

void AbstractLinearBlitInstance::init(LinearBlitPtr blit) {
    this->proto = blit;
    BlitInstance::init(blit);
}

LinearBlit::LinearBlit(const std::string &name, const std::string &description, linear_fct fct, LinkList<Parameter> &parameters) : Blit(name, description, parameters) {
    this->fct = fct;
}

LinearBlit::LinearBlit(const std::string &name, const std::string &description, linear_fct fct, LinkList<Parameter> &&parameters): LinearBlit(name, description, fct, parameters) {

}

LinearBlit::~LinearBlit() {

}


// Linear transparent blits
BLIT blit_xor(void *src, void *dst, int bytes, LinkList<ParameterInstance> &params) {
    register int c;
    register uint32_t *s = (uint32_t*)src;
    register uint32_t *d = (uint32_t*)dst;

    for(c = bytes >> 2; c > 0; c--, s++, d++)
        *d ^= *s;
}

BLIT rgb_jmemcpy(void *src, void *dst, int bytes, LinkList<ParameterInstance> &params) {
    jmemcpy(dst, src, bytes);
}

BLIT red_channel(void *src, void *dst, int bytes, LinkList<ParameterInstance> &params) {
    register int c;
    register uint8_t *s = (uint8_t*)src;
    register uint8_t *d = (uint8_t*)dst;
    for(c = bytes >> 2; c > 0; c--, s += 4, d += 4)
        *(d + rchan) = *(s + rchan);
}

BLIT green_channel(void *src, void *dst, int bytes, LinkList<ParameterInstance> &params) {
    register int c;
    register uint8_t *s = (uint8_t*)src;
    register uint8_t *d = (uint8_t*)dst;
    for(c = bytes >> 2; c > 0; c--, s += 4, d += 4)
        *(d + gchan) = *(s + gchan);
}

BLIT blue_channel(void *src, void *dst, int bytes, LinkList<ParameterInstance> &params) {
    register int c;
    register uint8_t *s = (uint8_t*)src;
    register uint8_t *d = (uint8_t*)dst;
    for(c = bytes >> 2; c > 0; c--, s += 4, d += 4)
        *(d + bchan) = *(s + bchan);
}

BLIT red_mask(void *src, void *dst, int bytes, LinkList<ParameterInstance> &params) {
    register int c;
    register uint32_t *s = (uint32_t*)src;
    register uint32_t *d = (uint32_t*)dst;

    for(c = bytes >> 2; c > 0; c--, s++, d++)
        *d |= *s & red_bitmask;

    unsigned char v;
    {
        v = (unsigned int) *(float*)(params.front()->get());  // only one value
    }

    SDL_imageFilterBinarizeUsingThreshold((unsigned char*)dst, (unsigned char*)dst, bytes, v);
}

BLIT green_mask(void *src, void *dst, int bytes, LinkList<ParameterInstance> &params) {
    register int c;
    register uint32_t *s = (uint32_t*)src;
    register uint32_t *d = (uint32_t*)dst;

    for(c = bytes >> 2; c > 0; c--, s++, d++)
        *d |= *s & green_bitmask;

    unsigned char v;
    {
        v = (unsigned int) *(float*)(params.front()->get());  // only one value
    }

    SDL_imageFilterBinarizeUsingThreshold((unsigned char*)dst, (unsigned char*)dst, bytes, v);
}

BLIT blue_mask(void *src, void *dst, int bytes, LinkList<ParameterInstance> &params) {
    register int c;
    register uint32_t *s = (uint32_t*)src;
    register uint32_t *d = (uint32_t*)dst;

    for(c = bytes >> 2; c > 0; c--, s++, d++)
        *d |= *s & blue_bitmask;

    unsigned char v;
    {
        v = (unsigned int) *(float*)(params.front()->get());  // only one value
    }

    SDL_imageFilterBinarizeUsingThreshold((unsigned char*)dst, (unsigned char*)dst, bytes, v);
}

BLIT schiffler_add(void *src, void *dst, int bytes, LinkList<ParameterInstance> &params) {
    SDL_imageFilterAdd((unsigned char*)src, (unsigned char*)dst, (unsigned char*)dst, bytes);
}

BLIT schiffler_sub(void *src, void *dst, int bytes, LinkList<ParameterInstance> &params) {
    SDL_imageFilterSub((unsigned char*)src, (unsigned char*)dst, (unsigned char*)dst, bytes);
}

BLIT schiffler_mean(void *src, void *dst, int bytes, LinkList<ParameterInstance> &params) {
    SDL_imageFilterMean((unsigned char*)src, (unsigned char*)dst, (unsigned char*)dst, bytes);
}

BLIT schiffler_absdiff(void *src, void *dst, int bytes, LinkList<ParameterInstance> &params) {
    SDL_imageFilterAbsDiff((unsigned char*)src, (unsigned char*)dst, (unsigned char*)dst, bytes);
}

BLIT schiffler_mult(void *src, void *dst, int bytes, LinkList<ParameterInstance> &params) {
    SDL_imageFilterMult((unsigned char*)src, (unsigned char*)dst, (unsigned char*)dst, bytes);
}

BLIT schiffler_multnor(void *src, void *dst, int bytes, LinkList<ParameterInstance> &params) {
    SDL_imageFilterMultNor((unsigned char*)src, (unsigned char*)dst, (unsigned char*)dst, bytes);
}

BLIT schiffler_div(void *src, void *dst, int bytes, LinkList<ParameterInstance> &params) {
    SDL_imageFilterDiv((unsigned char*)src, (unsigned char*)dst, (unsigned char*)dst, bytes);
}

BLIT schiffler_multdiv2(void *src, void *dst, int bytes, LinkList<ParameterInstance> &params) {
    SDL_imageFilterMultDivby2((unsigned char*)src, (unsigned char*)dst, (unsigned char*)dst, bytes);
}

BLIT schiffler_multdiv4(void *src, void *dst, int bytes, LinkList<ParameterInstance> &params) {
    SDL_imageFilterMultDivby2((unsigned char*)src, (unsigned char*)dst, (unsigned char*)dst, bytes);
}

BLIT schiffler_and(void *src, void *dst, int bytes, LinkList<ParameterInstance> &params) {
    SDL_imageFilterBitAnd((unsigned char*)src, (unsigned char*)dst, (unsigned char*)dst, bytes);
}

BLIT schiffler_or(void *src, void *dst, int bytes, LinkList<ParameterInstance> &params) {
    SDL_imageFilterBitOr((unsigned char*)src, (unsigned char*)dst, (unsigned char*)dst, bytes);
}

/* ====== end of transparent blits
   all the following blits can be considered effects
   they completely overwrite the underlying image */

/// Linear non-transparent blits

BLIT schiffler_neg(void *src, void *dst, int bytes, LinkList<ParameterInstance> &params) {
    SDL_imageFilterBitNegation((unsigned char*)src, (unsigned char*)dst, bytes);
}

BLIT schiffler_addbyte(void *src, void *dst, int bytes, LinkList<ParameterInstance> &params) {
    unsigned char v;
    {
        v = (unsigned int) *(float*)(params.front()->get());  // only one value
    }

    SDL_imageFilterAddByte((unsigned char*)src, (unsigned char*)dst, bytes, v);
}

BLIT schiffler_addbytetohalf(void *src, void *dst, int bytes, LinkList<ParameterInstance> &params) {
    unsigned char v;
    {
        v = (unsigned int) *(float*)(params.front()->get());  // only one value
    }

    SDL_imageFilterAddByteToHalf((unsigned char*)src, (unsigned char*)dst, bytes, v);
}

BLIT schiffler_subbyte(void *src, void *dst, int bytes, LinkList<ParameterInstance> &params) {
    unsigned char v;
    {
        v = (unsigned int) *(float*)(params.front()->get());  // only one value
    }

    SDL_imageFilterSubByte((unsigned char*)src, (unsigned char*)dst, bytes, v);
}

BLIT schiffler_shl(void *src, void *dst, int bytes, LinkList<ParameterInstance> &params) {
    unsigned char v;
    {
        v = (unsigned int) *(float*)(params.front()->get());  // only one value
    }

    SDL_imageFilterShiftLeft((unsigned char*)src, (unsigned char*)dst, bytes, v);
}

BLIT schiffler_shlb(void *src, void *dst, int bytes, LinkList<ParameterInstance> &params) {
    unsigned char v;
    {
        v = (unsigned int) *(float*)(params.front()->get());  // only one value
    }

    SDL_imageFilterShiftLeftByte((unsigned char*)src, (unsigned char*)dst, bytes, v);
}

BLIT schiffler_shr(void *src, void *dst, int bytes, LinkList<ParameterInstance> &params) {
    unsigned char v;
    {
        v = (unsigned int) *(float*)(params.front()->get());  // only one value
    }

    SDL_imageFilterShiftRight((unsigned char*)src, (unsigned char*)dst, bytes, v);
}

BLIT schiffler_mulbyte(void *src, void *dst, int bytes, LinkList<ParameterInstance> &params) {
    unsigned char v;
    {
        v = (unsigned int) *(float*)(params.front()->get());  // only one value
    }

    SDL_imageFilterMultByByte((unsigned char*)src, (unsigned char*)dst, bytes, v);
}

BLIT schiffler_binarize(void *src, void *dst, int bytes, LinkList<ParameterInstance> &params) {
    unsigned char v;
    {
        v = (unsigned int) *(float*)(params.front()->get());  // only one value
    }

    SDL_imageFilterBinarizeUsingThreshold
        ((unsigned char*)src, (unsigned char*)dst, bytes, v);
}

static LinkList<Blit> blits;
static bool init = false;

LinkList<Blit> &get_linear_blits() {
    if(!init) {

        /////////

        {
            blits.push_back(MakeShared<LinearBlit>(
                "RGB",
                "RGB blit (jmemcpy)",
                rgb_jmemcpy
            ));
        }

        /////////

        {
            blits.push_back(MakeShared<LinearBlit>(
                "ADD",
                "bytewise addition",
                schiffler_add
            ));
        }

        /////////

        {
            blits.push_back(MakeShared<LinearBlit>(
                "SUB",
                "bytewise subtraction",
                schiffler_sub
            ));
        }

        /////////

        {
            blits.push_back(MakeShared<LinearBlit>(
                "MEAN",
                "bytewise mean",
                schiffler_add
            ));
        }

        /////////

        {
            blits.push_back(MakeShared<LinearBlit>(
                "ABSDIFF",
                "absolute difference",
                schiffler_absdiff
            ));
        }

        /////////

        {
            blits.push_back(MakeShared<LinearBlit>(
                "MULT",
                "multiplication",
                schiffler_mult
            ));
        }

        /////////

        {
            blits.push_back(MakeShared<LinearBlit>(
                "MULTNOR",
                "normalized multiplication",
                schiffler_multnor
            ));
        }

        /////////

        {
            blits.push_back(MakeShared<LinearBlit>(
                "DIV",
                "division",
                schiffler_div
            ));
        }

        /////////

        {
            blits.push_back(MakeShared<LinearBlit>(
                "MULTDIV2",
                "multiplication and division by 2",
                schiffler_multdiv2
            ));
        }

        /////////

        {
            blits.push_back(MakeShared<LinearBlit>(
                "MULTDIV4",
                "multiplication and division by 4",
                schiffler_multdiv4
            ));
        }

        /////////

        {
            blits.push_back(MakeShared<LinearBlit>(
                "AND",
                "bitwise and",
                schiffler_and
            ));
        }

        /////////

        {
            blits.push_back(MakeShared<LinearBlit>(
                "OR",
                "bitwise or",
                schiffler_or
            ));
        }

        /////////

        {
            blits.push_back(MakeShared<LinearBlit>(
                "XOR",
                "bitwise xor",
                blit_xor
            ));
        }

        /////////

        {
            blits.push_back(MakeShared<LinearBlit>(
                "RED",
                "red channel only blit",
                red_channel
            ));
        }

        /////////

        {
            blits.push_back(MakeShared<LinearBlit>(
                "GREEN",
                "green channel only blit",
                green_channel
            ));
        }

        /////////

        {
            blits.push_back(MakeShared<LinearBlit>(
                "BLUE",
                "blue channel only blit",
                blue_channel
            ));
        }

        /////////

        {
            LinkList<Parameter> parameters;
            parameters.push_back(MakeShared<Parameter>(
                Parameter::NUMBER,
                "bit threshold",
                "bitmask threshold to apply to the red channel",
                255.0
            ));

            blits.push_back(MakeShared<LinearBlit>(
                "REDMASK",
                "red channel threshold mask",
                red_mask,
                parameters
            ));
        }

        /////////

        {
            LinkList<Parameter> parameters;
            parameters.push_back(MakeShared<Parameter>(
                Parameter::NUMBER,
                "bit threshold",
                "bitmask threshold to apply to the green channel",
                255.0
            ));

            blits.push_back(MakeShared<LinearBlit>(
                "GREENMASK",
                "green channel threshold mask",
                green_mask,
                parameters
            ));
        }

        ////////

        {
            LinkList<Parameter> parameters;
            parameters.push_back(MakeShared<Parameter>(
                Parameter::NUMBER,
                "bit threshold",
                "bitmask threshold to apply to the blue channel",
                255.0
            ));

            blits.push_back(MakeShared<LinearBlit>(
                "BLUEMASK",
                "blue channel threshold mask",
                blue_mask,
                parameters
            ));
        }

        /////////

        {
            blits.push_back(MakeShared<LinearBlit>(
                "NEG",
                "bitwise negation",
                schiffler_neg
            ));
        }

        /////////

        {
            LinkList<Parameter> parameters;
            parameters.push_back(MakeShared<Parameter>(
                Parameter::NUMBER,
                "byte increment",
                "amount to sum to the byte",
                255.0
            ));

            blits.push_back(MakeShared<LinearBlit>(
                "ADDB",
                "add byte to bytes",
                schiffler_addbyte,
                parameters
            ));
        }

        /////////

        {
            LinkList<Parameter> parameters;
            parameters.push_back(MakeShared<Parameter>(
                Parameter::NUMBER,
                "half byte increment",
                "amount to sum to the half byte",
                127.0
            ));

            blits.push_back(MakeShared<LinearBlit>(
                "ADDBH",
                "add byte to half",
                schiffler_addbytetohalf,
                parameters
            ));
        }

        /////////

        {
            LinkList<Parameter> parameters;
            parameters.push_back(MakeShared<Parameter>(
                Parameter::NUMBER,
                "byte decrement",
                "amount to substract to the pixel bytes",
                255.0
            ));

            blits.push_back(MakeShared<LinearBlit>(
                "SUBB",
                "subtract byte to bytes",
                schiffler_subbyte,
                parameters
            ));
        }

        /////////

        {
            LinkList<Parameter> parameters;
            parameters.push_back(MakeShared<Parameter>(
                Parameter::NUMBER,
                "byte decrement",
                "amount to substract to the pixel bytes",
                255.0
            ));

            blits.push_back(MakeShared<LinearBlit>(
                "SHL",
                "shift left bits",
                schiffler_shl,
                parameters
            ));
        }

        /////////

        {
            LinkList<Parameter> parameters;
            parameters.push_back(MakeShared<Parameter>(
                Parameter::NUMBER,
                "shift bits",
                "amount of left bit shifts to apply on each pixel's byte",
                8.0
            ));

            blits.push_back(MakeShared<LinearBlit>(
                "SHLB",
                "shift left byte",
                schiffler_shlb,
                parameters
            ));
        }

        /////////

        {
            LinkList<Parameter> parameters;
            parameters.push_back(MakeShared<Parameter>(
                Parameter::NUMBER,
                "shift bits",
                "amount of right bit shifts to apply on each pixel's byte",
                8.0
            ));

            blits.push_back(MakeShared<LinearBlit>(
                "SHR",
                "shift right bits",
                schiffler_shr,
                parameters
            ));
        }

        /////////

        {
            LinkList<Parameter> parameters;
            parameters.push_back(MakeShared<Parameter>(
                Parameter::NUMBER,
                "byte multiplier",
                "amount to multiply on each pixel's byte",
                255.0
            ));

            blits.push_back(MakeShared<LinearBlit>(
                "MULB",
                "multiply by byte",
                schiffler_mulbyte,
                parameters
            ));
        }

        /////////

        {
            LinkList<Parameter> parameters;
            parameters.push_back(MakeShared<Parameter>(
                Parameter::NUMBER,
                "threshold",
                "binary threshold value",
                255.0
            ));

            blits.push_back(MakeShared<LinearBlit>(
                "BIN",
                "binarize using threshold",
                schiffler_binarize,
                parameters
            ));
        }

        init = true;
    }

    return blits;
}

