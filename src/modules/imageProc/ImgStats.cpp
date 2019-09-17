/**
 * @file	src/modules/imageProc/ImgStats.cpp
 * @date	Feb. 2017
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
#include "ImgStats.h"

#include <opencv2/core/core.hpp>

#include "Poco/NumberFormatter.h"

size_t ImgStats::refCount = 0;

ImgStats::ImgStats(ModuleFactory* parent, std::string customName):
    Module(parent, customName)
{
    if (refCount)
        setInternalName("ImgStats" + Poco::NumberFormatter::format(refCount));
    else
        setInternalName("ImgStats");

    setCustomName(customName);
    setLogger("module." + name());

    // no parameter

    // ports
    setInPortCount(inPortCnt);
    setOutPortCount(outPortCnt);

    addInPort("image", "Image to be analyzed. ", DataItem::typeCvMat, imagePort);
	addInPort("mask", "mask defining where to analyze mean, min, max, std dev", DataItem::typeCvMat, maskPort);

    addOutPort("height", "height of the image", DataItem::typeInt64, heightPort);
    addOutPort("width", "width of the image", DataItem::typeInt64, widthPort);
    addOutPort("mean", "mean value of the image", DataItem::typeDblFloat, meanPort);
    addOutPort("sigma", "standard deviation of the image values", DataItem::typeDblFloat, sigmaPort);
    addOutPort("min", "min value of the image", DataItem::typeDblFloat, minPort);
    addOutPort("max", "max value of the image", DataItem::typeDblFloat, maxPort);

    notifyCreation();

    // if nothing failed
    refCount++;
}

void ImgStats::process(int startCond)
{
	bool withMask;
	switch (startCond)
	{
	case allDataStartState:
		withMask = true;
		break;
	case allPluggedDataStartState:
		withMask = false;
		break;
	default:
		poco_information(logger(), name() + ": no input data. Exiting. ");
		return;
	}


	DataAttributeIn attrIn, attrMask;
	cv::Mat* imgData;
	cv::Mat* maskData = NULL;

    readLockInPort(imagePort);
    readInPortData<cv::Mat>(imagePort, imgData);
    readInPortDataAttribute(imagePort, &attrIn);

	if (withMask)
	{
		readLockInPort(maskPort);
		readInPortData<cv::Mat>(maskPort, maskData);
		readInPortDataAttribute(maskPort, &attrMask);
	}
	
	DataAttributeOut outAttr = attrIn;

	if (withMask)
		outAttr += attrMask;
	
	cv::Mat workingImg;
    imgData->copyTo(workingImg);

    releaseInPort(imagePort);

    int width, height;
    double min, max, mean, sigma;

    height = workingImg.rows;
    width = workingImg.cols;

	if (withMask)
		cv::minMaxLoc(workingImg, &min, &max,
			0, 0, // min, max loc
			*maskData);
	else
		cv::minMaxLoc(workingImg, &min, &max);

    cv::Mat meanMat, stdDev;
    cv::meanStdDev(workingImg, meanMat, stdDev);

    sigma = stdDev.at<double>(0,0);
    mean = meanMat.at<double>(0,0);

    processingTerminated();

    reserveLockOut(); // recursive outMutex locked once

    std::set<size_t> ports;

    ports.insert(heightPort);
    ports.insert(widthPort);
    ports.insert(meanPort);
    ports.insert(sigmaPort);
    ports.insert(minPort);
    ports.insert(maxPort);

    Poco::Int64* pIntData;
    double* pDblData;

    try
    {
        while (ports.size())
        {
            switch (reserveOutPortOneOf(ports)) // outMutex locked twice
            {
            case heightPort:
                getDataToWrite<Poco::Int64>(heightPort, pIntData);
                *pIntData = height;
                notifyOutPortReady(heightPort, outAttr); // outMutex released once here
                break;
            case widthPort:
                getDataToWrite<Poco::Int64>(widthPort, pIntData);
                *pIntData = width;
                notifyOutPortReady(widthPort, outAttr); // outMutex released once here
                break;
            case meanPort:
                getDataToWrite<double>(meanPort, pDblData);
                *pDblData = mean;
                notifyOutPortReady(meanPort, outAttr); // outMutex released once here
                break;
            case sigmaPort:
                getDataToWrite<double>(sigmaPort, pDblData);
                *pDblData = sigma;
                notifyOutPortReady(sigmaPort, outAttr); // outMutex released once here
                break;
            case minPort:
                getDataToWrite<double>(minPort, pDblData);
                *pDblData = min;
                notifyOutPortReady(minPort, outAttr); // outMutex released once here
                break;
            case maxPort:
                getDataToWrite<double>(maxPort, pDblData);
                *pDblData = max;
                notifyOutPortReady(maxPort, outAttr); // outMutex released once here
                break;
            default:
                poco_bugcheck_msg("impossible port case in reserveOutPortoneOf()");
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
