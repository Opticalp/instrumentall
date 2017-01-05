/**
 * @file	src/dataLoggers/ShowImageLogger.cpp
 * @date	Apr. 2016
 * @author	PhRG - opticalp.fr
 */

/*
 Copyright (c) 2016 Ph. Renaud-Goud / Opticalp

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

#include "ShowImageLogger.h"

#include "core/DataItem.h"

#include "Poco/NumberFormatter.h"

#include "opencv2/opencv.hpp"

#if CV_MAJOR_VERSION == 3
#include <opencv2/highgui/highgui.hpp>
#endif


size_t ShowImageLogger::refCount = 0;

ShowImageLogger::ShowImageLogger()
{
    mName = "ShowImageLogger";
    if (refCount)
        mName += Poco::NumberFormatter::format(refCount);

    refCount++;
}

void ShowImageLogger::log()
{
    cv::Mat img = *(getDataSource()->getData<cv::Mat>());

    if (img.data)
    {
        // create a window for display.
        cv::namedWindow( name(), CV_WINDOW_AUTOSIZE );
        // Show our image inside it.
        cv::imshow( name(), img );

        // take time to effectively display the image
        cv::waitKey(100);
    }
    else
    {
        throw Poco::RuntimeException(name(),"empty image, nothing to show");
    }
}

bool ShowImageLogger::isSupportedInputDataType(int datatype)
{
    return (datatype == (DataItem::typeCvMat | DataItem::contScalar));
}

std::set<int> ShowImageLogger::supportedInputDataType()
{
    std::set<int> ret;
    ret.insert(DataItem::typeCvMat);
    return ret;
}

#endif /* HAVE_OPENCV */
