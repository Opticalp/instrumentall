/**
 * @file	src/modules/imageProc/BoxMask.cpp
 * @date	Jun. 2017
 * @author	PhRG / KG / AI
 */

/*
 Copyright (c) 2017 Ph. Renaud-Goud / Opticalp and contributors

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

#include "BoxMask.h"

#include <opencv2/imgproc/imgproc.hpp>

#include "Poco/NumberFormatter.h"

size_t BoxMask::refCount = 0;

BoxMask::BoxMask(ModuleFactory* parent, std::string customName):
    Module(parent, customName)
{
    if (refCount)
        setInternalName("BoxMask" + Poco::NumberFormatter::format(refCount));
    else
        setInternalName("BoxMask");

    setCustomName(customName);
    setLogger("module." + name());

    // parameters
	setParameterCount(paramCnt);
	addParameter(paramImgInType, "imgInType",
		"type of input image (ref, min or max). \n"
		" - \"ref\" takes only the size of the input image, \n"
		" - \"min\" takes for each pixel the min value of the input image and "
		"the mask (binary operation AND), \n"
		" - \"max\" takes the max of the input image and the mask (binary OR)",
		ParamItem::typeString,"ref");
	addParameter(paramInValue, "inValue",
		"Pixel value to be set inside the box",
		ParamItem::typeInteger, "1");
	addParameter(paramOutValue, "outValue",
	    "Pixel value to be set outside the box",
		ParamItem::typeInteger, "100");
	addParameter(paramBoxType, "boxType",
		"Define the shape of box: rect or ellipse. ",
		ParamItem::typeString, "rect");
	addParameter(paramBoxWidth, "boxWidth",
		"Width of the box. ",
		ParamItem::typeInteger, "1");
	addParameter(paramBoxHeight, "boxHeight",
		"Height of the box. ",
		ParamItem::typeInteger, "1");
	addParameter(paramBoxAngle, "boxAngle",
		"Rotation angle (degrees) of the box. ",
		ParamItem::typeFloat, "0");
	addParameter(paramBoxXCenter, "boxXcenter",
		"X center position of the mask. ",
		ParamItem::typeInteger, "0");
	addParameter(paramBoxYCenter, "boxYcenter",
		"Y center position of the mask. ",
		ParamItem::typeInteger, "0");

	setParametersDefaultValue();

    // ports
    setInPortCount(inPortCnt);
    setOutPortCount(outPortCnt);

    addInPort("image", "reference image", DataItem::typeCvMat, imageInPort);

    addOutPort("mask", "Can be a combination with the reference image. ", DataItem::typeCvMat, maskPort);

    notifyCreation();

    // if nothing failed
    refCount++;
}

Poco::Int64 BoxMask::getIntParameterValue(size_t paramIndex)
{
    switch (paramIndex)
    {
    case paramInValue:
        return inValue;
    case paramOutValue:
        return outValue;
	case paramBoxWidth:
        return boxWidth;
    case paramBoxHeight:
        return boxHeight;
    case paramBoxXCenter:
        return boxXcenter;
    case paramBoxYCenter:
        return boxYcenter;
    default:
        poco_bugcheck_msg("impossible param index in getIntParameterValue");
        throw Poco::BugcheckException();
    }
}

void BoxMask::setIntParameterValue(size_t paramIndex, Poco::Int64 value)
{
    switch (paramIndex)
    {
    case paramInValue:
        if (value < 0 || value > 255)
            throw Poco::RangeException("setParameterValue",
                    "inValue has to be >= 0 and <= 255");
        inValue = value;
        break;
    case paramOutValue:
        if (value < 0 || value > 255)
            throw Poco::RangeException("setParameterValue",
                    "outValue has to be >= 0 and <= 255");
        outValue = value;
        break;
    case paramBoxWidth:
        if (value <= 0)
            throw Poco::RangeException("setParameterValue",
                    "boxWidth has to be > 0");
        boxWidth = value;
        break;
    case paramBoxHeight:
        if (value <= 0)
            throw Poco::RangeException("setParameterValue",
                    "boxHeight has to be > 0");
        boxHeight = value;
        break;
    case paramBoxXCenter:
        //if (value < 0)
        //    throw Poco::RangeException("setParameterValue",
        //            "boxXcenter has to be >= 0");
        boxXcenter = value;
        break;
    case paramBoxYCenter:
        //if (value < 0)
        //    throw Poco::RangeException("setParameterValue",
        //            "boxYcenter has to be >= 0");
        boxYcenter = value;
        break;
    default:
        poco_bugcheck_msg("impossible param index in getIntParameterValue");
        throw Poco::BugcheckException();
    }
}

double BoxMask::getFloatParameterValue(size_t paramIndex)
{
    poco_assert(paramIndex == paramBoxAngle);

    return boxAngle;
}

void BoxMask::setFloatParameterValue(size_t paramIndex, double value)
{
    poco_assert(paramIndex == paramBoxAngle);
    boxAngle = value;
}

#include "Poco/String.h"

std::string BoxMask::getStrParameterValue(size_t paramIndex)
{
	switch (paramIndex)
    {
    case paramImgInType:
        switch(imgInType)
        {
        case imgInTypeRef:
            return "ref";
        case imgInTypeMin:
            return "min";
        case imgInTypeMax:
            return "max";
        default:
            poco_bugcheck_msg("BoxMask::getParameter(imgInType) wrong intern imgInType");
            throw Poco::BugcheckException();
        }
        break;
    case paramBoxType:
        switch (boxType)
        {
        case boxTypeRect:
            return "rect";
        case boxTypeEllipse:
            return "ellipse";
        default:
            poco_bugcheck_msg("BoxMask::getParameter(boxType) wrong intern boxType");
            throw Poco::BugcheckException();
        }
    default:
        poco_bugcheck_msg("getStrParameterValue: wrong index");
        throw Poco::BugcheckException();
    }
}

void BoxMask::setStrParameterValue(size_t paramIndex, std::string value)
{
    switch (paramIndex)
    {
    case paramImgInType:
    {
        if (Poco::icompare(value, "ref") == 0)
            imgInType = imgInTypeRef;
        else if (Poco::icompare(value, "min") == 0)
            imgInType = imgInTypeMin;
		else if (Poco::icompare(value, "max") == 0)
            imgInType = imgInTypeMax;
        else
            throw Poco::DataFormatException("setParameterValue",
                    value + ": imgInType can only be set to ref, min or max");
		break;
    }
    case paramBoxType:
        if (Poco::icompare(value, "rect") == 0)
            boxType = boxTypeRect;
        else if (Poco::icompare(value, "ellipse") == 0)
            boxType = boxTypeEllipse;
        else
            throw Poco::DataFormatException("setParameterValue",
                    value + ": boxType can only be set to rect or ellipse");
        break;
    default:
        poco_bugcheck_msg("setStrParameterValue: wrong index");
        throw Poco::BugcheckException();
    }
}

void BoxMask::process(int startCond)
{
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

	cv::Mat imgOut;

	imgHeight= pData->rows;
	imgWidth = pData->cols;

    cv::Mat maskImg(imgHeight,imgWidth, CV_8U,cv::Scalar(outValue));

	Poco::Timestamp now;

    switch (boxType)
    {
    case boxTypeRect:
        buildMaskRectangle(maskImg);
        break;
    case boxTypeEllipse:
        buildMaskEllipse(maskImg);
        break;
    default:
        throw Poco::NotImplementedException("BoxMask",
                "The requested boxType is not implemented");
    }

    switch (imgInType)
    {
    case imgInTypeRef:
        imgOut = maskImg.clone();
        break;
    case imgInTypeMin:
        cv::min(*pData, maskImg, imgOut);
        break;
    case imgInTypeMax:
        cv::max(*pData, maskImg, imgOut);
        break;
    default:
        throw Poco::NotImplementedException("BoxMask",
                        "The requested imgInType is not implemented");
    }

	poco_information(logger(), name() + " elapsed time (ms) is: "
        + Poco::NumberFormatter::format(now.elapsed()/1000));

    releaseInPort(imageInPort);
    reserveOutPort(maskPort);
    processingTerminated();

    getDataToWrite<cv::Mat>(maskPort, pData);

	*pData = imgOut;

    notifyOutPortReady(maskPort, outAttr);
}

void BoxMask::buildMaskRectangle(cv::Mat& maskOut)
{
    cv::RotatedRect rect(
        cv::Point2f(boxXcenter, boxYcenter),
        cv::Size2f(boxHeight, boxWidth), -boxAngle );

    // array to get the vertexes
    cv::Point2f pts[4];
    rect.points(pts);
    poco_information(logger(), "point[0]: " + Poco::NumberFormatter::format(pts[0].x) + ";" +
        Poco::NumberFormatter::format(pts[0].y) );

    cv::Point* ptsInt = new cv::Point[4];
    for (int ind=0; ind<4 ; ind++)
        ptsInt[ind] = pts[ind];

    int npts=4;
    cv::fillPoly( maskOut,
            const_cast<const cv::Point**>(&ptsInt),
            &npts, 1, inValue);

    delete ptsInt;
}

void BoxMask::buildMaskEllipse(cv::Mat& maskOut)
{
    cv::ellipse(maskOut,
            cv::Point(boxXcenter, boxYcenter),
            cv::Size(boxHeight, boxWidth),
            -boxAngle, 0, 360, inValue, -1);
}
#endif /* HAVE_OPENCV */
