/**
 * @file	src/dataProxies/ImageScharr.h
 * @date	Jan 2019
 * @author	PhRG - opticalp.fr
 */

/*
 Copyright (c) 2019 Ph. Renaud-Goud / Opticalp

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

#ifndef SRC_DATAPROXIES_IMAGESCHARR_H_
#define SRC_DATAPROXIES_IMAGESCHARR_H_

#ifdef HAVE_OPENCV

#include "core/DataProxy.h"

/**
 * ImageScharr
 *
 * Apply simple scharr operator to an image
 */
class ImageScharr: public DataProxy
{
public:
	ImageScharr();
	virtual ~ImageScharr() { }

    std::string description() { return classDescription(); }

    static std::string classDescription()
        { return "Apply a Scharr operator to the input image image. "; }

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
		paramAlter,
        paramCnt
    };

    bool alter;

//    Poco::Int64 getIntParameterValue(size_t paramIndex);
//    void setIntParameterValue(size_t paramIndex, Poco::Int64 value);
//    double getFloatParameterValue(size_t paramIndex);
//    void setFloatParameterValue(size_t paramIndex, double value);

    std::string getStrParameterValue(size_t paramIndex);
    void setStrParameterValue(size_t paramIndex, std::string value);
};
#endif /* HAVE_OPENCV */
#endif /* SRC_DATAPROXIES_IMAGESCHARR_H_ */
