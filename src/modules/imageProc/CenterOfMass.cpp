/**
 * @file	src/modules/imageProc/CenterOfMass.cpp
 * @date	Jul 2018
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

#include "CenterOfMass.h"

#include <opencv2/core/core.hpp>

size_t CenterOfMass::refCount = 0;

CenterOfMass::CenterOfMass(ModuleFactory* parent, std::string customName):
	Module(parent, customName), 
	xCenter(-1), yCenter(-1)
{
    if (refCount)
        setInternalName("centerOfMass" + Poco::NumberFormatter::format(refCount));
    else
        setInternalName("centerOfMass");
        
    setCustomName(customName);
    setLogger("module." + name());

    // ports
	setInPortCount(inPortCnt);

    setOutPortCount(outPortCnt);

    addInPort("image", "Image for which the center of mass has to be computed", DataItem::typeCvMat, imageInPort);
	addInPort("mask", "[optional] mask defining from which pixels to compute the center of mass", DataItem::typeCvMat, maskInPort);

    addOutPort("xPosition", "x position of the center of mass", DataItem::typeFloat, xPosPort);
	addOutPort("yPosition", "x position of the center of mass", DataItem::typeFloat, yPosPort);

    notifyCreation();
    refCount++;
}

void CenterOfMass::process(int startCond)
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
		computeCenterOfMass(*imgData, *maskData);
    }
	else
    {
		computeCenterOfMass(*imgData, cv::Mat());
    }

    releaseInPort(imageInPort);
    if (withMask)
        releaseInPort(maskInPort);

    reserveOutPort(xPosPort);
	reserveOutPort(yPosPort);

    // do not uncomment: we still use xCenter and yCenter
    // processingTerminated();

    float *pXData, *pYData;
    getDataToWrite<float>(xPosPort, pXData);
	getDataToWrite<float>(yPosPort, pYData);

	*pXData = xCenter;
	*pYData = yCenter;

    notifyOutPortReady(xPosPort, outAttr);
	notifyOutPortReady(yPosPort, outAttr);
}

void CenterOfMass::computeCenterOfMass(cv::Mat imgIn, cv::Mat mask)
{
    if ((imgIn.type()!=CV_8U) && (imgIn.type()!=CV_16U)
    		&& (imgIn.type()!=CV_32F) && (imgIn.type()!=CV_64F))
        throw Poco::NotImplementedException("computeCenterOfMass", "data type is not supported");

    // re-init
    float totalWeight = 0;
	float xPos = 0;
	float yPos = 0;
	float weight = 0;
	xCenter = 0; yCenter = 0;
    
    if (imgIn.type()==CV_8U)
    {
        for (int row = 0; row < imgIn.rows; row++)
            for (int col = 0; col < imgIn.cols; col++)
                if (mask.empty() || mask.at<unsigned char>(row, col))
                {
					weight = imgIn.at<unsigned char>(row,col);
                    totalWeight += weight;
					xPos += weight * col;
					yPos += weight * row;
                }
    }
    else if (imgIn.type()==CV_16U)
    {
        for (int row = 0; row < imgIn.rows; row++)
            for (int col = 0; col < imgIn.cols; col++)
                if (mask.empty() || mask.at<unsigned char>(row, col))
                {
					weight = imgIn.at<unsigned short>(row,col);
					totalWeight += weight;
					xPos += weight * col;
					yPos += weight * row;
				}
    }
	else if (imgIn.type() == CV_32F)
	{
		for (int row = 0; row < imgIn.rows; row++)
			for (int col = 0; col < imgIn.cols; col++)
				if (mask.empty() || mask.at<unsigned char>(row, col))
				{
					weight = imgIn.at<float>(row, col);
					totalWeight += weight;
					xPos += weight * col;
					yPos += weight * row;
				}
	}
	else if (imgIn.type() == CV_64F)
	{
		for (int row = 0; row < imgIn.rows; row++)
			for (int col = 0; col < imgIn.cols; col++)
				if (mask.empty() || mask.at<unsigned char>(row, col))
				{
					weight = imgIn.at<double>(row, col);
					totalWeight += weight;
					xPos += weight * col;
					yPos += weight * row;
				}
	}
    
    poco_information(logger(), "Total weight is: " + Poco::NumberFormatter::format(totalWeight));
    
	if (totalWeight)
	{
		xCenter = xPos / totalWeight;
		yCenter = yPos / totalWeight;
	}
        
}

#endif /* HAVE_OPENCV */