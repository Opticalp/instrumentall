/**
 * @file    src/Modules/imageProc/DistToOrigin.cpp
 * @date    Aug 2018
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

#ifdef HAVE_OPENCV

#include "DistToOrigin.h"

#include <opencv2/imgproc/imgproc.hpp>

#include "Poco/NumberFormatter.h"
#include "Poco/String.h" // icompare

size_t DistToOrigin::refCount = 0;

DistToOrigin::DistToOrigin(ModuleFactory* parent, std::string customName):
            Module(parent, customName)
{
    if (refCount)
         setInternalName("DistToOrigin" + Poco::NumberFormatter::format(refCount));
     else
         setInternalName("DistToOrigin");

     setCustomName(customName);
     setLogger("module." + name());

     // parameters
     setParameterCount(paramCnt);
     addParameter(paramXpos, "xPos",
             "x-coord origin/reference position. -1 means: image center. ",
             ParamItem::typeInteger, "-1");
     addParameter(paramYpos, "yPos",
             "y-coord origin/reference position. -1 means: image center. ",
             ParamItem::typeInteger, "-1");
     addParameter(paramRatio, "ratio",
             "Amplification ratio of the centering error",
             ParamItem::typeFloat, "1.0");
     addParameter(paramColorMode, "colorMode",
             "One of: gray, red, green, blue. \n"
             "If a color is given, the output image will be in color",
             ParamItem::typeString, "gray");
     addParameter(paramColorLevel, "level",
             "Color level (0 is black, 255 is light",
             ParamItem::typeInteger, "255");

     setParametersDefaultValue();

     // ports
     setInPortCount(inPortCnt);
     setOutPortCount(outPortCnt);

     addInPort("image", "image to be annotated", DataItem::typeCvMat, imageInPort);
     addInPort("deltaX", "Distance to the origin (x)", DataItem::typeFloat, deltaXPort);
     addInPort("deltaY", "Distance to the origin (y)", DataItem::typeFloat, deltaYPort);

     addOutPort("image", "Image with added vector", DataItem::typeCvMat, imageOutPort);

     setParametersDefaultValue();

     notifyCreation();

     // if nothing failed
     refCount++;
}

Poco::Int64 DistToOrigin::getIntParameterValue(size_t paramIndex)
{
    switch (paramIndex)
    {
    case paramXpos:
        return xPos;
    case paramYpos:
        return yPos;
    case paramColorLevel:
        return colorLevel;
    default:
        poco_bugcheck_msg("unknown parameter index");
        throw Poco::BugcheckException();
    }
}

void DistToOrigin::setIntParameterValue(size_t paramIndex, Poco::Int64 value)
{
    switch (paramIndex)
    {
    case paramXpos:
        xPos = static_cast<int>(value);
        break;
    case paramYpos:
        yPos = static_cast<int>(value);
        break;
    case paramColorLevel:
        if (value < 0 || value > 255)
            throw Poco::RangeException("setParameterValue",
                    "colorLevel shall be in [0 255]");
        colorLevel = static_cast<int>(value);
        break;
    default:
        poco_bugcheck_msg("unknown parameter index");
    }
}

std::string DistToOrigin::getStrParameterValue(size_t paramIndex)
{
    switch (paramIndex)
    {
    case paramColorMode:
        switch (color)
        {
        case colorGray:
            return "gray";
        case colorRed:
            return "red";
        case colorGreen:
            return "green";
        case colorBlue:
            return "blue";
        default:
            poco_bugcheck_msg("unknown color mode");
            throw Poco::BugcheckException();
        }
        break;
    default:
        poco_bugcheck_msg("unknown parameter index");
        throw Poco::BugcheckException();
    }
}

void DistToOrigin::setStrParameterValue(size_t paramIndex, std::string value)
{
    switch (paramIndex)
    {
    case paramColorMode:
        if (Poco::icompare(value, "gray") == 0)
            color = colorGray;
        else if (Poco::icompare(value, "red") == 0)
            color = colorRed;
        else if (Poco::icompare(value, "green") == 0)
            color = colorGreen;
        else if (Poco::icompare(value, "blue") == 0)
            color = colorBlue;
        else
            throw Poco::InvalidArgumentException("setParameterValue",
                    "colorMode shall be one of: gray, red, green, blue");
        break;
    default:
        poco_bugcheck_msg("unknown parameter index");
    }
}

double DistToOrigin::getFloatParameterValue(size_t paramIndex)
{
	switch (paramIndex)
	{
	case paramRatio:
		return ratio;
	default:
		poco_bugcheck_msg("wrong parameter index");
		throw Poco::BugcheckException();
	}
}

void DistToOrigin::setFloatParameterValue(size_t paramIndex, double value)
{
	switch (paramIndex)
	{
	case paramRatio:
		ratio = value;
		break;
	default:
		poco_bugcheck_msg("wrong parameter index");
		throw Poco::BugcheckException();
	}
}

void DistToOrigin::process(int startCond)
{
    if (startCond != allDataStartState)
    {
        poco_error(logger(), name() + ": no input data. Exiting. ");
        return;
    }

	float x, y, dX, dY;
	float* pfData;

	readLockInPort(deltaXPort);
    readInPortData<float>(deltaXPort, pfData);
	dX = *pfData;
	releaseInPort(deltaXPort);

	readLockInPort(deltaYPort);
	readInPortData<float>(deltaYPort, pfData);
	dY = *pfData;
	releaseInPort(deltaYPort);

	DataAttributeIn attr;
	cv::Mat* pData;

	readLockInPort(imageInPort);
	readInPortData<cv::Mat>(imageInPort, pData);
	readInPortDataAttribute(imageInPort, &attr);

	DataAttributeOut outAttr = attr;

    cv::Mat workingImg;

    if ((color != colorGray) && pData->channels() < 3)
        cv::cvtColor(*pData, workingImg, cv::COLOR_GRAY2BGR);
    else
        pData->copyTo(workingImg);

    releaseInPort(imageInPort);
    reserveOutPort(imageOutPort);

	if (xPos != -1)
		x = xPos;
	else
		x = workingImg.cols / 2;

	if (yPos != -1)
		y = yPos;
	else
		y = workingImg.rows / 2;

    cv::Scalar fillCol;

    switch (color)
    {
    case colorGray:
        fillCol = CV_RGB(colorLevel,colorLevel,colorLevel);
        break;
    case colorRed:
        fillCol = CV_RGB(colorLevel,0,0);
        break;
    case colorGreen:
        fillCol = CV_RGB(0,colorLevel,0);
        break;
    case colorBlue:
        fillCol = CV_RGB(0,0,colorLevel);
        break;
    }

	cv::line(workingImg,
		cv::Point(x, y),
		cv::Point(x + cvRound(dX * ratio), y + cvRound(dY * ratio)),
		fillCol);

    processingTerminated();

    getDataToWrite<cv::Mat>(imageOutPort, pData);

    *pData = workingImg;

    notifyOutPortReady(imageOutPort, outAttr);
}

#endif /* HAVE_OPENCV */
