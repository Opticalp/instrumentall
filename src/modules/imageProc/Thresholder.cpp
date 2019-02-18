/**
 * @file	src/modules/imageProc/Thresholder.cpp
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

#ifdef HAVE_OPENCV

#include "Thresholder.h"

cv::Mat Thresholder::doThreshold(cv::Mat* pImg, cv::Mat* pMask,
		double thresVal, size_t& cnt, size_t& totCnt)
{
	if (pMask == NULL)
	{
	    switch (pImg->type())
	    {
	    case CV_8U:
			return thresNoMask<unsigned char>(pImg, thresVal, cnt, totCnt);
	    case CV_16U:
	    	return thresNoMask<unsigned short>(pImg, thresVal, cnt, totCnt);
	    case CV_32F:
			return thresNoMask<float>(pImg, thresVal, cnt, totCnt);
	    case CV_64F:
			return thresNoMask<double>(pImg, thresVal, cnt, totCnt);
	    default:
	        throw Poco::DataFormatException("thresholdNoMask",
	                "Input image pixel format has to be 8- or 16-bit integer, 32- or 64-bit float");
	    }
	}

    if (pImg->total() != pMask->total())
        throw Poco::RuntimeException("thresholdWithMask",
                "image size and mask size do not fit");

    if (pMask->type() != CV_8U)
        throw Poco::DataFormatException("thresholdWithMask",
                "image mask pixel format has to be 8-bit integer");

    poco_information(logger(), "onValue is " + Poco::NumberFormatter::format(onValue));

    switch (pImg->type())
    {
    case CV_8U:
        if (onValue == -1)
            return thresWMaskVal<unsigned char>(pImg, pMask, thresVal, cnt, totCnt);
        else
        	return thresWMask<unsigned char>(pImg, pMask, thresVal, cnt, totCnt);
    case CV_16U:
        if (onValue == -1)
            return thresWMaskVal<unsigned short>(pImg, pMask, thresVal, cnt, totCnt);
        else
        	return thresWMask<unsigned short>(pImg, pMask, thresVal, cnt, totCnt);
    case CV_32F:
		if (onValue == -1)
			return thresWMaskVal<float>(pImg, pMask, thresVal, cnt, totCnt);
		else
			return thresWMask<float>(pImg, pMask, thresVal, cnt, totCnt);
    case CV_64F:
        if (onValue == -1)
            return thresWMaskVal<double>(pImg, pMask, thresVal, cnt, totCnt);
        else
        	return thresWMask<double>(pImg, pMask, thresVal, cnt, totCnt);
    default:
        throw Poco::DataFormatException("thresholdWithMask",
                "Input image pixel format has to be 8- or 16-bit integer, 32- or 64-bit float");
    }
}

#endif /* HAVE_OPENCV */
