/**
 * @file	src/modules/imageProc/Thresholder.h
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

#ifndef SRC_MODULES_IMAGEPROC_THRESHOLDER_H_
#define SRC_MODULES_IMAGEPROC_THRESHOLDER_H_

#ifdef HAVE_OPENCV

#include "Poco/Logger.h"
#include "Poco/NumberFormatter.h"
#include <opencv2/core/core.hpp>


/**
 * Thresholder
 *
 * implement common methods for all thresholding modules
 */
class Thresholder
{
public:
    cv::Mat doThreshold(cv::Mat* pImg, cv::Mat* pMask, double thresVal);

    Poco::Int64 onValue;
    bool high;

private:
    template <typename T>
    cv::Mat thresWMask(cv::Mat* pImg, cv::Mat* pMask, double thresVal);
    template <typename T>
    cv::Mat thresWMaskVal(cv::Mat* pImg, cv::Mat* pMask, double thresVal);
    template <typename T>
    cv::Mat thresNoMask(cv::Mat* pImg, double thresVal);

    virtual Poco::Logger& logger() = 0;
};

template<typename T>
inline cv::Mat Thresholder::thresWMask(cv::Mat* pImg, cv::Mat* pMask,
		double thresVal)
{
    cv::Mat out(pImg->rows, pImg->cols, CV_8U, cv::Scalar(0,0,0,0));

    size_t count=0;

    int cols = pImg->cols, rows = pImg->rows;
    if(pImg->isContinuous() && pMask->isContinuous() && out.isContinuous())
    {
        cols *= rows;
        rows = 1;
    }

    unsigned char onVal = static_cast<unsigned char>(onValue);

    T thresValue = static_cast<T>(thresVal);

    if (high)
    {
        for(int i = 0; i < rows; i++)
        {
            const T* inI = pImg->ptr<T>(i);
            const unsigned char* maI = pMask->ptr<unsigned char>(i);
            unsigned char* outI = out.ptr<unsigned char>(i);
            for(int j = 0; j < cols; j++)
                if (maI[j] && inI[j] >= thresValue)
                {
                    outI[j] = onVal;
                    count++;
                }
        }
    }
    else
    {
        for(int i = 0; i < rows; i++)
        {
            const T* inI = pImg->ptr<T>(i);
            const unsigned char* maI = pMask->ptr<unsigned char>(i);
            unsigned char* outI = out.ptr<unsigned char>(i);
            for(int j = 0; j < cols; j++)
                if (maI[j] && inI[j] <= thresValue)
                {
                    outI[j] = onVal;
                    count++;
                }
        }
    }

    poco_information(logger(),
            Poco::NumberFormatter::format(count)
            + " pixels are thresholded. ");

    return out;
}

template<typename T>
inline cv::Mat Thresholder::thresWMaskVal(cv::Mat* pImg, cv::Mat* pMask,
		double thresVal)
{
    poco_information(logger(), "onValue is -1, using mask values");

    cv::Mat out(pImg->rows, pImg->cols, CV_8U, cv::Scalar(0,0,0,0));

    size_t count=0;

    int cols = pImg->cols, rows = pImg->rows;
    if(pImg->isContinuous() && pMask->isContinuous() && out.isContinuous())
    {
        cols *= rows;
        rows = 1;
    }

    unsigned char onVal = static_cast<unsigned char>(onValue);

    T thresValue = static_cast<T>(thresVal);

    if (high)
    {
        for(int i = 0; i < rows; i++)
        {
            const T* inI = pImg->ptr<T>(i);
            const unsigned char* maI = pMask->ptr<unsigned char>(i);
            unsigned char* outI = out.ptr<unsigned char>(i);
            for(int j = 0; j < cols; j++)
                if (maI[j] && inI[j] >= thresValue)
                {
                    outI[j] = maI[j];
                    count++;
                }
        }
    }
    else
    {
        for(int i = 0; i < rows; i++)
        {
            const T* inI = pImg->ptr<T>(i);
            const unsigned char* maI = pMask->ptr<unsigned char>(i);
            unsigned char* outI = out.ptr<unsigned char>(i);
            for(int j = 0; j < cols; j++)
                if (maI[j] && inI[j] <= thresValue)
                {
                    outI[j] = maI[j];
                    count++;
                }
        }
    }

    poco_information(logger(),
            Poco::NumberFormatter::format(count)
            + " pixels are thresholded. ");

    return out;
}

template<typename T>
inline cv::Mat Thresholder::thresNoMask(cv::Mat* pImg, double thresVal)
{
    cv::Mat out(pImg->rows, pImg->cols, CV_8U, cv::Scalar(0,0,0,0));

    size_t count=0;

    int cols = pImg->cols, rows = pImg->rows;
    if(pImg->isContinuous() && out.isContinuous())
    {
        cols *= rows;
        rows = 1;
    }

    unsigned char onVal = static_cast<unsigned char>(onValue);

	T thresValue = static_cast<T>(thresVal);

	if (high)
	{
		for(int i = 0; i < rows; i++)
		{
			const T* inI = pImg->ptr<T>(i);
			unsigned char* outI = out.ptr<unsigned char>(i);

			for(int j = 0; j < cols; j++)
				if (inI[j] >= thresValue)
				{
					outI[j] = onVal;
					count++;
				}
		}
	}
	else
	{
		for(int i = 0; i < rows; i++)
		{
			const T* inI = pImg->ptr<T>(i);
			unsigned char* outI = out.ptr<unsigned char>(i);

			for(int j = 0; j < cols; j++)
				if (inI[j] <= thresValue)
				{
					outI[j] = onVal;
					count++;
				}
		}
	}

    poco_information(logger(),
            Poco::NumberFormatter::format(count)
            + " pixels are thresholded. ");

    return out;
}

#endif /* HAVE_OPENCV */
#endif /* SRC_MODULES_IMAGEPROC_THRESHOLDER_H_ */
