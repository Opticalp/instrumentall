/**
 * @file	src/modules/imageProc/ImgStats.h
 * @date    Feb. 2017
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

#ifndef SRC_MODULES_IMAGEPROC_IMGSTATS_H_
#define SRC_MODULES_IMAGEPROC_IMGSTATS_H_

#ifdef HAVE_OPENCV

#include "core/Module.h"

/**
 * Retrieve simple stats of an image
 * 
 * Width, height, mean value, min, max
 * 
 * @par ver 2.2.1
 * With mask
 */
class ImgStats: public Module
{
public:
    ImgStats(ModuleFactory* parent, std::string customName);

    std::string description()
    {
        return "Retrieve source image simple statistics: "
                "height, width, mean, min, max. ";
    }

private:
    /**
     * Main logic
     */
    void process(int startCond);

    static size_t refCount; ///< reference counter to generate a unique internal name

    /// Indexes of the input ports
    enum inPorts
    {
		maskPort,
		imagePort,
        inPortCnt
    };

    /// Indexes of the output ports
    enum outPorts
    {
        heightPort,
        widthPort,
        meanPort,
        sigmaPort,
        minPort,
        maxPort,
        outPortCnt
    };

};

#endif /* HAVE_OPENCV */
#endif /* SRC_MODULES_IMAGEPROC_IMGSTATS_H_ */
