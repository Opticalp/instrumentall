/**
 * @file	src/dataProxies/ImageReticle.h
 * @date	May 2018
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

#ifndef SRC_DATAPROXIES_IMAGERETICLE_H_
#define SRC_DATAPROXIES_IMAGERETICLE_H_

#ifdef HAVE_OPENCV

#include "core/DataProxy.h"

/**
 * ImageReticle
 *
 * Add a cross reticle on the input image
 */
class ImageReticle: public DataProxy
{
public:
	ImageReticle();
	virtual ~ImageReticle() { }

    std::string description() { return classDescription(); }

    static std::string classDescription()
        { return "Display a cross reticle on the forwarded image. "; }

    std::set<int> supportedInputDataType();
    std::set<int> supportedOutputDataType();

private:
	static size_t refCount;

	/**
     * Main logic
     */
    void convert();

    enum params
    {
        paramXpos,
        paramYpos,
		paramReticleSize,
		paramBlankRatio,
        paramXwidth,
        paramYwidth,
		paramAngle,
		paramAlter,
		paramGreyLevel,
        paramCnt
    };

    int xPos, yPos, xWidth, yWidth, reticleSize;
    unsigned char greyLevel;
    bool alter;
    double angle, blankRatio;

    Poco::Int64 getIntParameterValue(size_t paramIndex);
    void setIntParameterValue(size_t paramIndex, Poco::Int64 value);

    double getFloatParameterValue(size_t paramIndex);
    void setFloatParameterValue(size_t paramIndex, double value);

    std::string getStrParameterValue(size_t paramIndex);
    void setStrParameterValue(size_t paramIndex, std::string value);

    /**
     * Utility function
     */
    cv::Point pt2fToPt(cv::Point2f srcPt);
};
#endif /* HAVE_OPENCV */
#endif /* SRC_DATAPROXIES_IMAGERETICLE_H_ */
