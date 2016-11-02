/**
 * @file	src/DeviceFactory.cpp
 * @date	Mar 2016
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

#include "DeviceFactory.h"

#include "DaqDeviceFactory.h"
#include "CameraFactory.h"

std::vector<std::string> DeviceFactory::selectValueList()
{
    std::vector<std::string> list;
    list.push_back("DAQ");
    list.push_back("camera");
    // list.push_back("motionStage");
    return list;
}

ModuleFactoryBranch* DeviceFactory::newChildFactory(std::string selector)
{
	if (selector.compare("DAQ") == 0)
	{
		return new DaqDeviceFactory(this, selector);
	}
	else if (selector.compare("camera") == 0)
	{
		return new CameraFactory(this, selector);
	}
//	else if (selector.compare("motionStage") == 0)
//	{
//		return new MotionStageFactory(this, selector);
//	}
	else
	{
		poco_bugcheck_msg("Create: unknown selector");
		throw Poco::BugcheckException();
	}
}
