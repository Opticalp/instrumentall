/**
 * @file	src/modules/imageProc/HistogramMod.cpp
 * @date	Jun 2018
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

#include "HistogramMod.h"

#include <opencv2/core/core.hpp>

size_t HistogramMod::refCount = 0;

HistogramMod::HistogramMod(ModuleFactory* parent, std::string customName):
	Module(parent, customName)
{
    if (refCount)
        setInternalName("Histogram" + Poco::NumberFormatter::format(refCount));
    else
        setInternalName("Histogram");
        
    setCustomName(customName);
    setLogger("module." + name());

    // ports
	setInPortCount(inPortCnt);

    setOutPortCount(outPortCnt);

    addInPort("image", "8-bit or 16-bit image for which the histogram has to be computed", DataItem::typeCvMat, imageInPort);
	addInPort("mask", "[optional] mask defining from which pixels to compute the histogram", DataItem::typeCvMat, maskInPort);

    addOutPort("histogram", "Histogram of the image (256 or more indexes)", DataItem::typeFloat | DataItem::contVector, histogramPort);

    notifyCreation();
    refCount++;
}

void HistogramMod::process(int startCond)
{
	bool withMask;
	switch (startCond)
	{
	case allDataStartState:
        poco_information(logger(), name() + " starting with mask");
		withMask = true;
		break;
	case allPluggedDataStartState:
        poco_information(logger(), name() + " starting without mask");
		withMask = false;
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
    {
        outAttr += attrMask;
		computeHistogram(*imgData, *maskData);
    }
	else
    {
		computeHistogram(*imgData, cv::Mat());
    }

    releaseInPort(imageInPort);
    if (withMask)
        releaseInPort(maskInPort);

    reserveOutPort(histogramPort);
    
    // do not uncomment: we still use histogram
    // processingTerminated();

    std::vector<float>* pData;
    getDataToWrite< std::vector<float> >(histogramPort, pData);

    pData->resize(4096);
    for (int ind = 0; ind < 4096; ind++)
        (*pData)[ind] = histogram[ind];

    notifyOutPortReady(histogramPort, outAttr);
}

void HistogramMod::computeHistogram(cv::Mat imgIn, cv::Mat mask)
{
    if ((imgIn.type()!=CV_8U) && (imgIn.type()!=CV_16U)
    		&& (imgIn.type()!=CV_32F) && (imgIn.type()!=CV_64F))
        throw Poco::NotImplementedException("computeHistogram", "data type is not CV_8U");

    if ((imgIn.type()==CV_32F) || (imgIn.type()==CV_64F))
    	poco_notice(logger(), "assuming that the input float-pixel image has values in [0.0 .. 1.0]");

    // re-init
    size_t count = 0;
    for (int i=0; i< 4096; i++)
        histogram[i] = 0;
    
    if (imgIn.type()==CV_8U)
    {
        for (int row = 0; row < imgIn.rows; row++)
            for (int col = 0; col < imgIn.cols; col++)
                if (mask.empty() || mask.at<unsigned char>(row, col))
                {
                    histogram[imgIn.at<unsigned char>(row,col)]++;
                    count++;
                }
    }
    else if (imgIn.type()==CV_16U)
    {
        for (int row = 0; row < imgIn.rows; row++)
            for (int col = 0; col < imgIn.cols; col++)
                if (mask.empty() || mask.at<unsigned char>(row, col))
                {
                    histogram[imgIn.at<unsigned char>(row,col) >> 4]++;
                    count++;
                }
    }
    else
    {
        for (int row = 0; row < imgIn.rows; row++)
            for (int col = 0; col < imgIn.cols; col++)
                if (mask.empty() || mask.at<unsigned char>(row, col))
                {
                    histogram[cvRound(4096*imgIn.at<double>(row,col))]++;
                    count++;
                }
    }
    
    poco_information(logger(), "counted pixels is: " + Poco::NumberFormatter::format(count));
    
    if (count)
        for (int i=0; i< 4096; i++)
            histogram[i] = histogram[i]/count;
}

#endif /* HAVE_OPENCV */