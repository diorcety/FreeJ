/*  FreeJ
 *  (c) Copyright 2001-2009 Denis Roio aka jaromil <jaromil@dyne.org>
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
 */

#include "geometry.h"

Geometry::Geometry() {
    init(0, 0, 0);
    transformation.setIdentity();
}

Geometry::~Geometry() {
}

void Geometry::init(int nw, int nh, int nbpp) {
    size = Size(nw, nh, 0);
    bpp = nbpp;
    pixelsize = nw * nh;
    bytesize  = nw * nh * (bpp / 8);
    bytewidth = nw * (bpp / 8);
}

const Transformation &Geometry::getTransformation() const {
    return transformation;
}

Transformation &Geometry::getTransformation() {
    return transformation;
}

uint8_t Geometry::getBpp() const {
    return bpp;
}

const Size &Geometry::getSize() const {
   return size;
}

uint32_t Geometry::getPixelSize() const {
    return pixelsize;
}

uint32_t Geometry::getByteSize() const {
    return bytesize;
}

uint16_t Geometry::getByteWidth() const {
    return bytewidth;
}


