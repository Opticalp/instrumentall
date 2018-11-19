/**
 * @file	src/modules/imageProc/RotCrop.h
 * @date    Jan. 2017
 * @author	PhRG - opticalp.fr
 */

/*
 Copyright (c) 2017 Ph. Renaud-Goud / Opticalp

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */

#ifndef SRC_MODULES_IMAGEPROC_ROTCROP_H_
#define SRC_MODULES_IMAGEPROC_ROTCROP_H_

#ifdef HAVE_OPENCV

#include "core/Module.h"

/**
 * Rotate and crop an image
 * 
 * The parameters (center, size, angle) can be set using parameter setters
 * if the sample position is detected automatically
 */
class RotCrop: public Module
{
public:
    RotCrop(ModuleFactory* parent, std::string customName);

    std::string description()
    {
        return "Rotate, then crop the input image. "
                "The rotation and the crop are centered on the same point. "
                "The behavior is not defined for out-of-image pixels";
    }

private:
    /**
     * Main logic
     */
    void process(int startCond);

    static size_t refCount; ///< reference counter to generate a unique internal name

    enum params
    {
        paramXCenter,
        paramYCenter,
        paramHeight,
        paramWidth,
        paramAngle,
        paramCnt
    };

    Poco::Int64 getIntParameterValue(size_t paramIndex);
    void setIntParameterValue(size_t paramIndex, Poco::Int64 value);

    // angle
    double getFloatParameterValue(size_t paramIndex);
    void setFloatParameterValue(size_t paramIndex, double value);

    Poco::Int64 xCenter, yCenter;
    Poco::Int64 width, height;
    double angle;

    /// Indexes of the input ports
    enum inPorts
    {
        imageInPort,
        inPortCnt
    };

    /// Indexes of the output ports
    enum outPorts
    {
        imageOutPort,
        outPortCnt
    };

};

#endif /* HAVE_OPENCV */
#endif /* SRC_MODULES_IMAGEPROC_ROTCROP_H_ */
