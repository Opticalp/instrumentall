/**
 * @file	src/dataProxies/ImageReticle.cpp
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

#include "ImageReticle.h"

#ifdef HAVE_OPENCV

size_t ImageReticle::refCount = 0;

ImageReticle::ImageReticle(): DataProxy("ImageReticle")
{
	setName(refCount);
    refCount++;

    setParameterCount(paramCnt);
    addParameter(paramXpos, "xPos", "reticle x position (zero based)", ParamItem::typeInteger,"400");
    addParameter(paramYpos, "yPos", "reticle y position (zero based)", ParamItem::typeInteger,"300");
    addParameter(paramReticleSize, "reticleSize", "reticle size", ParamItem::typeInteger,"20");
    addParameter(paramXwidth, "xWidth", "reticle x width", ParamItem::typeInteger, "0");
    addParameter(paramYwidth, "yWidth", "reticle y width", ParamItem::typeInteger, "0");
    addParameter(paramAngle, "angle", "reticle rotation (degrees)", ParamItem::typeFloat, "0.0");
    addParameter(paramAlter, "alter", "alter the input image (\"yes\") "
    		"or duplicate the image before drawing (\"no\")", ParamItem::typeString, "yes");
    addParameter(paramGreyLevel, "greyLevel", "cross grey level", ParamItem::typeInteger, "127");

    setIntParameterValue(paramXpos, getIntParameterDefaultValue(paramXpos));
    setIntParameterValue(paramYpos, getIntParameterDefaultValue(paramYpos));
    setIntParameterValue(paramReticleSize, getIntParameterDefaultValue(paramReticleSize));
    setIntParameterValue(paramXwidth, getIntParameterDefaultValue(paramXwidth));
    setIntParameterValue(paramYwidth, getIntParameterDefaultValue(paramYwidth));
    setFloatParameterValue(paramAngle, getFloatParameterDefaultValue(paramAngle));
    setStrParameterValue(paramAlter, getStrParameterDefaultValue(paramAlter));
    setIntParameterValue(paramGreyLevel, getIntParameterDefaultValue(paramGreyLevel));
}

std::set<int> ImageReticle::supportedInputDataType()
{
	std::set<int> list;
	list.insert(DataItem::typeCvMat);
	return list;
}

std::set<int> ImageReticle::supportedOutputDataType()
{
	std::set<int> list;
	list.insert(DataItem::typeCvMat);
	return list;
}

void ImageReticle::convert()
{
	cv::Mat workingImg;

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

    // draw the reticle.
    cv::RotatedRect overallRect(	cv::Point2f(xPos, yPos),
    								cv::Size2f(reticleSize*2 + xWidth, reticleSize*2 + yWidth),
									angle	);

    cv::Point2f overallPoints[4];
    overallRect.points(overallPoints);

    cv::line(workingImg,
    		pt2fToPt(0.5*(overallPoints[0] + overallPoints[1])),
			pt2fToPt(0.5*(overallPoints[2] + overallPoints[3])),
			CV_RGB(greyLevel, greyLevel, greyLevel));

    cv::line(workingImg,
    		pt2fToPt(0.5*(overallPoints[1] + overallPoints[2])),
			pt2fToPt(0.5*(overallPoints[3] + overallPoints[0])),
			CV_RGB(greyLevel, greyLevel, greyLevel));

    if (xWidth)
    {
        cv::RotatedRect zoneRectX(  cv::Point2f(xPos, yPos),
                                    cv::Size2f(xWidth, reticleSize),
                                    angle   );

        cv::Point2f zonePointsX[4];
        zoneRectX.points(zonePointsX);

        cv::line(workingImg,
                pt2fToPt(zonePointsX[0]),
                pt2fToPt(zonePointsX[1]),
                CV_RGB(greyLevel, greyLevel, greyLevel));

        cv::line(workingImg,
                pt2fToPt(zonePointsX[2]),
                pt2fToPt(zonePointsX[3]),
                CV_RGB(greyLevel, greyLevel, greyLevel));
    }

    if (yWidth)
    {
        cv::RotatedRect zoneRectY(  cv::Point2f(xPos, yPos),
                                    cv::Size2f(reticleSize, yWidth),
                                    angle   );

        cv::Point2f zonePointsY[4];
        zoneRectY.points(zonePointsY);

        cv::line(workingImg,
                pt2fToPt(zonePointsY[1]),
                pt2fToPt(zonePointsY[2]),
                CV_RGB(greyLevel, greyLevel, greyLevel));

        cv::line(workingImg,
                pt2fToPt(zonePointsY[0]),
                pt2fToPt(zonePointsY[3]),
                CV_RGB(greyLevel, greyLevel, greyLevel));
    }

    *getData<cv::Mat>() = workingImg;
}

Poco::Int64 ImageReticle::getIntParameterValue(size_t paramIndex)
{
	switch(paramIndex)
	{
	case paramXpos:
		return xPos;
	case paramYpos:
		return yPos;
	case paramReticleSize:
		return reticleSize;
	case paramXwidth:
		return xWidth;
	case paramYwidth:
		return yWidth;
	case paramGreyLevel:
		return greyLevel;
	default:
		poco_bugcheck_msg("wrong parameter index");
		throw Poco::BugcheckException();
	}
}

void ImageReticle::setIntParameterValue(size_t paramIndex, Poco::Int64 value)
{
	switch(paramIndex)
	{
	case paramXpos:
		if (value < 0)
			throw Poco::RangeException("xPos must be positive or null");
		xPos = value;
		break;
	case paramYpos:
		if (value < 0)
			throw Poco::RangeException("yPos must be positive or null");
		yPos = value;
		break;
	case paramReticleSize:
		if (value <= 0)
			throw Poco::RangeException("reticleSize must be strictly positive ");
		reticleSize = value;
		break;
	case paramXwidth:
		if (value < 0)
			throw Poco::RangeException("xSize must be positive or null");
		xWidth = value;
		break;
	case paramYwidth:
		if (value < 0)
			throw Poco::RangeException("ySize must be positive or null");
		yWidth = value;
		break;
	case paramGreyLevel:
		if (value < 0 || value > 255)
			throw Poco::RangeException("grey level must be in the range [0-255]");
		greyLevel = value;
		break;
	default:
		poco_bugcheck_msg("wrong parameter index");
		throw Poco::BugcheckException();
	}
}

double ImageReticle::getFloatParameterValue(size_t paramIndex)
{
	poco_assert(paramIndex == paramAngle);
	return angle;
}

void ImageReticle::setFloatParameterValue(size_t paramIndex, double value)
{
	poco_assert(paramIndex == paramAngle);
	angle = value;
}

std::string ImageReticle::getStrParameterValue(size_t paramIndex)
{
	poco_assert(paramIndex = paramAlter);
	if (alter)
		return "yes";
	else
		return "no";
}

#include "Poco/String.h"

void ImageReticle::setStrParameterValue(size_t paramIndex, std::string value)
{
	poco_assert(paramIndex = paramAlter);

	if (Poco::icompare(value, "yes") == 0)
		alter = true;
	else if (Poco::icompare(value, "no") == 0)
		alter = false;
	else
		Poco::InvalidArgumentException("\"alter\" parameter value has to be: \"yes\" or \"no\"");
}

cv::Point ImageReticle::pt2fToPt(cv::Point2f srcPt)
{
	return cv::Point(cvRound(srcPt.x),cvRound(srcPt.y));
}

#endif /* HAVE_OPENCV */
