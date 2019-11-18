/**
 * @file	src/modules/imageProc/BorderCut.cpp
 * @date	Feb. 2019
 * @author	PhRG - opticalp.fr
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

#include "BorderCut.h"

#include <opencv2/imgproc/imgproc.hpp>

#include "Poco/NumberFormatter.h"
#include <math.h>

size_t BorderCut::refCount = 0;

BorderCut::BorderCut(ModuleFactory* parent, std::string customName):
    Module(parent, customName)
{
    if (refCount)
        setInternalName("BorderCut" + Poco::NumberFormatter::format(refCount));
    else
        setInternalName("BorderCut");

    setCustomName(customName);
    setLogger("module." + name());

    // parameters
    setParameterCount(paramCnt);
    addParameter(paramMinEdgeLength, "minEdgeLength",
            "Minimum length of the border edge (pixels). "
            "I.e. the threshold has to be overpassed this count of times "
            "before the edge being considered as an edge. ",
            ParamItem::typeInteger, "4");
    addParameter(paramPadding, "padding",
            "Reserved distance between the image content and the border (pixels)",
            ParamItem::typeInteger, "15");
    addParameter(paramThreshold, "threshold",
            "Derivative (adjacent diff) threshold value. ",
            ParamItem::typeFloat, "4");

    setParametersDefaultValue();

    // ports
    setInPortCount(inPortCnt);
    setOutPortCount(outPortCnt);

    addInPort("image", "image with potential border", DataItem::typeCvMat, imageInPort);

    addOutPort("image", "cropped image if border(s) was(were) detected", DataItem::typeCvMat, imageOutPort);

    notifyCreation();

    // if nothing failed
    refCount++;
}

Poco::Int64 BorderCut::getIntParameterValue(size_t paramIndex)
{
    switch (paramIndex)
    {
    case paramMinEdgeLength:
        return minEdgeLen;
    case paramPadding:
        return padding;
    default:
        poco_bugcheck_msg("impossible param index in getIntParameterValue");
        throw Poco::BugcheckException();
    }
}

void BorderCut::setIntParameterValue(size_t paramIndex, Poco::Int64 value)
{
    switch (paramIndex)
    {
    case paramMinEdgeLength:
        if (value < 0)
            throw Poco::RangeException("setParameterValue",
                    "minEdgeLength has to be >= 0");
        minEdgeLen = value;
        break;
    case paramPadding:
        if (value < 0)
            throw Poco::RangeException("setParameterValue",
                    "Padding has to be >= 0");
        padding = value;
        break;
    default:
        poco_bugcheck_msg("impossible param index in getIntParameterValue");
        throw Poco::BugcheckException();
    }
}

double BorderCut::getFloatParameterValue(size_t paramIndex)
{
    poco_assert(paramIndex == paramThreshold);

    return thres;
}

void BorderCut::setFloatParameterValue(size_t paramIndex, double value)
{
    poco_assert(paramIndex == paramThreshold);

    thres = value;
}

#include "Poco/Format.h"

void BorderCut::process(int startCond)
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

	int xLeft, xRight; 
	int yTop, yBottom;

    cv::Mat tmp1, tmp2;
    int borderLeft, borderRight, borderTop, borderBottom;

    cv::reduce(*pData,tmp1,0,CV_REDUCE_AVG,CV_32F); // result is a row
    
    borderLeft = hasEdge(tmp1);
	if (borderLeft)
		poco_information(logger(), "left border at: " + Poco::NumberFormatter::format(borderLeft));

	xLeft = borderLeft;
    
    cv::flip(tmp1,tmp2,1); // flip row
    
    borderRight = hasEdge(tmp2);
    if (borderRight)
        poco_information(logger(), "right border at: " + Poco::NumberFormatter::format(-borderRight));

	xRight = pData->cols - borderRight - 1;

    cv::reduce(*pData,tmp1,1,CV_REDUCE_AVG,CV_32F); // result is a column
    tmp1 = tmp1.t();

    borderTop = hasEdge(tmp1);
    if (borderTop)
        poco_information(logger(), "top border at: " + Poco::NumberFormatter::format(borderTop));

	yTop = borderTop;
    
    cv::flip(tmp1,tmp2,1); // flip row

    borderBottom = hasEdge(tmp2);
    if (borderBottom)
        poco_information(logger(), "bottom border at: " + Poco::NumberFormatter::format(-borderBottom));

	yBottom = pData->rows - borderBottom - 1;

	poco_information(logger(), "Cropping +------ " + 
	Poco::NumberFormatter::format(yTop) + " --------");
	poco_information(logger(), "         |                     |");
	poco_information(logger(), "       " + 
		Poco::NumberFormatter::format(xLeft) + "                 " +
		Poco::NumberFormatter::format(xRight)  );
	poco_information(logger(), "         |                     |");
	poco_information(logger(), "         + ------ " +
		Poco::NumberFormatter::format(yBottom) + " --------");

	(*pData)( cv::Range(yTop, yBottom+1),
					 cv::Range(xLeft, xRight+1) ).copyTo(tmp1);
	poco_information(logger(), "Cropping done. ");
  
    releaseInPort(imageInPort);
    reserveOutPort(imageOutPort);
    processingTerminated();

    getDataToWrite<cv::Mat>(imageOutPort, pData);

    *pData = tmp1;

    notifyOutPortReady(imageOutPort, outAttr);
}

int BorderCut::hasEdge(cv::Mat inVec)
{
    poco_information(logger(), "vector size: " + 
        Poco::NumberFormatter::format(inVec.rows) + ";" +
        Poco::NumberFormatter::format(inVec.cols) );
    
    // diff
    cv::Mat tmp1, diff;
    diff = inVec(cv::Range::all(),cv::Range(1,inVec.cols)) - inVec(cv::Range::all(),cv::Range(0,inVec.cols-1));
    

    double minTmp,maxTmp;
    cv::minMaxLoc(diff,&minTmp, &maxTmp);
    poco_information(logger(), "Max diff is " + Poco::NumberFormatter::format(maxTmp));


    // scan
    int fail(0);
    for (int start = 0; start < diff.cols; start++)
    {
//        poco_information(logger(), 
//            "#" + Poco::NumberFormatter::format(start) + ":\t" +
//            Poco::NumberFormatter::format(inVec.at<float>(0,start)) + "\t;\t" + 
//            Poco::NumberFormatter::format(inVec.at<float>(0,start+1)) + "\t>>>\t" +
//            Poco::NumberFormatter::format(diff.at<float>(0,start)));
        
        if (diff.at<float>(0,start) < thres)
        {
            if (fail == 0)
                continue;
            else if (fail < minEdgeLen)
                fail = 0;
            else if (start > padding)
                return start - padding;
            else
                return 0;
        }
        else
        {
            ++fail;
        }            
    }
    
    poco_information(logger(), "no border was found");
    return 0;
}

#endif /* HAVE_OPENCV */
