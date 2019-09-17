/**
 * @file	src/modules/imageProc/MinMaxImg.cpp
 * @date	Feb. 2019
 * @author	PhRG / Opticalp.fr
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

#ifdef HAVE_OPENCV

#include "MinMaxImg.h"

#include <opencv2/imgproc/imgproc.hpp>

#include "Poco/NumberFormatter.h"

size_t MinMaxImg::refCount = 0;

MinMaxImg::MinMaxImg(ModuleFactory* parent, std::string customName):
    Module(parent, customName)
{
    if (refCount)
        setInternalName("MinMaxImg" + Poco::NumberFormatter::format(refCount));
    else
        setInternalName("MinMaxImg");

    setCustomName(customName);
    setLogger("module." + name());

    // parameters
	setParameterCount(paramCnt);
	addParameter(paramCombType, "combType",
		"Combination type: \n"
		" - \"min\" takes for each pixel the min value of the input images (binary operation AND), \n"
		" - \"max\" takes the max of the input images (binary OR)",
		ParamItem::typeString,"max");

	setParametersDefaultValue();

    // ports
    setInPortCount(inPortCnt);
    setOutPortCount(outPortCnt);

    addInPort("imgA", "image A. Preferably 8-bit image. ", DataItem::typeCvMat, imgAInPort);
    addInPort("imgB", "image B. Same type than A; same size. ", DataItem::typeCvMat, imgBInPort);

    addOutPort("image", "Combination of the input images. ", DataItem::typeCvMat, imageOutPort);

    notifyCreation();

    // if nothing failed
    refCount++;
}

#include "Poco/String.h"

std::string MinMaxImg::getStrParameterValue(size_t paramIndex)
{
	switch (paramIndex)
    {
    case paramCombType:
        switch(imgInType)
        {
        case imgInTypeMin:
            return "min";
        case imgInTypeMax:
            return "max";
        default:
            poco_bugcheck_msg("MinMaxImg::getParameter(imgInType) wrong intern imgInType");
            throw Poco::BugcheckException();
        }
        break;
    default:
        poco_bugcheck_msg("getStrParameterValue: wrong index");
        throw Poco::BugcheckException();
    }
}

void MinMaxImg::setStrParameterValue(size_t paramIndex, std::string value)
{
    switch (paramIndex)
    {
    case paramCombType:
    {
        if (Poco::icompare(value, "min") == 0)
            imgInType = imgInTypeMin;
		else if (Poco::icompare(value, "max") == 0)
            imgInType = imgInTypeMax;
        else
            throw Poco::DataFormatException("setParameterValue",
                    value + ": combType can only be set to \"min\" or \"max\"");
		break;
    }
    default:
        poco_bugcheck_msg("setStrParameterValue: wrong index");
        throw Poco::BugcheckException();
    }
}

void MinMaxImg::process(int startCond)
{
    if (startCond != allDataStartState)
    {
        poco_information(logger(), name() + ": no input data. Exiting. ");
        return;
    }

    DataAttributeIn attrA, attrB;
    cv::Mat *pImA, *pImB;

    readLockInPort(imgAInPort);
    readInPortData<cv::Mat>(imgAInPort, pImA);
    readInPortDataAttribute(imgAInPort, &attrA);
    readLockInPort(imgBInPort);
    readInPortData<cv::Mat>(imgBInPort, pImB);
    readInPortDataAttribute(imgBInPort, &attrB);

    DataAttributeOut outAttr = attrA + attrB;

	cv::Mat imgOut;

//	imgHeight= pImA->rows;
//	imgWidth = pImA->cols;
//    
//    if ((pImB->rows != imgHeight) || (pImB->cols != imgWidth))
//        throw Poco::RuntimeException(name(),"size(imA) != size(imB)");
//        
//    if (pImA->type() != pImB->type())
//        throw Poco::RuntimeException(name(),"type(imA) != type(imB)");
//
//    if (pImA->type() != CV_8U)
//        throw Poco::NotImplementedException(name(),"Only unsigned char img are supported by now");
        
    switch (imgInType)
    {
    case imgInTypeMin:
        cv::min(*pImA, *pImB, imgOut);
        break;
    case imgInTypeMax:
        cv::max(*pImA, *pImB, imgOut);
        break;
    default:
        throw Poco::NotImplementedException("MinMaxImg",
                        "The requested combType is not implemented");
    }

    releaseAllInPorts();
    
    reserveOutPort(imageOutPort);
    processingTerminated();

    getDataToWrite<cv::Mat>(imageOutPort, pImA);

	*pImA = imgOut;

    notifyOutPortReady(imageOutPort, outAttr);
}

#endif /* HAVE_OPENCV */
