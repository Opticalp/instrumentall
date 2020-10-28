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
#include "Poco/String.h"

#include "opencv2/opencv.hpp"

size_t SaveImageLogger::refCount = 0;

SaveImageLogger::SaveImageLogger():
        DataLogger("SaveImageLogger"),
        nextIndex(1), extension(".png"),
        prefix("img_"), digits(2), normalize(normDef)
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
	addParameter(paramNormalize, "normalization", "Img normalization method: "
			"available methods are \"min\", \"max\", \"none\", \"default\". \n"
			"default is max for float image, none for uchar8 image. ",
		ParamItem::typeString, "default");


    setStrParameterValue(paramDirectory, getStrParameterDefaultValue(paramDirectory));
    setStrParameterValue(paramPrefix, getStrParameterDefaultValue(paramPrefix));
    setIntParameterValue(paramDigits, getIntParameterDefaultValue(paramDigits));
    setStrParameterValue(paramExtension, getStrParameterDefaultValue(paramExtension));
    setIntParameterValue(paramNextIndex, getIntParameterDefaultValue(paramNextIndex));
	setStrParameterValue(paramNormalize, getStrParameterDefaultValue(paramNormalize));

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

        if (img.type() == CV_8U && (normalize == normDef || normalize == normNone))
    		cv::imwrite(imgPath.toString(), img);
        else
        {
			double offset = 0, scaleFactor;
			cv::Mat tmpImg; // temporary image

			if (img.type() == CV_16U)
				scaleFactor = 1./255;
			else
				scaleFactor = 255;

			if (normalize != normNone)
			{
				int norm;
				if (normalize == normDef)
					norm = normMax;
				else
					norm = normalize;

				double min,max;
				cv::minMaxLoc(img,&min,&max);

				if ((norm & normMin) == 0)
					min = 0;

				if (norm & normMax)
				{
					if (max-min)
						scaleFactor = 255.0 / (max-min);
					else if (max)
						scaleFactor = 255.0 / max;
				}

				offset = -min * scaleFactor;
			}

			img.convertTo(
					tmpImg,      // output image
					CV_8U,       // depth
					scaleFactor, // scale factor
					offset); // offset (after scaling)

			// save image
			cv::imwrite(imgPath.toString(), tmpImg);
        }

        // increment parameters
        nextIndex++;
    }
    else
    {
        throw Poco::RuntimeException(name(),"empty image, nothing to save");
    }
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
	case paramNormalize:
		switch (normalize)
		{
		case normNone:
			return "none";
		case normDef:
			return "default";
		default:
		{
			std::string ret;
			if (normalize & normMin)
				ret = "min";
			if (normalize & normMax)
				ret += "max";
			return ret;
		}
		}
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
	case paramNormalize:
	{
		int newNorm = 0;
		if (Poco::toLower(value).find("max") != std::string::npos)
			newNorm += normMax;
		if (Poco::toLower(value).find("min") != std::string::npos)
			newNorm += normMin;
		if (Poco::toLower(value).find("default") != std::string::npos)
			newNorm = normDef; // override min or max

		if (newNorm || Poco::toLower(value).find("none") != std::string::npos)
		{
			normalize = newNorm;
			break;
		}

		throw Poco::InvalidArgumentException("setParameterValue",
				"normalize has to be \"min\" , \"max\", \"none\" or \"default\". ");
	}
    default:
        poco_bugcheck_msg("wrong parameter index");
        throw Poco::BugcheckException();
    }
}

#endif /* HAVE_OPENCV */
