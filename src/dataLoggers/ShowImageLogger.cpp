/**
 * @file	src/dataLoggers/ShowImageLogger.cpp
 * @date	Apr. 2016
 * @author	PhRG - opticalp.fr
 */

/*
 Copyright (c) 2016 Ph. Renaud-Goud / Opticalp

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

#include "ShowImageLogger.h"

#include "core/DataItem.h"

#include "UI/GuiManager.h"
#include "UI/GUI/GuiProcessingUnit.h"

#include "Poco/NumberFormatter.h"

#include "opencv2/opencv.hpp"

#if CV_MAJOR_VERSION == 3
#include <opencv2/highgui/highgui.hpp>
#endif


size_t ShowImageLogger::refCount = 0;
size_t ShowImageLogger::winCount = 0;

ShowImageLogger::ShowImageLogger():
        DataLogger("ShowImageLogger"),
		imagePanelIndex(0)
{
    setName(refCount);

    setParameterCount(paramCnt);
    addParameter(paramImagePanel, "imagePanel",
    		"Index of the image panel (GUI) on which "
    		"to display the image", ParamItem::typeInteger, "0");

    setIntParameterValue(paramImagePanel, getIntParameterDefaultValue(paramImagePanel));

    refCount++;
}

void ShowImageLogger::log()
{
    cv::Mat img = *(getDataSource()->getData<cv::Mat>());

    if (img.data)
    {
#ifdef HAVE_WXWIDGETS
        // try gui mode
        GuiProcessingUnit* guiProc =
                Poco::Util::Application::instance()
                    .getSubsystem<GuiManager>()
                    .getGuiProcUnit();
        if (guiProc != NULL)
        {
            guiProc->showImage(img, imagePanelIndex);
        }
        else
#endif /* HAVE_WXWIDGETS */
        {
            std::string winName = name() + Poco::NumberFormatter::format(winCount++);
            // create a window for display.
            cv::namedWindow( winName, cv::WINDOW_AUTOSIZE );
            // Show our image inside it.
            cv::imshow( winName, img );

            // take time to effectively display the image
            cv::waitKey(100);
        }
    }
    else
    {
        throw Poco::RuntimeException(name(),"empty image, nothing to show");
    }
}

std::set<int> ShowImageLogger::supportedInputDataType()
{
    std::set<int> ret;
    ret.insert(DataItem::typeCvMat);
    return ret;
}

Poco::Int64 ShowImageLogger::getIntParameterValue(size_t paramIndex)
{
    switch (paramIndex)
    {
    case paramImagePanel:
        return imagePanelIndex;
    default:
        poco_bugcheck_msg("wrong parameter index");
        throw Poco::BugcheckException();
    }
}

void ShowImageLogger::setIntParameterValue(size_t paramIndex, Poco::Int64 value)
{
    switch (paramIndex)
    {
    case paramImagePanel:
        if (value < 0)
            throw Poco::RangeException("setParameterValue",
                    "parameter imagePanel has to be positive");
        imagePanelIndex = static_cast<int>(value);
        break;
    default:
        poco_bugcheck_msg("wrong parameter index");
        throw Poco::BugcheckException();
    }
}


#endif /* HAVE_OPENCV */
