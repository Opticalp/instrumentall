/**
 * @file	src/modules/imageProc/ThresAbs.cpp
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
#include "ThresAbs.h"

#include <opencv2/core/core.hpp>

size_t ThresAbs::refCount = 0;

ThresAbs::ThresAbs(ModuleFactory* parent, std::string customName):
	Module(parent, customName)
{
    setInternalName("ThresAbs" + Poco::NumberFormatter::format(refCount));
    setCustomName(customName);
    setLogger("module." + name());

    // parameters
    setParameterCount(paramCnt);
    addParameter(paramThresholdValue, "thresholdValue",
            "Threshold value. If a value is equal to the threshold value, "
            "it is considered as thresholded. ",
            ParamItem::typeFloat, "255");
    addParameter(paramLowHigh, "lowHigh",
            "specify if it is a low threshold (\"low\"), or a high threshold (\"high\")",
            ParamItem::typeString, "high");
    addParameter(paramOnValue, "onValue",
            "Binary ON value: value given to a thresholded pixel in the output image. "
            "If this value is -1, and a mask is present, the binary output is the mask value. "
            "If no mask is given, and this value is -1, it will be changed to 255. ",
            ParamItem::typeInteger, "-1");

    setFloatParameterValue(paramThresholdValue, getFloatParameterDefaultValue(paramThresholdValue));
    setIntParameterValue(paramOnValue, getIntParameterDefaultValue(paramOnValue));
    setStrParameterValue(paramLowHigh, getStrParameterDefaultValue(paramLowHigh));

    // ports
    setInPortCount(inPortCnt);

    setOutPortCount(outPortCnt);

    addInPort("image", "8-bit or 16-bit image to be thresholded", DataItem::typeCvMat, imageInPort);
	addInPort("mask", "mask defining where to apply the threshold", DataItem::typeCvMat, maskInPort);

    addOutPort("binImage", "8-bit binary thresholded image. See \"onValue\" parameter description. ",
            DataItem::typeCvMat, imageOutPort);
    addOutPort("count", "count of thresholded pixels", DataItem::typeInt64, cntOutPort);
    addOutPort("totalCount", "total count of analyzed pixels", DataItem::typeInt64, totCntOutPort);

    notifyCreation();
    refCount++;
}

Poco::Int64 ThresAbs::getIntParameterValue(size_t paramIndex)
{
    switch (paramIndex)
    {
    case paramOnValue:
        return onValue;
    default:
        poco_bugcheck_msg("impossible parameter index");
        throw Poco::BugcheckException();
    }
}

void ThresAbs::setIntParameterValue(size_t paramIndex, Poco::Int64 value)
{
    switch (paramIndex)
    {
    case paramOnValue:
    	if ((value!=-1) && ((value<1) || (value>255)))
            throw Poco::RangeException("setParameterValue",
                    "onValue has to be in [1 .. 255] or -1");
        onValue = value;
        break;
    default:
        poco_bugcheck_msg("impossible parameter index");
    }
}

double ThresAbs::getFloatParameterValue(size_t paramIndex)
{
    switch (paramIndex)
    {
    case paramThresholdValue:
        return threshold;
    default:
        poco_bugcheck_msg("impossible parameter index");
        throw Poco::BugcheckException();
    }
}

void ThresAbs::setFloatParameterValue(size_t paramIndex, double value)
{
    switch (paramIndex)
    {
    case paramThresholdValue:
        if ((value<0) || (value>65535))
            throw Poco::RangeException("setParameterValue",
                    "thresholdValue has to be in [0 .. 65535]");
        threshold = value;
        break;
    default:
        poco_bugcheck_msg("impossible parameter index");
    }
}

#include "Poco/String.h"

std::string ThresAbs::getStrParameterValue(size_t paramIndex)
{
    poco_assert(paramIndex == paramLowHigh);

    if (high)
        return "high";
    else
        return "low";
}

void ThresAbs::setStrParameterValue(size_t paramIndex, std::string value)
{
    poco_assert(paramIndex == paramLowHigh);

    if (Poco::toLower(value).compare("low") == 0)
        high = false;
    else if (Poco::toLower(value).compare("high") == 0)
        high = true;
    else
        throw Poco::InvalidArgumentException("setParameterValue",
                "lowHigh has to be \"low\" or \"high\"");
}

void ThresAbs::process(int startCond)
{
	bool withMask;
	switch (startCond)
	{
	case allDataStartState:
		withMask = true;
		break;
	case allPluggedDataStartState:
		withMask = false;
		if (onValue == -1)
			onValue = 255;
		break;
	default:
        poco_information(logger(), name() + ": no input data. Exiting. ");
        return;
	}

    DataAttributeIn attrIn, attrMask;
    cv::Mat* imgData;
    cv::Mat* maskData = NULL;

    readLockInPort(imageInPort);
    readInPortData<cv::Mat>(imageInPort, imgData);
    readInPortDataAttribute(imageInPort, &attrIn);

    if (withMask)
    {
        readLockInPort(maskInPort);
        readInPortData<cv::Mat>(maskInPort, maskData);
        readInPortDataAttribute(maskInPort, &attrMask);
    }

    DataAttributeOut outAttr = attrIn;

    if (withMask)
        outAttr += attrMask;

    cv::Mat workingImg;

    size_t cnt, totCnt;
	workingImg = doThreshold(imgData, maskData, threshold, cnt, totCnt);

    releaseInPort(imageInPort);
    if (withMask)
        releaseInPort(maskInPort);

    reserveLockOut(); // recursive outMutex locked once

    std::set<size_t> somePorts;
    somePorts.insert(imageOutPort);
    somePorts.insert(cntOutPort);
    somePorts.insert(totCntOutPort);

    float* pFltData;
    Poco::Int64* pIntData;

    try
    {
          while (somePorts.size())
      {
          switch (reserveOutPortOneOf(somePorts)) // outMutex locked twice
          {
          case imageOutPort:
              getDataToWrite<cv::Mat>(imageOutPort, imgData);
              *imgData = workingImg;
              notifyOutPortReady(imageOutPort, outAttr);
              break;
          case cntOutPort:
              getDataToWrite<Poco::Int64>(cntOutPort, pIntData);
              *pIntData = static_cast<Poco::Int64>(cnt);
              notifyOutPortReady(cntOutPort, outAttr);
              break;
          case totCntOutPort:
              getDataToWrite<Poco::Int64>(totCntOutPort, pIntData);
              *pIntData = static_cast<Poco::Int64>(totCnt);
              notifyOutPortReady(totCntOutPort, outAttr);
              break;
          default:
              poco_bugcheck_msg("impossible reserved port");
          }
        }
    }
    catch (...) // prevent deadlock if canceling during reserveOutPortOneOf
    {
           unlockOut();
           throw;
    }
    unlockOut(); // outMutex released twice. OK. unlocked.
}

#endif /* HAVE_OPENCV */
