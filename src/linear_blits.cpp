
#include <config.h>
#include <stdlib.h>


#include <jutils.h>
#include <blitter.h>
#include <blit.h>
#include <parameter_instance.h>

#include <SDL_imageFilter.h>



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
        LockedLinkList<ParameterInstance> list = LockedLinkList<ParameterInstance>(params);
        v = (unsigned int) *(float*)(list.front()->get());  // only one value
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
        LockedLinkList<ParameterInstance> list = LockedLinkList<ParameterInstance>(params);
        v = (unsigned int) *(float*)(list.front()->get());  // only one value
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
        LockedLinkList<ParameterInstance> list = LockedLinkList<ParameterInstance>(params);
        v = (unsigned int) *(float*)(list.front()->get());  // only one value
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
        LockedLinkList<ParameterInstance> list = LockedLinkList<ParameterInstance>(params);
        v = (unsigned int) *(float*)(list.front()->get());  // only one value
    }

    SDL_imageFilterAddByte((unsigned char*)src, (unsigned char*)dst, bytes, v);
}

BLIT schiffler_addbytetohalf(void *src, void *dst, int bytes, LinkList<ParameterInstance> &params) {
    unsigned char v;
    {
        LockedLinkList<ParameterInstance> list = LockedLinkList<ParameterInstance>(params);
        v = (unsigned int) *(float*)(list.front()->get());  // only one value
    }

    SDL_imageFilterAddByteToHalf((unsigned char*)src, (unsigned char*)dst, bytes, v);
}

BLIT schiffler_subbyte(void *src, void *dst, int bytes, LinkList<ParameterInstance> &params) {
    unsigned char v;
    {
        LockedLinkList<ParameterInstance> list = LockedLinkList<ParameterInstance>(params);
        v = (unsigned int) *(float*)(list.front()->get());  // only one value
    }

    SDL_imageFilterSubByte((unsigned char*)src, (unsigned char*)dst, bytes, v);
}

BLIT schiffler_shl(void *src, void *dst, int bytes, LinkList<ParameterInstance> &params) {
    unsigned char v;
    {
        LockedLinkList<ParameterInstance> list = LockedLinkList<ParameterInstance>(params);
        v = (unsigned int) *(float*)(list.front()->get());  // only one value
    }

    SDL_imageFilterShiftLeft((unsigned char*)src, (unsigned char*)dst, bytes, v);
}

BLIT schiffler_shlb(void *src, void *dst, int bytes, LinkList<ParameterInstance> &params) {
    unsigned char v;
    {
        LockedLinkList<ParameterInstance> list = LockedLinkList<ParameterInstance>(params);
        v = (unsigned int) *(float*)(list.front()->get());  // only one value
    }

    SDL_imageFilterShiftLeftByte((unsigned char*)src, (unsigned char*)dst, bytes, v);
}

BLIT schiffler_shr(void *src, void *dst, int bytes, LinkList<ParameterInstance> &params) {
    unsigned char v;
    {
        LockedLinkList<ParameterInstance> list = LockedLinkList<ParameterInstance>(params);
        v = (unsigned int) *(float*)(list.front()->get());  // only one value
    }

    SDL_imageFilterShiftRight((unsigned char*)src, (unsigned char*)dst, bytes, v);
}

BLIT schiffler_mulbyte(void *src, void *dst, int bytes, LinkList<ParameterInstance> &params) {
    unsigned char v;
    {
        LockedLinkList<ParameterInstance> list = LockedLinkList<ParameterInstance>(params);
        v = (unsigned int) *(float*)(list.front()->get());  // only one value
    }

    SDL_imageFilterMultByByte((unsigned char*)src, (unsigned char*)dst, bytes, v);
}

BLIT schiffler_binarize(void *src, void *dst, int bytes, LinkList<ParameterInstance> &params) {
    unsigned char v;
    {
        LockedLinkList<ParameterInstance> list = LockedLinkList<ParameterInstance>(params);
        v = (unsigned int) *(float*)(list.front()->get());  // only one value
    }

    SDL_imageFilterBinarizeUsingThreshold
        ((unsigned char*)src, (unsigned char*)dst, bytes, v);
}

