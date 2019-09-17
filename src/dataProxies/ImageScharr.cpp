/**
 * @file	src/dataProxies/ImageScharr.cpp
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

#include "ImageScharr.h"

#include "Poco/Timespan.h"
#include "Poco/NumberFormatter.h"

#ifdef HAVE_OPENCV

size_t ImageScharr::refCount = 0;

ImageScharr::ImageScharr(): DataProxy("ImageScharr")
{
	setName(refCount);
    refCount++;

    setParameterCount(paramCnt);
    addParameter(paramAlter, "alter", "alter the input image (\"yes\") "
    		"or duplicate the image before drawing (\"no\")", ParamItem::typeString, "no");

    setStrParameterValue(paramAlter, getStrParameterDefaultValue(paramAlter));
}

std::set<int> ImageScharr::supportedInputDataType()
{
	std::set<int> list;
	list.insert(DataItem::typeCvMat);
	return list;
}

std::set<int> ImageScharr::supportedOutputDataType()
{
	std::set<int> list;
	list.insert(DataItem::typeCvMat);
	return list;
}

void ImageScharr::convert()
{
	cv::Mat workingImg;

    poco_information(logger(),"Scharr: time is (to)");
    Poco::Timestamp now;

    if (alter)
		workingImg = *getDataSource()->getData<cv::Mat>();
	else
		workingImg = (*getDataSource()->getData<cv::Mat>()).clone();

	if (!workingImg.data)
	{
		poco_warning(logger(), "empty image, nothing to do");
		*getData<cv::Mat>() = workingImg;
		return;
	}

    if (workingImg.type()!=CV_8U)
    {
        double min,max;
        cv::minMaxLoc(workingImg,&min,&max);

        cv::Mat tmpImg; // temporary image
        workingImg.convertTo(
                tmpImg,      // output image
                CV_8U,       // depth
                255.0/max ); // scale factor

        workingImg = tmpImg;
    }

    poco_information(logger(), "Image prepared for Scharr; time is (to) + "
        + Poco::NumberFormatter::format(now.elapsed()/1000));

    // Generate gradX and gradY
    cv::Mat gradX, gradY;
    cv::Mat mag;

    // Gradient X
    cv::Sobel( workingImg, gradX, CV_32F, 1, 0, CV_SCHARR);
    // Gradient Y
    cv::Sobel( workingImg, gradY, CV_32F, 0, 1, CV_SCHARR);

    poco_information(logger(), "Scharr done; time is (to) + "
        + Poco::NumberFormatter::format(now.elapsed()/1000));

    cv::magnitude(gradX, gradY, mag);
    poco_information(logger(), "Magnitude done; time is (to) + "
        + Poco::NumberFormatter::format(now.elapsed()/1000));

    // convert back to CV_8U
    double min,max;
    cv::minMaxLoc(mag, &min, &max);

    poco_information(logger(),
            Poco::format("Scharr. Min is: %f, max is: %f", min, max));

    mag.convertTo(
            workingImg,  // output image
            CV_8U,       // depth
            1.0/32 );    // scale factor (adapted for scharr)

    poco_information(logger(), "derivative done; time is (to) + "
        + Poco::NumberFormatter::format(now.elapsed()/1000));

    *getData<cv::Mat>() = workingImg;
}

//Poco::Int64 ImageScharr::getIntParameterValue(size_t paramIndex)
//{
//	switch(paramIndex)
//	{
//	default:
//		poco_bugcheck_msg("wrong parameter index");
//		throw Poco::BugcheckException();
//	}
//}
//
//void ImageScharr::setIntParameterValue(size_t paramIndex, Poco::Int64 value)
//{
//	switch(paramIndex)
//	{
//	default:
//		poco_bugcheck_msg("wrong parameter index");
//		throw Poco::BugcheckException();
//	}
//}
//
//double ImageScharr::getFloatParameterValue(size_t paramIndex)
//{
//	switch (paramIndex)
//	{
//	default:
//		poco_bugcheck_msg("wrong parameter index");
//		throw Poco::BugcheckException();
//	}
//}
//
//void ImageScharr::setFloatParameterValue(size_t paramIndex, double value)
//{
//	switch (paramIndex)
//	{
//	default:
//		poco_bugcheck_msg("wrong parameter index");
//		throw Poco::BugcheckException();
//	}
//}

std::string ImageScharr::getStrParameterValue(size_t paramIndex)
{
	poco_assert(paramIndex == paramAlter);
	if (alter)
		return "yes";
	else
		return "no";
}

#include "Poco/String.h"

void ImageScharr::setStrParameterValue(size_t paramIndex, std::string value)
{
	poco_assert(paramIndex == paramAlter);

	if (Poco::icompare(value, "yes") == 0)
		alter = true;
	else if (Poco::icompare(value, "no") == 0)
		alter = false;
	else
		Poco::InvalidArgumentException("\"alter\" parameter value has to be: \"yes\" or \"no\"");
}

#endif /* HAVE_OPENCV */
