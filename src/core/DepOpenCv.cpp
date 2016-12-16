/**
 * @file	src/core/DepOpenCv.cpp
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

#include "DepOpenCv.h"
#include "version.h"
#include "opencv2/opencv.hpp"

std::string DepOpenCv::name()
{
    return "OpenCV";
}

std::string DepOpenCv::description()
{
    return "OpenCV (Open Source Computer Vision Library) is an open source "
            "computer vision and machine learning software library. OpenCV "
            "was built to provide a common infrastructure for computer vision "
            "applications and to accelerate the use of machine perception in "
            "the commercial products. ";
}

std::string DepOpenCv::URL()
{
    return "http://opencv.org/";
}

std::string DepOpenCv::license()
{
    return "License Agreement for Open Source Computer Vision Library (3-clause BSD License)\n"
            "Copyright (C) 2000-2016, Intel Corporation,\n"
            "Copyright (C) 2009-2011, Willow Garage Inc.,\n"
            "Copyright (C) 2009-2016, NVIDIA Corporation,\n"
            "Copyright (C) 2010-2013, Advanced Micro Devices, Inc., \n"
            "Copyright (C) 2015-2016, OpenCV Foundation, \n"
            "Copyright (C) 2015-2016, Itseez Inc.";
}

std::string DepOpenCv::buildTimeVersion()
{
    return OPENCV_VERSION;
}

#include "Poco/StringTokenizer.h"

std::string DepOpenCv::runTimeVersion()
{
	std::string full(cv::getBuildInformation());

	if (full.empty())
		return "";

	Poco::StringTokenizer tok(full, "\n");
    return tok[1];
}

#endif /* HAVE_OPENCV */
