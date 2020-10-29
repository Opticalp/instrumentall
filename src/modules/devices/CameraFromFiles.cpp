/**
 * @file	src/modules/devices/CameraFromFiles.cpp
 * @date	jan. 2017
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

#include "CameraFromFiles.h"

#include "core/DataAttributeIn.h"
#include "core/DataAttribute.h"

#include "core/InDataPort.h"
#include "core/OutPort.h"

#include "Poco/NumberFormatter.h"
#include "Poco/String.h"
#include "Poco/StringTokenizer.h"

//#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"

size_t CameraFromFiles::refCount = 0;

CameraFromFiles::CameraFromFiles(ModuleFactory* parent, std::string customName):
            Module(parent, customName),
            forceGrayscale(true)
{
    if (refCount)
        setInternalName("CameraFromFiles" + Poco::NumberFormatter::format(refCount));
    else
        setInternalName("CameraFromFiles");

    setCustomName(customName);
    setLogger("module." + name());

    // parameters
    setParameterCount(paramCnt);
    addParameter(paramDirectory, "directory",
            "Base directory for all the files given in the files parameter",
            ParamItem::typeString, ""); // default is empty: relative path
    addParameter(paramFiles, "files",
            "CR-separated list of files to be used as image sources",
            ParamItem::typeString, "");
    currentImgPath = imgPaths.begin();
    addParameter(paramForceGrayscale, "forceGrayscale", "force a color image to be grayscale (ON). "
            "Leave as is elsewhere (OFF)", ParamItem::typeString, "ON");

    setParametersDefaultValue();

    // ports
    setInPortCount(inPortCnt);
    setOutPortCount(outPortCnt);

//    addInPort("inPortA", "data in", DataItem::typeInteger, inPortA);
    addTrigPort("trig", "Launch the image generation", trigPort);

    addOutPort("acqReady", "acquisition ready trigger", DataItem::typeInt32, acqReadyOutPort);
    addOutPort("image", "image from file", DataItem::typeCvMat, imgOutPort);

    notifyCreation();

    // if nothing failed
    refCount++;
}

void CameraFromFiles::process(int startCond)
{
    DataAttributeOut attr;

    switch (startCond)
    {
    case noDataStartState:
        poco_information(logger(), name() + " processing direct launch.");
        break;
    case allDataStartState:
        {
            DataAttributeIn inAttr;
            readLockInPort(trigPort);
            readInPortDataAttribute(trigPort, &inAttr);
            releaseInPort(trigPort);

            attr = inAttr;

            poco_information(logger(), name() + " processing trigged launch.");
            break;
        }
    default:
        poco_bugcheck_msg("impossible start condition");
        throw Poco::BugcheckException();
    }

    dataLock.readLock();

    if (currentImgPath == imgPaths.end())
    {
        poco_notice(logger(), "end of image path stack reached, "
                "starting again from the beginning");
        currentImgPath = imgPaths.begin();
    }

    if (currentImgPath == imgPaths.end())
    {
        poco_warning(logger(), "No image path in the stack. "
                "Please fill the \"files\" parameter");
        return;
    }

    Poco::Path fullImagePath = imgDir;
    fullImagePath.append(*currentImgPath++);

    poco_information(logger(), name() + " tries to open: "
            + fullImagePath.toString());

    dataLock.unlock();

    processingTerminated();

    reserveOutPort(acqReadyOutPort);

    Poco::Int32* pInt32;
    getDataToWrite<Poco::Int32>(acqReadyOutPort, pInt32);
    *pInt32 = 1;
    notifyOutPortReady(acqReadyOutPort, attr);

    poco_information(logger(),"acq ready output port set");

    reserveOutPort(imgOutPort);

    cv::Mat* pMat;
    getDataToWrite<cv::Mat>(imgOutPort, pMat);

    // Read the image file (returns empty Mat if any error occurs)
    if (forceGrayscale)
    {
        poco_information(logger(),
            "Retrieving the image in gray scale format");
        *pMat = cv::imread(fullImagePath.toString().c_str(),
                cv::IMREAD_GRAYSCALE);
    }
    else
    {
        poco_information(logger(),
            "Retrieving the image in original color format");
        *pMat = cv::imread(fullImagePath.toString().c_str(),
                cv::IMREAD_UNCHANGED);
    }

    if (!pMat->data)
        poco_warning(logger(), "Empty image. Check the given file name. ");

    notifyOutPortReady(imgOutPort, attr);
}

std::string CameraFromFiles::getStrParameterValue(size_t paramIndex)
{
    Poco::RWLock::ScopedReadLock lock(dataLock);
    switch (paramIndex)
    {
    case paramDirectory:
        return imgDir.toString();
    case paramFiles:
        return Poco::cat(std::string("\n"), imgPaths.begin(), imgPaths.end());
    case paramForceGrayscale:
        if (forceGrayscale)
            return "ON";
        else
            return "OFF";
    default:
        poco_bugcheck_msg("getStrParameterValue: wrong index");
        throw Poco::BugcheckException();
    }
}

void CameraFromFiles::setStrParameterValue(size_t paramIndex, std::string value)
{
    Poco::RWLock::ScopedWriteLock lock(dataLock);

    switch (paramIndex)
    {
    case paramDirectory:
        imgDir = value;
        imgDir.makeAbsolute();
        break;
    case paramFiles:
    {
        Poco::StringTokenizer tok(value, "\n",
                Poco::StringTokenizer::TOK_IGNORE_EMPTY | Poco::StringTokenizer::TOK_TRIM);
        imgPaths.assign(tok.begin(), tok.end());
        currentImgPath = imgPaths.begin();
        break;
    }
    case paramForceGrayscale:
        if (Poco::icompare(value, "ON") == 0)
            forceGrayscale = true;
        else if (Poco::icompare(value, "OFF") == 0)
            forceGrayscale = false;
        else
            throw Poco::DataFormatException("setParameterValue",
                    value + ": forceGrayscale can only be set to ON or OFF");
        break;
    default:
        poco_bugcheck_msg("setStrParameterValue: wrong index");
        throw Poco::BugcheckException();
    }
}

#endif /* HAVE_OPENCV */
