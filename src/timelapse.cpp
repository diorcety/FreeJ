/*  FreeJ
 *  (c) Copyright 2001-2009 Denis Roio <jaromil@dyne.org>
 *                2008-2009 Christoph Rudorff <goil@dyne.org>
 *                     2009 Andrea Guzzo <xant@xant.net>
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

#include "timelapse.h"
#include <sys/time.h>
#include <cmath>

Timelapse::Timelapse() {
    timerclear(&elapsed_tv);
    running = false;
    ratio = 1.0;
}

Timelapse::~Timelapse() {
}

double Timelapse::getRatio() const {
    return ratio;
}

void Timelapse::setRatio(double ratio) {
    if(ratio == this->ratio) return;
    calc();
    this->ratio = ratio;
}

void Timelapse::start() {
    if(running) return;
    gettimeofday(&start_tv, NULL);
    running = true;
}

void Timelapse::reset() {
    gettimeofday(&start_tv, NULL);
    timerclear(&elapsed_tv);
}

void Timelapse::stop() {
    if(!running) return;
    calc();
}

bool Timelapse::isRunning() const {
    return running;
}

double Timelapse::getTime() {
    calc();
    return (double)elapsed_tv.tv_sec + (double)elapsed_tv.tv_usec/(double)1000000;
}

void Timelapse::calc() {
    if(!running) return;
    timeval done, now_tv;

    gettimeofday(&now_tv, NULL);
    timersub(&now_tv, &start_tv, &done);
    start_tv = now_tv;
    if(ratio != 1.0) {
        double val = (double)done.tv_sec + (double)done.tv_usec/(double)1000000;
        double fl_val = floor(val);
        done.tv_sec = fl_val;
        done.tv_usec = (val - fl_val) * (double)1000000;
    }
    timeradd(&elapsed_tv, &done, &elapsed_tv);
}
