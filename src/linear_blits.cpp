
#include <config.h>
#include <stdlib.h>


#include <jutils.h>
#include <blitter.h>

#include <SDL_imageFilter.h>



// Linear transparent blits
BLIT blit_xor(void *src, void *dst, int bytes, Linklist<Parameter> *params) {
    register int c;
    register uint32_t *s = (uint32_t*)src;
    register uint32_t *d = (uint32_t*)dst;

    for(c = bytes >> 2; c > 0; c--, s++, d++)
        *d ^= *s;
}

BLIT rgb_jmemcpy(void *src, void *dst, int bytes, Linklist<Parameter> *params) {
    jmemcpy(dst, src, bytes);
}

BLIT red_channel(void *src, void *dst, int bytes, Linklist<Parameter> *params) {
    register int c;
    register uint8_t *s = (uint8_t*)src;
    register uint8_t *d = (uint8_t*)dst;
    for(c = bytes >> 2; c > 0; c--, s += 4, d += 4)
        *(d + rchan) = *(s + rchan);
}

BLIT green_channel(void *src, void *dst, int bytes, Linklist<Parameter> *params) {
    register int c;
    register uint8_t *s = (uint8_t*)src;
    register uint8_t *d = (uint8_t*)dst;
    for(c = bytes >> 2; c > 0; c--, s += 4, d += 4)
        *(d + gchan) = *(s + gchan);
}

BLIT blue_channel(void *src, void *dst, int bytes, Linklist<Parameter> *params) {
    register int c;
    register uint8_t *s = (uint8_t*)src;
    register uint8_t *d = (uint8_t*)dst;
    for(c = bytes >> 2; c > 0; c--, s += 4, d += 4)
        *(d + bchan) = *(s + bchan);
}

BLIT red_mask(void *src, void *dst, int bytes, Linklist<Parameter> *params) {
    register int c;
    register uint32_t *s = (uint32_t*)src;
    register uint32_t *d = (uint32_t*)dst;

    for(c = bytes >> 2; c > 0; c--, s++, d++)
        *d |= *s & red_bitmask;

    unsigned char v;
    {
        LockedLinkList<Parameter> list = params->getLock();
        v = (unsigned int) *(float*)(list.front()->value);  // only one value
    }

    SDL_imageFilterBinarizeUsingThreshold((unsigned char*)dst, (unsigned char*)dst, bytes, v);
}

BLIT green_mask(void *src, void *dst, int bytes, Linklist<Parameter> *params) {
    register int c;
    register uint32_t *s = (uint32_t*)src;
    register uint32_t *d = (uint32_t*)dst;

    for(c = bytes >> 2; c > 0; c--, s++, d++)
        *d |= *s & green_bitmask;

    unsigned char v;
    {
        LockedLinkList<Parameter> list = params->getLock();
        v = (unsigned int) *(float*)(list.front()->value);  // only one value
    }

    SDL_imageFilterBinarizeUsingThreshold((unsigned char*)dst, (unsigned char*)dst, bytes, v);
}

BLIT blue_mask(void *src, void *dst, int bytes, Linklist<Parameter> *params) {
    register int c;
    register uint32_t *s = (uint32_t*)src;
    register uint32_t *d = (uint32_t*)dst;

    for(c = bytes >> 2; c > 0; c--, s++, d++)
        *d |= *s & blue_bitmask;

    unsigned char v;
    {
        LockedLinkList<Parameter> list = params->getLock();
        v = (unsigned int) *(float*)(list.front()->value);  // only one value
    }

    SDL_imageFilterBinarizeUsingThreshold((unsigned char*)dst, (unsigned char*)dst, bytes, v);
}

BLIT schiffler_add(void *src, void *dst, int bytes, Linklist<Parameter> *params) {
    SDL_imageFilterAdd((unsigned char*)src, (unsigned char*)dst, (unsigned char*)dst, bytes);
}

