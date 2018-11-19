/**
 * @file	src/modules/imageProc/ThresholdSpecifiedFactory.cpp
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

#include "ThresholdSpecifiedFactory.h"

#include "modules/GenericLeafFactory.h"
#include "ThresAbs.h"
#include "ThresPop.h"
#include "ThresMean.h"

ThresholdSpecifiedFactory::ThresholdSpecifiedFactory(ModuleFactory* parent,
        std::string selector):
                ModuleFactoryBranch(parent, selector)
{
    if (selector.compare("population")==0)
    {
        mType = populationType;
    }
	else if (selector.compare("absolute")==0)
    {
        mType = absoluteType;
    }
    else
    {
        poco_assert(selector.compare("mean")==0);
        mType = meanType;
    }

    setLogger(name());
}


std::string ThresholdSpecifiedFactory::name()
{
    switch (mType)
    {
    case absoluteType:
        return "ThresAbsFactory";
    case populationType:
    	return "ThresPopFactory";
	case meanType:
    	return "ThresMeanFactory";
    default:
        poco_bugcheck_msg("impossible threshold factory type");
        throw Poco::BugcheckException();
    }
}

std::string ThresholdSpecifiedFactory::description()
{
    switch (mType)
    {
    case absoluteType:
        return "Factory to build thresholding modules "
                "based on absolute values";
    case populationType:
    	return "Factory to build thresholding modules "
    			"based on population values";
	case meanType:
    	return "Factory to build thresholding modules "
    			"based on mean values";
    default:
        poco_bugcheck_msg("impossible threshold factory type");
        throw Poco::BugcheckException();
    }
}

Module* ThresholdSpecifiedFactory::newChildModule(
        std::string customName)
{
	switch (mType)
	{
	case absoluteType:
		return new ThresAbs(this, customName);
	case populationType:
		return new ThresPop(this, customName);
	case meanType:
		return new ThresMean(this, customName);
	default:
		poco_bugcheck_msg("impossible thresholder type");
		throw Poco::BugcheckException();
	}
}

#endif /* HAVE_OPENCV */
