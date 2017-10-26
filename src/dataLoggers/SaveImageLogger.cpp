/**
 * @file	src/dataLoggers/SaveImageLogger.cpp
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

#include "SaveImageLogger.h"
#include "core/DataItem.h"

#include "Poco/NumberFormatter.h"

#include "opencv2/opencv.hpp"

size_t SaveImageLogger::refCount = 0;

SaveImageLogger::SaveImageLogger():
        DataLogger("SaveImageLogger"),
        nextIndex(1), extension(".png"),
        prefix("img_"), digits(2)
{
    setName(refCount);

    setParameterCount(paramCnt);
    addParameter(paramDirectory, "directory", "Directory in which to store the images",
            ParamItem::typeString, "");
    addParameter(paramPrefix, "prefix", "Prefix to be used to build the image file name",
            ParamItem::typeString, "img_");
    addParameter(paramDigits, "digits", "Count of digits to be used to build the image file name. "
            "Those digits will follow the prefix", ParamItem::typeInteger, "2");
    addParameter(paramExtension, "extension", "Image file extension, including the dot. E.g. \".png\"",
            ParamItem::typeString, ".png");
    addParameter(paramNextIndex, "nextIndex", "Next index to be used to generate the image file name",
            ParamItem::typeInteger, "1");

    setStrParameterValue(paramDirectory, getStrParameterDefaultValue(paramDirectory));
    setStrParameterValue(paramPrefix, getStrParameterDefaultValue(paramPrefix));
    setIntParameterValue(paramDigits, getIntParameterDefaultValue(paramDigits));
    setStrParameterValue(paramExtension, getStrParameterDefaultValue(paramExtension));
    setIntParameterValue(paramNextIndex, getIntParameterDefaultValue(paramNextIndex));

    refCount++;
}

void SaveImageLogger::log()
{
    cv::Mat img = *(getDataSource()->getData<cv::Mat>());

    if (img.data)
    {
        Poco::Path imgPath(directory);

        if (digits)
        {
            std::string fullIndex = Poco::NumberFormatter::format0(nextIndex, digits);
            std::string index(fullIndex.end()-digits, fullIndex.end()); // cheap modulo
            imgPath.append(prefix + index + extension);
        }
        else
        {
            imgPath.append(prefix + extension);
        }

        if (img.type()!=CV_8U && img.type()!=CV_16U)
        {
            double min,max;
            cv::minMaxLoc(img,&min,&max);

            cv::Mat tmpImg; // temporary image
            img.convertTo(
                    tmpImg,      // output image
                    CV_8U,       // depth
                    255.0/max ); // scale factor

            // save image
            cv::imwrite(imgPath.toString(), tmpImg);
        }
        else
        {
            cv::imwrite(imgPath.toString(), img);
        }

        // increment parameters
        nextIndex++;
    }
    else
    {
        throw Poco::RuntimeException(name(),"empty image, nothing to save");
    }
}

bool SaveImageLogger::isSupportedInputDataType(int datatype)
{
    return (datatype == (DataItem::typeCvMat | DataItem::contScalar));
}

std::set<int> SaveImageLogger::supportedInputDataType()
{
    std::set<int> ret;
    ret.insert(DataItem::typeCvMat);
    return ret;
}

Poco::Int64 SaveImageLogger::getIntParameterValue(size_t paramIndex)
{
    switch (paramIndex)
    {
    case paramDigits:
        return digits;
    case paramNextIndex:
        return nextIndex;
    default:
        poco_bugcheck_msg("wrong parameter index");
        throw Poco::BugcheckException();
    }
}

std::string SaveImageLogger::getStrParameterValue(size_t paramIndex)
{
    switch (paramIndex)
    {
    case paramDirectory:
        return directory.toString();
    case paramPrefix:
        return prefix;
    case paramExtension:
        return extension;
    default:
        poco_bugcheck_msg("wrong parameter index");
        throw Poco::BugcheckException();
    }
}

void SaveImageLogger::setIntParameterValue(size_t paramIndex, Poco::Int64 value)
{
    switch (paramIndex)
    {
    case paramDigits:
        if (value < 0)
            throw Poco::RangeException("setParameterValue",
                    "parameter digits has to be positive");
        digits = static_cast<int>(value);
        break;
    case paramNextIndex:
        nextIndex = value;
        break;
    default:
        poco_bugcheck_msg("wrong parameter index");
        throw Poco::BugcheckException();
    }
}

void SaveImageLogger::setStrParameterValue(size_t paramIndex, std::string value)
{
    switch (paramIndex)
    {
    case paramDirectory:
        directory = value;
        directory.makeAbsolute();
        break;
    case paramPrefix:
        prefix = value;
        break;
    case paramExtension:
        extension = value;
        break;
    default:
        poco_bugcheck_msg("wrong parameter index");
        throw Poco::BugcheckException();
    }
}

#endif /* HAVE_OPENCV */
