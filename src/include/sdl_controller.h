#ifndef __SDL_CONTROLLER_H__
#define __SDL_CONTROLLER_H__

#include <controller.h>
#include <SDL.h>

FREEJ_FORWARD_PTR(SdlController)
class SdlController : public Controller {
protected:
    SDL_Event event; ///< SDL event structure

public:
    SdlController();
    ~SdlController();
    void poll_sdlevents(Uint32 eventmask);
    ///< helper function to filter and redispatch unhandled SDL_Events
    ///< calls dispatch() foreach event in eventmask

};

#endif