BLIT schiffler_sub(void *src, void *dst, int bytes, Linklist<Parameter> *params) {
    SDL_imageFilterSub((unsigned char*)src, (unsigned char*)dst, (unsigned char*)dst, bytes);
}

BLIT schiffler_mean(void *src, void *dst, int bytes, Linklist<Parameter> *params) {
    SDL_imageFilterMean((unsigned char*)src, (unsigned char*)dst, (unsigned char*)dst, bytes);
}

BLIT schiffler_absdiff(void *src, void *dst, int bytes, Linklist<Parameter> *params) {
    SDL_imageFilterAbsDiff((unsigned char*)src, (unsigned char*)dst, (unsigned char*)dst, bytes);
}

BLIT schiffler_mult(void *src, void *dst, int bytes, Linklist<Parameter> *params) {
    SDL_imageFilterMult((unsigned char*)src, (unsigned char*)dst, (unsigned char*)dst, bytes);
}

BLIT schiffler_multnor(void *src, void *dst, int bytes, Linklist<Parameter> *params) {
    SDL_imageFilterMultNor((unsigned char*)src, (unsigned char*)dst, (unsigned char*)dst, bytes);
}

BLIT schiffler_div(void *src, void *dst, int bytes, Linklist<Parameter> *params) {
    SDL_imageFilterDiv((unsigned char*)src, (unsigned char*)dst, (unsigned char*)dst, bytes);
}

BLIT schiffler_multdiv2(void *src, void *dst, int bytes, Linklist<Parameter> *params) {
    SDL_imageFilterMultDivby2((unsigned char*)src, (unsigned char*)dst, (unsigned char*)dst, bytes);
}

BLIT schiffler_multdiv4(void *src, void *dst, int bytes, Linklist<Parameter> *params) {
    SDL_imageFilterMultDivby2((unsigned char*)src, (unsigned char*)dst, (unsigned char*)dst, bytes);
}

BLIT schiffler_and(void *src, void *dst, int bytes, Linklist<Parameter> *params) {
    SDL_imageFilterBitAnd((unsigned char*)src, (unsigned char*)dst, (unsigned char*)dst, bytes);
}

BLIT schiffler_or(void *src, void *dst, int bytes, Linklist<Parameter> *params) {
    SDL_imageFilterBitOr((unsigned char*)src, (unsigned char*)dst, (unsigned char*)dst, bytes);
}

/* ====== end of transparent blits
   all the following blits can be considered effects
   they completely overwrite the underlying image */

/// Linear non-transparent blits

BLIT schiffler_neg(void *src, void *dst, int bytes, Linklist<Parameter> *params) {
    SDL_imageFilterBitNegation((unsigned char*)src, (unsigned char*)dst, bytes);
}

BLIT schiffler_addbyte(void *src, void *dst, int bytes, Linklist<Parameter> *params) {
    unsigned char v;
    {
        LockedLinkList<Parameter> list = params->getLock();
        v = (unsigned int) *(float*)(list.front()->value);  // only one value
    }

    SDL_imageFilterAddByte((unsigned char*)src, (unsigned char*)dst, bytes, v);
}

BLIT schiffler_addbytetohalf(void *src, void *dst, int bytes, Linklist<Parameter> *params) {
    unsigned char v;
    {
        LockedLinkList<Parameter> list = params->getLock();
        v = (unsigned int) *(float*)(list.front()->value);  // only one value
    }

    SDL_imageFilterAddByteToHalf((unsigned char*)src, (unsigned char*)dst, bytes, v);
}

BLIT schiffler_subbyte(void *src, void *dst, int bytes, Linklist<Parameter> *params) {
    unsigned char v;
    {
        LockedLinkList<Parameter> list = params->getLock();
        v = (unsigned int) *(float*)(list.front()->value);  // only one value
    }

    SDL_imageFilterSubByte((unsigned char*)src, (unsigned char*)dst, bytes, v);
}

