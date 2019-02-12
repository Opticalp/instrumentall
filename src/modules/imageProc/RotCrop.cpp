/**
 * @file	src/modules/imageProc/RotCrop.cpp
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

#include "RotCrop.h"

#include <opencv2/imgproc/imgproc.hpp>

#include "Poco/NumberFormatter.h"
#include <math.h>

size_t RotCrop::refCount = 0;

RotCrop::RotCrop(ModuleFactory* parent, std::string customName):
    Module(parent, customName)
{
    if (refCount)
        setInternalName("RotCrop" + Poco::NumberFormatter::format(refCount));
    else
        setInternalName("RotCrop");

    setCustomName(customName);
    setLogger("module." + name());

    // parameters
    setParameterCount(paramCnt);
    addParameter(paramXCenter, "xCenter",
            "x-coord of the center of the rectangle to be rotated + cropped. "
            "\"-1\" means: image center",
            ParamItem::typeInteger, "-1");
    addParameter(paramYCenter, "yCenter",
            "y-coord of the center of the rectangle to be rotated + cropped. "
            "\"-1\" means: image center",
            ParamItem::typeInteger, "-1");
    addParameter(paramHeight, "height",
            "height of the rectangle to be cropped. ",
            ParamItem::typeInteger, "100");
    addParameter(paramWidth, "width",
            "width of the rectangle to be cropped. ",
            ParamItem::typeInteger, "100");
    addParameter(paramAngle, "angle",
            "Tilt angle (degrees) of the rectangle. ",
            ParamItem::typeFloat, "0");

    setParametersDefaultValue();

    // ports
    setInPortCount(inPortCnt);
    setOutPortCount(outPortCnt);

    addInPort("image", "image to be rotated + cropped", DataItem::typeCvMat, imageInPort);

    addOutPort("image", "cropped and rotated image", DataItem::typeCvMat, imageOutPort);

    notifyCreation();

    // if nothing failed
    refCount++;
}

Poco::Int64 RotCrop::getIntParameterValue(size_t paramIndex)
{
    switch (paramIndex)
    {
    case paramXCenter:
        return xCenter;
    case paramYCenter:
        return yCenter;
    case paramWidth:
        return width;
    case paramHeight:
        return height;
    default:
        poco_bugcheck_msg("impossible param index in getIntParameterValue");
        throw Poco::BugcheckException();
    }
}

void RotCrop::setIntParameterValue(size_t paramIndex, Poco::Int64 value)
{
    switch (paramIndex)
    {
    case paramXCenter:
        if (value < -1)
            throw Poco::RangeException("setParameterValue",
                    "xCenter has to be >= -1");
        xCenter = value;
        break;
    case paramYCenter:
        if (value < -1)
            throw Poco::RangeException("setParameterValue",
                    "yCenter has to be >= -1");
        yCenter = value;
        break;
    case paramWidth:
        if (value <= 0)
            throw Poco::RangeException("setParameterValue",
                    "width has to be > 0");
        width = value;
        break;
    case paramHeight:
        if (value <= 0)
            throw Poco::RangeException("setParameterValue",
                    "height has to be > 0");
        height = value;
        break;
    default:
        poco_bugcheck_msg("impossible param index in getIntParameterValue");
        throw Poco::BugcheckException();
    }
}

double RotCrop::getFloatParameterValue(size_t paramIndex)
{
    poco_assert(paramIndex == paramAngle);

    return angle;
}

void RotCrop::setFloatParameterValue(size_t paramIndex, double value)
{
    poco_assert(paramIndex == paramAngle);

    angle = value;
}

#define PI 3.14159265

#include "Poco/Format.h"

void RotCrop::process(int startCond)
{
    // TODO: special case if angle == 0?

    if (startCond != allDataStartState)
    {
        poco_information(logger(), name() + ": no input data. Exiting. ");
        return;
    }

    DataAttributeIn attr;
    cv::Mat* pData;

    readLockInPort(imageInPort);
    readInPortData<cv::Mat>(imageInPort, pData);
    readInPortDataAttribute(imageInPort, &attr);

    DataAttributeOut outAttr = attr;

    cv::Mat workingImg;

    // -angle is taken because angle = tilt of the rectangle.
    // then rot(-tilt) is applied to straighten up the image
    double alpha = cos(angle * PI / 180.);
    double beta = -sin(angle * PI / 180.);

    cv::Mat warp_mat( 2, 3, CV_64FC1 );

    // rotation
    warp_mat.at<double>(0,0) = alpha;
    warp_mat.at<double>(0,1) =  beta;
    warp_mat.at<double>(1,0) = -beta;
    warp_mat.at<double>(1,1) = alpha;

    // translation
    int xC, yC;
    
    if (xCenter==-1)
        xC = pData->cols/2;
    else
        xC = xCenter;

    if (yCenter==-1)
        yC = pData->rows/2;
    else
        yC = yCenter;

    warp_mat.at<double>(0,2) = width/2. - xC * alpha - yC * beta;
    warp_mat.at<double>(1,2) = height/2. + xC * beta - yC * alpha;

    warpAffine(*pData, workingImg, warp_mat, cv::Size(width, height));

    releaseInPort(imageInPort);
    reserveOutPort(imageOutPort);
    processingTerminated();

    getDataToWrite<cv::Mat>(imageOutPort, pData);

    *pData = workingImg;

    notifyOutPortReady(imageOutPort, outAttr);
}

#endif /* HAVE_OPENCV */
