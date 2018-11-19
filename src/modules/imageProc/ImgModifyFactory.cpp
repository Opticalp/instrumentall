/**
 * @file	src/modules/imageProc/ImgModifyFactory.cpp
 * @date	Jun 2018
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

#include "ImgModifyFactory.h"

#include "modules/GenericLeafFactory.h"
#include "RotCrop.h"
#include "ImgPrintInfo.h"
#include "DistToOrigin.h"

std::vector<std::string> ImgModifyFactory::selectValueList()
{
    std::vector<std::string> list;

#ifdef HAVE_OPENCV
	list.push_back("rotCrop");
	list.push_back("printInfo");
	list.push_back("distToOrigin");
#endif

    return list;
}

ModuleFactoryBranch* ImgModifyFactory::newChildFactory(std::string selector)
{
#ifdef HAVE_OPENCV
	if (selector.compare("rotCrop") == 0)
	{
		return new GenericLeafFactory<RotCrop>("RotCropFactory",
			"Build module that rotate and crop the input image",
			this, selector);
	}
	else if (selector.compare("printInfo") == 0)
    {
        return new GenericLeafFactory<ImgPrintInfo>("ImgPrintInfoFactory",
            "Build module that displays infos on the input image",
            this, selector);
    }
	else if (selector.compare("distToOrigin") == 0)
	{
		return new GenericLeafFactory<DistToOrigin>("DistToOriginFactory",
			"Build module that displays visual infos (vector showing\n "
			" the distance to a given reference) on the input image",
			this, selector);
	}
    else
#endif /* HAVE_OPENCV */
	{
		poco_bugcheck_msg("Create: unknown selector");
		throw Poco::BugcheckException();
	}
}
