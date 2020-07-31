/**
 * @file    src/Modules/imageProc/ImgPrintInfo.cpp
 * @date    Jun 2018
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

#include "ImgPrintInfo.h"

#include <opencv2/imgproc/imgproc.hpp>

#include "Poco/NumberFormatter.h"
#include "Poco/String.h" // icompare

size_t ImgPrintInfo::refCount = 0;

ImgPrintInfo::ImgPrintInfo(ModuleFactory* parent, std::string customName):
            Module(parent, customName)
{
    if (refCount)
         setInternalName("ImgPrintInfo" + Poco::NumberFormatter::format(refCount));
     else
         setInternalName("ImgPrintInfo");

     setCustomName(customName);
     setLogger("module." + name());

     // parameters
     setParameterCount(paramCnt);
     addParameter(paramXpos, "xPos",
             "x-coord text position",
             ParamItem::typeInteger, "10");
     addParameter(paramYpos, "yPos",
             "y-coord text position",
             ParamItem::typeInteger, "40");
     addParameter(paramTitle, "title",
             "Title of the displayed value",
             ParamItem::typeString, "");
     addParameter(paramColorMode, "colorMode",
             "One of: gray, red, green, blue. \n"
             "If a color is given, the output image will be in color",
             ParamItem::typeString, "gray");
     addParameter(paramColorLevel, "level",
             "Color level (0 is black, 255 is light",
             ParamItem::typeInteger, "255");

     setParametersDefaultValue();

     // ports
     setInPortCount(inPortCnt);
     setOutPortCount(outPortCnt);

     addInPort("image", "image to be annotated", DataItem::typeCvMat, imageInPort);
     addInPort("integer", "Integer value to display", DataItem::typeInt64, intValueInPort);
     addInPort("float", "float value to display", DataItem::typeFloat, floatValueInPort);

     addOutPort("image", "Image with caption", DataItem::typeCvMat, imageOutPort);

     setParametersDefaultValue();

     notifyCreation();

     // if nothing failed
     refCount++;
}

Poco::Int64 ImgPrintInfo::getIntParameterValue(size_t paramIndex)
{
    switch (paramIndex)
    {
    case paramXpos:
        return xPos;
    case paramYpos:
        return yPos;
    case paramColorLevel:
        return colorLevel;
    default:
        poco_bugcheck_msg("unknown parameter index");
        throw Poco::BugcheckException();
    }
}

void ImgPrintInfo::setIntParameterValue(size_t paramIndex, Poco::Int64 value)
{
    switch (paramIndex)
    {
    case paramXpos:
        xPos = static_cast<int>(value);
        break;
    case paramYpos:
        yPos = static_cast<int>(value);
        break;
    case paramColorLevel:
        if (value < 0 || value > 255)
            throw Poco::RangeException("setParameterValue",
                    "colorLevel shall be in [0 255]");
        colorLevel = static_cast<int>(value);
        break;
    default:
        poco_bugcheck_msg("unknown parameter index");
    }
}

std::string ImgPrintInfo::getStrParameterValue(size_t paramIndex)
{
    switch (paramIndex)
    {
    case paramTitle:
        return title;
    case paramColorMode:
        switch (color)
        {
        case colorGray:
            return "gray";
        case colorRed:
            return "red";
        case colorGreen:
            return "green";
        case colorBlue:
            return "blue";
        default:
            poco_bugcheck_msg("unknown color mode");
            throw Poco::BugcheckException();
        }
        break;
    default:
        poco_bugcheck_msg("unknown parameter index");
        throw Poco::BugcheckException();
    }
}

void ImgPrintInfo::setStrParameterValue(size_t paramIndex, std::string value)
{
    switch (paramIndex)
    {
    case paramTitle:
        title = value;
        break;
    case paramColorMode:
        if (Poco::icompare(value, "gray") == 0)
            color = colorGray;
        else if (Poco::icompare(value, "red") == 0)
            color = colorRed;
        else if (Poco::icompare(value, "green") == 0)
            color = colorGreen;
        else if (Poco::icompare(value, "blue") == 0)
            color = colorBlue;
        else
            throw Poco::InvalidArgumentException("setParameterValue",
                    "colorMode shall be one of: gray, red, green, blue");
        break;
    default:
        poco_bugcheck_msg("unknown parameter index");
    }
}

void ImgPrintInfo::process(int startCond)
{
    if (startCond != allPluggedDataStartState)
    {
        poco_error(logger(), name() + ": wrong input data. Exiting. "
                "Please unbind one of the input values. ");
        return;
    }

    if (!isInPortCaught(imageInPort))
    {
        poco_error(logger(), "No image present, exiting. "
                "Please plug the image input");
        return;
    }

    DataAttributeIn attr;

    std::string msg;

    bool hasDble = isInPortCaught(floatValueInPort);
    bool hasInt = isInPortCaught(intValueInPort);

    if (!hasDble && !hasInt)
    {
        poco_notice(logger(), "no value plugged on the input ports");
        msg = title;
    }
    else if (!title.empty())
    {
        msg = title + ": ";
    }

    if (hasDble)
    {
        float* pValue;
        readLockInPort(floatValueInPort);
        readInPortData<float>(floatValueInPort, pValue);
        msg += Poco::NumberFormatter::format(*pValue);
    }
    else if (hasInt)
    {
        Poco::Int64* pValue;
        readLockInPort(intValueInPort);
        readInPortData<Poco::Int64>(intValueInPort, pValue);
        msg += Poco::NumberFormatter::format(*pValue);
    }

    cv::Mat* pData;

    readLockInPort(imageInPort);
    readInPortData<cv::Mat>(imageInPort, pData);
    readInPortDataAttribute(imageInPort, &attr);

    DataAttributeOut outAttr = attr;

    cv::Mat workingImg;

    if ((color != colorGray) && pData->channels() < 3)
        cv::cvtColor(*pData, workingImg, cv::COLOR_GRAY2BGR);
    else
        pData->copyTo(workingImg);

    releaseInPort(imageInPort);
    reserveOutPort(imageOutPort);

    cv::Scalar fillCol;

    switch (color)
    {
    case colorGray:
        fillCol = CV_RGB(colorLevel,colorLevel,colorLevel);
        break;
    case colorRed:
        fillCol = CV_RGB(colorLevel,0,0);
        break;
    case colorGreen:
        fillCol = CV_RGB(0,colorLevel,0);
        break;
    case colorBlue:
        fillCol = CV_RGB(0,0,colorLevel);
        break;
    }

    int font = cv::FONT_HERSHEY_DUPLEX;
    poco_information(logger(), "displaying: <" + msg + ">");
    cv::putText(workingImg, msg, cv::Point(xPos, yPos),
            font, 1, // font, font size
            fillCol, // color
            1, cv::LINE_AA); // thickness, line type

    processingTerminated();

    getDataToWrite<cv::Mat>(imageOutPort, pData);

    *pData = workingImg;

    notifyOutPortReady(imageOutPort, outAttr);
}

#endif /* HAVE_OPENCV */