BLIT schiffler_shl(void *src, void *dst, int bytes, Linklist<Parameter> *params) {
    unsigned char v;
    {
        LockedLinkList<Parameter> list = params->getLock();
        v = (unsigned int) *(float*)(list.front()->value);  // only one value
    }

    SDL_imageFilterShiftLeft((unsigned char*)src, (unsigned char*)dst, bytes, v);
}

BLIT schiffler_shlb(void *src, void *dst, int bytes, Linklist<Parameter> *params) {
    unsigned char v;
    {
        LockedLinkList<Parameter> list = params->getLock();
        v = (unsigned int) *(float*)(list.front()->value);  // only one value
    }

    SDL_imageFilterShiftLeftByte((unsigned char*)src, (unsigned char*)dst, bytes, v);
}

BLIT schiffler_shr(void *src, void *dst, int bytes, Linklist<Parameter> *params) {
    unsigned char v;
    {
        LockedLinkList<Parameter> list = params->getLock();
        v = (unsigned int) *(float*)(list.front()->value);  // only one value
    }

    SDL_imageFilterShiftRight((unsigned char*)src, (unsigned char*)dst, bytes, v);
}

BLIT schiffler_mulbyte(void *src, void *dst, int bytes, Linklist<Parameter> *params) {
    unsigned char v;
    {
        LockedLinkList<Parameter> list = params->getLock();
        v = (unsigned int) *(float*)(list.front()->value);  // only one value
    }

    SDL_imageFilterMultByByte((unsigned char*)src, (unsigned char*)dst, bytes, v);
}

BLIT schiffler_binarize(void *src, void *dst, int bytes, Linklist<Parameter> *params) {
    unsigned char v;
    {
        LockedLinkList<Parameter> list = params->getLock();
        v = (unsigned int) *(float*)(list.front()->value);  // only one value
    }

    SDL_imageFilterBinarizeUsingThreshold
        ((unsigned char*)src, (unsigned char*)dst, bytes, v);
}

