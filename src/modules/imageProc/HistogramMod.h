/**
 * @file	src/modules/imageProc/HistogramMod.h
 * @date	Jun 2018
 * @author	PhRG - opticalp.fr
 */

/*
 Copyright (c) 2018 Ph. Renaud-Goud / Opticalp

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

#ifndef SRC_MODULES_IMAGEPROC_HISTOGRAM_H_
#define SRC_MODULES_IMAGEPROC_HISTOGRAM_H_

#ifdef HAVE_OPENCV

#include "core/Module.h"
#include "Poco/NumberFormatter.h"

/**
 * @class HistogramMod
 * @file HistogramMod.h
 * @brief Compute the histogram af a uChar image
 */
class HistogramMod: public Module
{
public:
    HistogramMod(ModuleFactory* parent, std::string customName);
    
    std::string description()
    {
        return "Compute the histogram of an image. \n"
            "Outputs a 4096-element array with normalized population for each grey level \n"
            "(usually 256 grey-levels for a 8-bit image). ";
    }
  
private: 
    static size_t refCount; ///< reference counter to generate a unique internal name
  
    /**
     * Main logic
     */
    void process(int startCond);

    /// Indexes of the input ports
    enum inPorts
    {
        imageInPort,
		maskInPort,
        inPortCnt
    };

    /// Indexes of the output ports
    enum outPorts
    {
        histogramPort,
        outPortCnt
    };

//    enum params
//    {
//        paramThresholdValue,
//        paramLowHigh,
//        paramOnValue,
//        paramCnt
//    };

 	void computeHistogram(cv::Mat imgIn, cv::Mat mask);
    float histogram[4096]; ///< only 12 relevant bits, even for 16-bit images. We could have use a map here as a sparse vector.
};

#endif /* HAVE_OPENCV */
#endif /* SRC_MODULES_IMAGEPROC_HISTOGRAM_H_ */
