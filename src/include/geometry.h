/*  FreeJ
 *  (c) Copyright 2001-2010 Denis Roio <jaromil@dyne.org>
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

#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <inttypes.h>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <Eigen/SVD>
#include "sharedptr.h"

typedef Eigen::Affine3d Transformation;
typedef Eigen::Matrix3d Matrix;
typedef Eigen::Vector3d Vector;
typedef Eigen::Array3d Array;
typedef Eigen::Translation3d Translation;
typedef Eigen::AngleAxisd Rotation;
typedef Eigen::AlignedScaling3d Scaling;
typedef Eigen::Vector3i Size;

/**
   This class is made to hold informations about the geometry of
   various video components in FreeJ, describing their format and
   image bounds.

   @brief Collects geometrical information about Layer, Screen and other components
 */
class Geometry {

public:

    Geometry();
    virtual ~Geometry();

    void init(int nw, int nh, int nbpp);

private:
    Transformation transformation;
    Size size;
    uint8_t bpp; ///< bits per pixel

    uint32_t pixelsize; ///< size of the whole frame in pixels
    uint32_t bytesize; ///< size of the whole frame in bytes
    uint16_t bytewidth; ///< width of frame in bytes (also called pitch or stride)

public:
    const Transformation &getTransformation() const;
    Transformation &getTransformation();

    const Size &getSize() const;
    
    uint8_t getBpp() const;
    uint32_t getPixelSize() const;
    uint32_t getByteSize() const;
    uint16_t getByteWidth() const;


};

#include <sstream>

inline std::string printTransformation(const Transformation &transformation) {
    std::stringstream ss;
    Matrix rotation;
    Matrix scaling;
    transformation.computeRotationScaling(&rotation, &scaling);
    Translation translation(transformation.translation());
    ss << transformation.matrix() << std::endl;
    ss << "................" << std::endl;
    ss << translation*Vector(0,0,0) << std::endl;
    ss << scaling << std::endl;
    ss << rotation << std::endl;
    return ss.str();
}

inline std::string printTransformation(Transformation &&transformation) {
    std::stringstream ss;
    Matrix rotation;
    Matrix scaling;
    transformation.computeRotationScaling(&rotation, &scaling);
    Translation translation(transformation.translation());
    ss << transformation.matrix() << std::endl;
    ss << "................" << std::endl;
    ss << translation*Vector(0,0,0) << std::endl;
    ss << scaling << std::endl;
    ss << rotation << std::endl;
    return ss.str();
}

#endif /* __GEOMETRY_H__ */

