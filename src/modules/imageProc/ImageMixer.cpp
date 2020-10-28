/**
 * @file    src/Modules/imageProc/ImageMixer.cpp
 * @date    Oct 2020
 * @author	PhRG - opticalp.fr
 */

/*
 Copyright (c) 2020 Ph. Renaud-Goud / Opticalp

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

#include "ImageMixer.h"

#include <opencv2/imgproc/imgproc.hpp>

#include "Poco/NumberFormatter.h"

size_t ImageMixer::refCount = 0;

ImageMixer::ImageMixer(ModuleFactory* parent, std::string customName):
            Module(parent, customName), a(1), b(1), offset(0)
{
    if (refCount)
         setInternalName("ImageMixer" + Poco::NumberFormatter::format(refCount));
     else
         setInternalName("ImageMixer");

     setCustomName(customName);
     setLogger("module." + name());

     // parameters
     setParameterCount(paramCnt);
     addParameter(paramA, "a",
             "Scale factor to be applied to image A",
             ParamItem::typeFloat, "1");
     addParameter(paramB, "b",
             "Scale factor to be applied to image B (input port or file)",
             ParamItem::typeFloat, "1");
     addParameter(paramOffset, "offset",
             "Offset to be applied to the sum a.imageA + b.imageB",
             ParamItem::typeFloat, "0");
     addParameter(paramFilePath, "filePath",
             "Image file path to be used if the image B input port\n"
             "is not plugged. ",
             ParamItem::typeString, "");

     setParametersDefaultValue();

     // ports
     setInPortCount(inPortCnt);
     setOutPortCount(outPortCnt);

     addInPort("imageA", "Image A. uchar8 is scaled down to [0 1].", DataItem::typeCvMat, imageAInPort);
     addInPort("imageB", "Image B. uchar8 is scaled down to [0 1]. \n"
    		 "Can be omitted if filePath is used",
    		 DataItem::typeCvMat, imageBInPort);

     addOutPort("image", "Floating point image: a.imageA + b.imageB + offset", DataItem::typeCvMat, imageOutPort);

     notifyCreation();

     // if nothing failed
     refCount++;
}

double ImageMixer::getFloatParameterValue(size_t paramIndex)
{
    switch (paramIndex)
    {
    case paramA:
        return a;
    case paramB:
        return b;
    case paramOffset:
    	return offset;
    default:
        poco_bugcheck_msg("unknown parameter index");
        throw Poco::BugcheckException();
    }
}

void ImageMixer::setFloatParameterValue(size_t paramIndex, double value)
{
    switch (paramIndex)
    {
    case paramA:
        a = value;
        break;
    case paramB:
        b = value;
        break;
    case paramOffset:
    	offset = value;
    	break;
    default:
        poco_bugcheck_msg("unknown parameter index");
    }
}

std::string ImageMixer::getStrParameterValue(size_t paramIndex)
{
    switch (paramIndex)
    {
    case paramFilePath:
    	return imagePath;
        break;
    default:
        poco_bugcheck_msg("unknown parameter index");
        throw Poco::BugcheckException();
    }
}

#include "Poco/Path.h"
void ImageMixer::setStrParameterValue(size_t paramIndex, std::string value)
{
	std::string path;

	switch (paramIndex)
    {
    case paramFilePath:
    	if (value.empty())
    	{
    		imagePath.clear();
    		return;
    	}
    	else
    		path = Poco::Path(value).absolute().toString();
        break;
    default:
        poco_bugcheck_msg("unknown parameter index");
    }

    // Try to load the image
	cv::Mat tmp = cv::imread(path.c_str(), cv::IMREAD_GRAYSCALE);

	poco_information(logger(), "image (" + path + ") size (w x h): " +
			Poco::NumberFormatter::format(tmp.cols) + " x " +
			Poco::NumberFormatter::format(tmp.rows) );

	imagePath = path;
	tmp.convertTo(fileImage, CV_64F, 1.0/255);
}

void ImageMixer::process(int startCond)
{
	switch (startCond)
	{
	case allPluggedDataStartState:
		if (!getInPort(imageAInPort)->hasDataSource())
		{
	        poco_error(logger(), name() + ": no input image A. Exiting. ");
	        return;
		}
		break;
	case allDataStartState:
		break;
	default:
        poco_error(logger(), name() + ": no input data. Exiting. ");
        return;
	}

	DataAttributeIn attr;
	cv::Mat imgA, imgB, result;
	cv::Mat *pData;

	readLockInPort(imageAInPort);
	readInPortData<cv::Mat>(imageAInPort, pData);
    if (pData->channels()>1)
        throw Poco::RuntimeException(name(),"color image input not supported");
	switch (pData->type())
	{
	case CV_8U:
		pData->convertTo(imgA, CV_64F, 1./255);
		break;
	case CV_64F:
		imgA = *pData;
		break;
	default:
		pData->convertTo(imgA, CV_64F);
	}
	readInPortDataAttribute(imageAInPort, &attr);
	DataAttributeOut outAttr(attr);

	if (getInPort(imageBInPort)->hasDataSource())
	{
		readLockInPort(imageBInPort);
		readInPortData<cv::Mat>(imageBInPort, pData);
	    if (pData->channels()>1)
	        throw Poco::RuntimeException(name(),"color image input not supported");
		switch (pData->type())
		{
		case CV_8U:
			pData->convertTo(imgB, CV_64F, 1./255);
			break;
		case CV_64F:
			imgB = *pData;
			break;
		default:
			pData->convertTo(imgB, CV_64F);
		}
		releaseInPort(imageBInPort);
	}
	else if (!fileImage.empty())
	{
		imgB = fileImage;
	}
	else
	{
		throw Poco::RuntimeException(name(),
				"imageB input port is not plugged and the image from file path is empty");
	}

	// warn if sizes differ
	if (imgA.size() != imgB.size())
	{
		poco_error(logger(), "image A size (w x h): " +
				Poco::NumberFormatter::format(imgA.cols) + " x " +
				Poco::NumberFormatter::format(imgA.rows) );
		poco_error(logger(), "image B size (w x h): " +
				Poco::NumberFormatter::format(imgB.cols) + " x " +
				Poco::NumberFormatter::format(imgB.rows) );
		throw Poco::RuntimeException(name(), "size(imageA) != size(imageB). Can not mix. ");
	}

	result = a*imgA + b*imgB + offset;

	releaseAllInPorts();

    reserveOutPort(imageOutPort);
    processingTerminated();

    getDataToWrite<cv::Mat>(imageOutPort, pData);

    *pData = result;

    notifyOutPortReady(imageOutPort, outAttr);
}


#endif /* HAVE_OPENCV */
