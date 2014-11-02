/*  FreeJ
 *  (c) Copyright 2001-2009 Denis Roio <jaromil@dyne.org>
 *                2008-2009 Christoph Rudorff <goil@dyne.org>
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

#ifndef __TIMELAPSE_H__
#define __TIMELAPSE_H__

#include <time.h>
#include <sys/time.h>
#include <inttypes.h>

class Timelapse {
public:
    Timelapse();
    ~Timelapse();

    double getRatio() const;
    void setRatio(double ratio);
    void start();
    void stop();
    void reset();
    bool isRunning() const;
    double getTime();

private:
    void calc();

    bool running;
    double ratio;
    struct timeval start_tv;
    struct timeval elapsed_tv;
};

#endif

