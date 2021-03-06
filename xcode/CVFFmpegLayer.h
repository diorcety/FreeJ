/*  FreeJ
 *  (c) Copyright 2009 Andrea Guzzo <xant@dyne.org>
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

#ifndef __CVFFLAYER_H__
#define __CVFFLAYER_H__

#include <CVLayer.h>

struct ffdec;

class CVFFmpegLayer : public CVLayer
{
    
private:
    struct ffdec *ff;
protected:
    CVPixelBufferRef pixelBuffer;
    
    void *feed();

public:
    CVFFmpegLayer(CVLayerController *controller);
    ~CVFFmpegLayer();
    bool open(const char *movie);
    void close();
    bool isDecoding();
    bool hasFF();
    int scaledWidth();
    int scaledHeight();
};

#endif
