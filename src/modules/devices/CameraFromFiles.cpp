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
            Module(parent, customName)
{
    setInternalName("CameraFromFiles" + Poco::NumberFormatter::format(refCount));
    setCustomName(customName);
    setLogger("module." + name());

    // parameters
    setParameterCount(paramCnt);
    addParameter(paramDirectory, "directory",
            "Base directory for all the files given in the files parameter",
            ParamItem::typeString, ""); // default is empty: relative path
    addParameter(paramFiles, "files",
            "CR-separated list of files to be used as image sources",
            ParamItem::typeString);
    currentImgPath = imgPaths.begin();
    addParameter(paramDataType, "dataType", "image data type in OpenCV notation (\"CV_8U\","
            "\"CV_8UC3\", \"CV_64FC3\"...)", ParamItem::typeString, "CV_8U");

    setStrParameterValue(paramDataType, getStrParameterDefaultValue(paramDataType));

    // ports
    setInPortCount(inPortCnt);
    setOutPortCount(outPortCnt);

//    addInPort("inPortA", "data in", DataItem::typeInteger, inPortA);
    addTrigPort("trig", "Launch the image generation", trigPort);

    addOutPort("acqReady", "acquisition ready trigger", DataItem::typeInteger, acqReadyOutPort);
    addOutPort("image", "image from file", DataItem::typeCvMat, imgOutPort);

    notifyCreation();

    // if nothing failed
    refCount++;
}

void CameraFromFiles::process(int startCond)
{
    poco_information(logger(), name() + " processing...");
}

std::string CameraFromFiles::getStrParameterValue(size_t paramIndex)
{
    switch (paramIndex)
    {
    case paramDirectory:
        return imgDir.toString();
    case paramFiles:
        return Poco::cat(std::string("\n"), imgPaths.begin(), imgPaths.end());
    case paramDataType:
        switch (imgDataType)
        {
        case CV_8UC3:
            return "CV_8UC3";
        case CV_16UC3:
            return "CV_16UC3";
        case CV_64FC3:
            return "CV_64FC3";
        case CV_16U:
            return "CV_16U";
        case CV_64F:
            return "CV_64F";
        case CV_8U:
        default:
            return "CV_8U";
        }
        break;
    default:
        poco_bugcheck_msg("getStrParameterValue: wrong index");
        throw Poco::BugcheckException();
    }
}

void CameraFromFiles::setStrParameterValue(size_t paramIndex, std::string value)
{
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
        break;
    }
    case paramDataType:
        if ((value.compare("CV_8UC1") == 0) || (value.compare("CV_8U") == 0))
            imgDataType = CV_8U;
        else if ((value.compare("CV_16UC1") == 0) || (value.compare("CV_16U") == 0))
            imgDataType = CV_16U;
        else if ((value.compare("CV_64FC1") == 0) || (value.compare("CV_64F") == 0))
            imgDataType = CV_64F;
        else if (value.compare("CV_8UC3") == 0)
            imgDataType = CV_8UC3;
        else if (value.compare("CV_16UC3") == 0)
            imgDataType = CV_16UC3;
        else if (value.compare("CV_64FC3") == 0)
            imgDataType = CV_64FC3;
        else
            throw Poco::DataFormatException("setParameterValue",
                    value + ": Unrecognized image data format");
        break;
    default:
        poco_bugcheck_msg("setStrParameterValue: wrong index");
        throw Poco::BugcheckException();
    }
}

#endif /* HAVE_OPENCV */