void setup_linear_blits(BlitterPtr blitter) {
    BlitPtr b;
    ParameterPtr p;

    LockedLinkList<Blit> list = LockedLinkList<Blit>(blitter->blitlist);

    /////////

    {
        list.push_back(MakeShared<Blit>(
            Blit::LINEAR,
            "RGB",
            "RGB blit (jmemcpy)",
            (void*)rgb_jmemcpy
        ));
    }

    /////////

    {
        list.push_back(MakeShared<Blit>(
            Blit::LINEAR,
            "ADD",
            "bytewise addition",
            (void*)schiffler_add
        ));
    }

    /////////

    {
        list.push_back(MakeShared<Blit>(
            Blit::LINEAR,
            "SUB",
            "bytewise subtraction",
            (void*)schiffler_sub
        ));
    }

    /////////

    {
        list.push_back(MakeShared<Blit>(
            Blit::LINEAR,
            "MEAN",
            "bytewise mean",
            (void*)schiffler_add
        ));
    }

    /////////

    {
        list.push_back(MakeShared<Blit>(
            Blit::LINEAR,
            "ABSDIFF",
            "absolute difference",
            (void*)schiffler_absdiff
        ));
    }

    /////////

    {
        list.push_back(MakeShared<Blit>(
            Blit::LINEAR,
            "MULT",
            "multiplication",
            (void*)schiffler_mult
        ));
    }

    /////////

    {
        list.push_back(MakeShared<Blit>(
            Blit::LINEAR,
            "MULTNOR",
            "normalized multiplication",
            (void*)schiffler_multnor
        ));
    }

    /////////

    {
        list.push_back(MakeShared<Blit>(
            Blit::LINEAR,
            "DIV",
            "division",
            (void*)schiffler_div
        ));
    }

    /////////

    {
        list.push_back(MakeShared<Blit>(
            Blit::LINEAR,
            "MULTDIV2",
            "multiplication and division by 2",
            (void*)schiffler_multdiv2
        ));
    }

    /////////

    {
        list.push_back(MakeShared<Blit>(
            Blit::LINEAR,
            "MULTDIV4",
            "multiplication and division by 4",
            (void*)schiffler_multdiv4
        ));
    }

    /////////

    {
        list.push_back(MakeShared<Blit>(
            Blit::LINEAR,
            "AND",
            "bitwise and",
            (void*)schiffler_and
        ));
    }

    /////////

    {
        list.push_back(MakeShared<Blit>(
            Blit::LINEAR,
            "OR",
            "bitwise or",
            (void*)schiffler_or
        ));
    }

    /////////

    {
        list.push_back(MakeShared<Blit>(
            Blit::LINEAR,
            "XOR",
            "bitwise xor",
            (void*)blit_xor
        ));
    }

    /////////

    {
        list.push_back(MakeShared<Blit>(
            Blit::LINEAR,
            "RED",
            "red channel only blit",
            (void*)red_channel
        ));
    }

    /////////

    {
        list.push_back(MakeShared<Blit>(
            Blit::LINEAR,
            "GREEN",
            "green channel only blit",
            (void*)green_channel
        ));
    }

    /////////

    {
        list.push_back(MakeShared<Blit>(
            Blit::LINEAR,
            "BLUE",
            "blue channel only blit",
            (void*)blue_channel
        ));
    }

    /////////

    {
        LinkList<Parameter> parameters;
        LockedLinkList<Parameter> p = LockedLinkList<Parameter>(parameters);
        p.push_back(MakeShared<Parameter>(
            Parameter::NUMBER,
            "bit threshold",
            "bitmask threshold to apply to the red channel",
            255.0
        ));

        list.push_back(MakeShared<Blit>(
            Blit::LINEAR,
            "REDMASK",
            "red channel threshold mask",
            (void*)red_mask,
            parameters
        ));
    }

    /////////

    {
        LinkList<Parameter> parameters;
        LockedLinkList<Parameter> p = LockedLinkList<Parameter>(parameters);
        p.push_back(MakeShared<Parameter>(
            Parameter::NUMBER,
            "bit threshold",
            "bitmask threshold to apply to the green channel",
            255.0
        ));

        list.push_back(MakeShared<Blit>(
            Blit::LINEAR,
            "GREENMASK",
            "green channel threshold mask",
            (void*)green_mask,
            parameters
        ));
    }

    ////////

    {
        LinkList<Parameter> parameters;
        LockedLinkList<Parameter> p = LockedLinkList<Parameter>(parameters);
        p.push_back(MakeShared<Parameter>(
            Parameter::NUMBER,
            "bit threshold",
            "bitmask threshold to apply to the blue channel",
            255.0
        ));

        list.push_back(MakeShared<Blit>(
            Blit::LINEAR,
            "BLUEMASK",
            "blue channel threshold mask",
            (void*)blue_mask,
            parameters
        ));
    }

    /////////

    {
        list.push_back(MakeShared<Blit>(
            Blit::LINEAR,
            "NEG",
            "bitwise negation",
            (void*)schiffler_neg
        ));
    }

    /////////

    {
        LinkList<Parameter> parameters;
        LockedLinkList<Parameter> p = LockedLinkList<Parameter>(parameters);
        p.push_back(MakeShared<Parameter>(
            Parameter::NUMBER,
            "byte increment",
            "amount to sum to the byte",
            255.0
        ));

        list.push_back(MakeShared<Blit>(
            Blit::LINEAR,
            "ADDB",
            "add byte to bytes",
            (void*)schiffler_addbyte,
            parameters
        ));
    }

    /////////

    {
        LinkList<Parameter> parameters;
        LockedLinkList<Parameter> p = LockedLinkList<Parameter>(parameters);
        p.push_back(MakeShared<Parameter>(
            Parameter::NUMBER,
            "half byte increment",
            "amount to sum to the half byte",
            127.0
        ));

        list.push_back(MakeShared<Blit>(
            Blit::LINEAR,
            "ADDBH",
            "add byte to half",
            (void*)schiffler_addbytetohalf,
            parameters
        ));
    }

    /////////

    {
        LinkList<Parameter> parameters;
        LockedLinkList<Parameter> p = LockedLinkList<Parameter>(parameters);
        p.push_back(MakeShared<Parameter>(
            Parameter::NUMBER,
            "byte decrement",
            "amount to substract to the pixel bytes",
            255.0
        ));

        list.push_back(MakeShared<Blit>(
            Blit::LINEAR,
            "SUBB",
            "subtract byte to bytes",
            (void*)schiffler_subbyte,
            parameters
        ));
    }

    /////////

    {
        LinkList<Parameter> parameters;
        LockedLinkList<Parameter> p = LockedLinkList<Parameter>(parameters);
        p.push_back(MakeShared<Parameter>(
            Parameter::NUMBER,
            "byte decrement",
            "amount to substract to the pixel bytes",
            255.0
        ));

        list.push_back(MakeShared<Blit>(
            Blit::LINEAR,
            "SHL",
            "shift left bits",
            (void*)schiffler_shl,
            parameters
        ));
    }

    /////////

    {
        LinkList<Parameter> parameters;
        LockedLinkList<Parameter> p = LockedLinkList<Parameter>(parameters);
        p.push_back(MakeShared<Parameter>(
            Parameter::NUMBER,
            "shift bits",
            "amount of left bit shifts to apply on each pixel's byte",
            8.0
        ));

        list.push_back(MakeShared<Blit>(
            Blit::LINEAR,
            "SHLB",
            "shift left byte",
            (void*)schiffler_shlb,
            parameters
        ));
    }

    /////////

    {
        LinkList<Parameter> parameters;
        LockedLinkList<Parameter> p = LockedLinkList<Parameter>(parameters);
        p.push_back(MakeShared<Parameter>(
            Parameter::NUMBER,
            "shift bits",
            "amount of right bit shifts to apply on each pixel's byte",
            8.0
        ));

        list.push_back(MakeShared<Blit>(
            Blit::LINEAR,
            "SHR",
            "shift right bits",
            (void*)schiffler_shr,
            parameters
        ));
    }

    /////////

    {
        LinkList<Parameter> parameters;
        LockedLinkList<Parameter> p = LockedLinkList<Parameter>(parameters);
        p.push_back(MakeShared<Parameter>(
            Parameter::NUMBER,
            "byte multiplier",
            "amount to multiply on each pixel's byte",
            255.0
        ));

        list.push_back(MakeShared<Blit>(
            Blit::LINEAR,
            "MULB",
            "multiply by byte",
            (void*)schiffler_mulbyte,
            parameters
        ));
    }

    /////////

    {
        LinkList<Parameter> parameters;
        LockedLinkList<Parameter> p = LockedLinkList<Parameter>(parameters);
        p.push_back(MakeShared<Parameter>(
            Parameter::NUMBER,
            "threshold",
            "binary threshold value",
            255.0
        ));

        list.push_back(MakeShared<Blit>(
            Blit::LINEAR,
            "BIN",
            "binarize using threshold",
            (void*)schiffler_binarize,
            parameters
        ));
    }


}