void setup_linear_blits(BlitterPtr blitter) {
    BlitPtr b;
    ParameterPtr p;

    LockedLinkList<Blit> list = blitter->blitlist.getLock();

    b = MakeShared<Blit>();
    b->setName("RGB");
    b->setDescription("RGB blit (jmemcpy)");
    b->type = Blit::LINEAR;
    b->fun = rgb_jmemcpy;
    list.push_front(b);

    blitter->default_blit = b; // default is RGB

    b = MakeShared<Blit>();
    b->setName("ADD");
    b->setDescription("bytewise addition");
    b->type = Blit::LINEAR;
    b->fun = schiffler_add;
    list.push_front(b);

    b = MakeShared<Blit>();
    b->setName("SUB");
    b->setDescription("bytewise subtraction");
    b->type = Blit::LINEAR;
    b->fun = schiffler_sub;
    list.push_front(b);

    b = MakeShared<Blit>();
    b->setName("MEAN");
    b->setDescription("bytewise mean");
    b->type = Blit::LINEAR;
    b->fun = schiffler_add;
    list.push_front(b);

    b = MakeShared<Blit>();
    b->setName("ABSDIFF");
    b->setDescription("absolute difference");
    b->type = Blit::LINEAR;
    b->fun = schiffler_absdiff;
    list.push_front(b);

    b = MakeShared<Blit>();
    b->setName("MULT");
    b->setDescription("multiplication");
    b->type = Blit::LINEAR;
    b->fun = schiffler_mult;
    list.push_front(b);

    b = MakeShared<Blit>();
    b->setName("MULTNOR");
    b->setDescription("normalized multiplication");
    b->type = Blit::LINEAR;
    b->fun = schiffler_multnor;
    list.push_front(b);

    b = MakeShared<Blit>();
    b->setName("DIV");
    b->setDescription("division");
    b->type = Blit::LINEAR;
    b->fun = schiffler_div;
    list.push_front(b);

    b = MakeShared<Blit>();
    b->setName("MULTDIV2");
    b->setDescription("multiplication and division by 2");
    b->type = Blit::LINEAR;
    b->fun = schiffler_multdiv2;
    list.push_front(b);

    b = MakeShared<Blit>();
    b->setName("MULTDIV4");
    b->setDescription("multiplication and division by 4");
    b->type = Blit::LINEAR;
    b->fun = schiffler_multdiv4;
    list.push_front(b);

    b = MakeShared<Blit>();
    b->setName("AND");
    b->setDescription("bitwise and");
    b->type = Blit::LINEAR;
    b->fun = schiffler_and;
    list.push_front(b);

    b = MakeShared<Blit>();
    b->setName("OR");
    b->setDescription("bitwise or");
    b->type = Blit::LINEAR;
    b->fun = schiffler_or;
    list.push_front(b);

    b = MakeShared<Blit>();
    b->setName("XOR");
    b->setDescription("bitwise xor");
    b->type = Blit::LINEAR;
    b->fun = blit_xor;
    list.push_front(b);

    b = MakeShared<Blit>();
    b->setName("RED");
    b->setDescription("red channel only blit");
    b->type = Blit::LINEAR;
    b->fun = red_channel;
    list.push_front(b);

    b = MakeShared<Blit>();
    b->setName("GREEN");
    b->setDescription("green channel only blit");
    b->type = Blit::LINEAR;
    b->fun = green_channel;
    list.push_front(b);

    b = MakeShared<Blit>();
    b->setName("BLUE");
    b->setDescription("blue channel only blit");
    b->type = Blit::LINEAR;
    b->fun = blue_channel;
    list.push_front(b);

    {
        b = MakeShared<Blit>();
        b->setName("REDMASK");
        b->setDescription("red channel threshold mask");
        b->type = Blit::LINEAR;
        b->fun = red_mask;
        list.push_front(b);

        LockedLinkList<Parameter> listP = b->parameters.getLock();

        p = MakeShared<Parameter>(Parameter::NUMBER);
        p->setName("bit threshold");
        p->setDescription("bitmask threshold to apply to the red channel");
        p->multiplier = 255.0;
        listP.push_back(p);
    }

    /////////

    {
        b = MakeShared<Blit>();
        b->setName("GREENMASK");
        b->setDescription("green channel threshold mask");
        b->type = Blit::LINEAR;
        b->fun = green_mask;
        list.push_front(b);

        LockedLinkList<Parameter> listP = b->parameters.getLock();

        p = MakeShared<Parameter>(Parameter::NUMBER);
        p->setName("bit threshold");
        p->setDescription("bitmask threshold to apply to the green channel");
        p->multiplier = 255.0;
        listP.push_back(p);
    }

    ////////

    {
        b = MakeShared<Blit>();
        b->setName("BLUEMASK");
        b->setDescription("blue channel threshold mask");
        b->type = Blit::LINEAR;
        b->fun = blue_mask;
        list.push_front(b);

        LockedLinkList<Parameter> listP = b->parameters.getLock();

        p = MakeShared<Parameter>(Parameter::NUMBER);
        p->setName("bit threshold");
        p->setDescription("bitmask threshold to apply to the blue channel");
        p->multiplier = 255.0;
        listP.push_back(p);
    }

    /////////

    b = MakeShared<Blit>();
    b->setName("NEG");
    b->setDescription("bitwise negation");
    b->type = Blit::LINEAR;
    b->fun = schiffler_neg;
    list.push_front(b);

    /////////

    {
        b = MakeShared<Blit>();
        b->setName("ADDB");
        b->setDescription("add byte to bytes");
        b->type = Blit::LINEAR;
        b->fun = schiffler_addbyte;
        list.push_front(b);

        LockedLinkList<Parameter> listP = b->parameters.getLock();

        p = MakeShared<Parameter>(Parameter::NUMBER);
        p->setName("byte increment");
        p->setDescription("amount to sum to the byte");
        p->multiplier = 255.0;
        listP.push_back(p);
    }

    /////////

    {
        b = MakeShared<Blit>();
        b->setName("ADDBH");
        b->setDescription("add byte to half");
        b->type = Blit::LINEAR;
        b->fun = schiffler_addbytetohalf;
        list.push_front(b);

        LockedLinkList<Parameter> listP = b->parameters.getLock();

        p = MakeShared<Parameter>(Parameter::NUMBER);
        p->setName("half byte increment");
        p->setDescription("amount to sum to the half byte");
        p->multiplier = 127.0;
        listP.push_back(p);
    }

    /////////

    {
        b = MakeShared<Blit>();
        b->setName("SUBB");
        b->setDescription("subtract byte to bytes");
        b->type = Blit::LINEAR;
        b->fun = schiffler_subbyte;
        list.push_front(b);

        LockedLinkList<Parameter> listP = b->parameters.getLock();

        p = MakeShared<Parameter>(Parameter::NUMBER);
        p->setName("byte decrement");
        p->setDescription("amount to substract to the pixel bytes");
        p->multiplier = 255.0;
        listP.push_back(p);
    }

    /////////

    {
        b = MakeShared<Blit>();
        b->setName("SHL");
        b->setDescription("shift left bits");
        b->type = Blit::LINEAR;
        b->fun = schiffler_shl;
        list.push_front(b);

        LockedLinkList<Parameter> listP = b->parameters.getLock();

        p = MakeShared<Parameter>(Parameter::NUMBER);
        p->setName("byte decrement");
        p->setDescription("amount to substract to the pixel bytes");
        p->multiplier = 255.0;
        listP.push_back(p);
    }

    /////////

    {
        b = MakeShared<Blit>();
        b->setName("SHLB");
        b->setDescription("shift left byte");
        b->type = Blit::LINEAR;
        b->fun = schiffler_shlb;
        list.push_front(b);

        LockedLinkList<Parameter> listP = b->parameters.getLock();

        p = MakeShared<Parameter>(Parameter::NUMBER);
        p->setName("shift bits");
        p->setDescription("amount of left bit shifts to apply on each pixel's byte");
        p->multiplier = 8.0;
        listP.push_back(p);
    }

    /////////

    {
        b = MakeShared<Blit>();
        b->setName("SHR");
        b->setDescription("shift right bits");
        b->type = Blit::LINEAR;
        b->fun = schiffler_shr;
        list.push_front(b);

        LockedLinkList<Parameter> listP = b->parameters.getLock();

        p = MakeShared<Parameter>(Parameter::NUMBER);
        p->setName("shift bits");
        p->setDescription("amount of right bit shifts to apply on each pixel's byte");
        p->multiplier = 8.0;
        listP.push_back(p);
    }

    /////////

    {
        b = MakeShared<Blit>();
        b->setName("MULB");
        b->setDescription("multiply by byte");
        b->type = Blit::LINEAR;
        b->fun = schiffler_mulbyte;
        list.push_front(b);

        LockedLinkList<Parameter> listP = b->parameters.getLock();

        p = MakeShared<Parameter>(Parameter::NUMBER);
        p->setName("byte multiplier");
        p->setDescription("amount to multiply on each pixel's byte");
        p->multiplier = 255.0;
        listP.push_back(p);
    }

    /////////

    {
        b = MakeShared<Blit>();
        b->setName("BIN");
        b->setDescription("binarize using threshold");
        b->type = Blit::LINEAR;
        b->fun = schiffler_binarize;
        list.push_front(b);

        LockedLinkList<Parameter> listP = b->parameters.getLock();

        p = MakeShared<Parameter>(Parameter::NUMBER);
        p->setName("threshold");
        p->setDescription("binary threshold value");
        p->multiplier = 255.0;
        listP.push_back(p);
    }


}

