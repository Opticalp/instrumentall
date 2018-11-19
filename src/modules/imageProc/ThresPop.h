/**
 * @file	src/modules/imageProc/ThresPop.h
 * @date	Jan. 2017
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

#ifndef SRC_MODULES_IMAGEPROC_THRESPOP_H_
#define SRC_MODULES_IMAGEPROC_THRESPOP_H_

#ifdef HAVE_OPENCV
#include "core/Module.h"

#include "Thresholder.h"

#include "Poco/NumberFormatter.h"

/**
 * Threshold considering a population ratio, with or without mask
 *
 * @notice The onValue parameter behavior is not the same with or without mask.
 */
class ThresPop: public Module, private Thresholder
{
public:
    ThresPop(ModuleFactory* parent, std::string customName);

    std::string description()
    {
        return "Image thresholding module. "
                "Population relative threshold value. "
                "Works on grayscale images. ";
    }

private:
    /**
     * Main logic
     */
    void process(int startCond);

    Poco::Int64 getIntParameterValue(size_t paramIndex);
    void setIntParameterValue(size_t paramIndex, Poco::Int64 value);

    double getFloatParameterValue(size_t paramIndex);
    void setFloatParameterValue(size_t paramIndex, double value);

    std::string getStrParameterValue(size_t paramIndex);
    void setStrParameterValue(size_t paramIndex, std::string value);

    Poco::Logger& logger() { return Module::logger(); }

    static size_t refCount; ///< reference counter to generate a unique internal name

    enum params
    {
        paramThresholdValue,
        paramLowHigh,
        paramOnValue,
        paramCnt
    };

    double threshold;

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
        imageOutPort,
        medianPort,
        outPortCnt
    };

    /**
     * compute the threshold value, given the input image
     */
    double thresholdValue(cv::Mat* pImg, cv::Mat* pMask, double& median);

	void computeHistogram(cv::Mat imgIn, cv::Mat mask);
    int histogram[4096]; ///< only 12 relevant bits, even for 16-bit images. We could have use a map here as a sparse vector.

    double getPopulationValue(size_t count);
};

#endif /* HAVE_OPENCV */
#endif /* SRC_MODULES_IMAGEPROC_THRESPOP_H_ */
