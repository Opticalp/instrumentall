/**
 * @file	src/modules/imageProc/ThresPop.cpp
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
#include "ThresPop.h"

#include <opencv2/core/core.hpp>

size_t ThresPop::refCount = 0;

ThresPop::ThresPop(ModuleFactory* parent, std::string customName):
	Module(parent, customName)
{
	setInternalName("ThresPop" + Poco::NumberFormatter::format(refCount));
    setCustomName(customName);
    setLogger("module." + name());

    // parameters
    setParameterCount(paramCnt);
    addParameter(paramThresholdValue, "thresholdValue",
            "Threshold value as a fraction of the population. "
            "E.g. a threshold value of 0.5 means that all pixels"
            "below the median value will be thresholded if lowHigh is low, "
            "or that all pixels above the median value will be thresholded"
            "if lowHigh is high",
            ParamItem::typeFloat, "0.5");
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
	addInPort("mask", "[optional] mask defining where to apply the threshold", DataItem::typeCvMat, maskInPort);

    addOutPort("binImage", "8-bit binary thresholded image. See \"onValue\" parameter description. ",
            DataItem::typeCvMat, imageOutPort);
    addOutPort("median", "Median value of the image", DataItem::typeFloat, medianPort);

    notifyCreation();
    refCount++;
}

Poco::Int64 ThresPop::getIntParameterValue(size_t paramIndex)
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

void ThresPop::setIntParameterValue(size_t paramIndex, Poco::Int64 value)
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

double ThresPop::getFloatParameterValue(size_t paramIndex)
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

void ThresPop::setFloatParameterValue(size_t paramIndex, double value)
{
    switch (paramIndex)
    {
    case paramThresholdValue:
    	if (value<0 || value>1)
    		throw Poco::RangeException("setParameterValue",
    				"thresholdValue has to be in [0.0 .. 1.0]");
        threshold = value;
        break;
    default:
        poco_bugcheck_msg("impossible parameter index");
    }
}

#include "Poco/String.h"

std::string ThresPop::getStrParameterValue(size_t paramIndex)
{
    poco_assert(paramIndex == paramLowHigh);

    if (high)
        return "high";
    else
        return "low";
}

void ThresPop::setStrParameterValue(size_t paramIndex, std::string value)
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

void ThresPop::process(int startCond)
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

    double median;
    double thres = thresholdValue(imgData, maskData, median);

    cv::Mat workingImg;

	workingImg = doThreshold(imgData, maskData, thres);

    releaseInPort(imageInPort);
    if (withMask)
        releaseInPort(maskInPort);

     reserveLockOut(); // recursive outMutex locked once

     std::set<size_t> somePorts;
     somePorts.insert(imageOutPort);
     somePorts.insert(medianPort);

     float* pFltData;

     try
     {
	       while (somePorts.size())
  	   {
      	   switch (reserveOutPortOneOf(somePorts)) // outMutex locked twice
		   {
      	   case medianPort:
      		   getDataToWrite<float>(medianPort, pFltData);
      		   *pFltData = static_cast<float>(median);
  	    	   notifyOutPortReady(medianPort, outAttr);
  	           break;
  	       case imageOutPort:
	    	   getDataToWrite<cv::Mat>(imageOutPort, imgData);
  	    	   *imgData = workingImg;
  	    	   notifyOutPortReady(imageOutPort, outAttr);
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

double ThresPop::thresholdValue(cv::Mat* pImg, cv::Mat* pMask, double& median)
{
	if (pMask)
		computeHistogram(*pImg, *pMask);
	else
		computeHistogram(*pImg, cv::Mat());

    size_t count = (pMask == NULL) ? pImg->total() : cv::countNonZero(*pMask);
    double ret = getPopulationValue(static_cast<size_t>(threshold * count));

    if ((pImg->type()==CV_32F) || (pImg->type()==CV_64F))
    	ret /= 4096;

    return ret;
}

void ThresPop::computeHistogram(cv::Mat imgIn, cv::Mat mask)
{
    if ((imgIn.type()!=CV_8U) && (imgIn.type()!=CV_16U)
    		&& (imgIn.type()!=CV_32F) && (imgIn.type()!=CV_64F))
        throw Poco::NotImplementedException("computeHistogram", "data type is not CV_8U");

    if ((imgIn.type()==CV_32F) || (imgIn.type()==CV_64F))
    	poco_notice(logger(), "assuming that the input float-pixel image has values in [0.0 .. 1.0]");

    for (int i=0; i< 4096; i++)
        histogram[i] = 0;

    if (imgIn.type()==CV_8U)
    {
        for (int row = 0; row < imgIn.rows; row++)
            for (int col = 0; col < imgIn.cols; col++)
            {
                if (mask.empty() || mask.at<unsigned char>(row, col))
                    histogram[imgIn.at<unsigned char>(row,col)]++;
            }
    }
    else if (imgIn.type()==CV_16U)
    {
        for (int row = 0; row < imgIn.rows; row++)
            for (int col = 0; col < imgIn.cols; col++)
            {
                if (mask.empty() || mask.at<unsigned char>(row, col))
                    histogram[imgIn.at<unsigned char>(row,col) >> 4]++;
            }
    }
    else
    {
        for (int row = 0; row < imgIn.rows; row++)
            for (int col = 0; col < imgIn.cols; col++)
            {
                if (mask.empty() || mask.at<unsigned char>(row, col))
                    histogram[cvRound(4096*imgIn.at<double>(row,col))]++;
            }
    }
}

double ThresPop::getPopulationValue(size_t count)
{
     size_t accum = 0;

     for (int i=0; i<4096; i++)
     {
         accum += histogram[i];

         if (accum >= count)
             return i;
     }

	 return 4096;
}


#endif /* HAVE_OPENCV */
