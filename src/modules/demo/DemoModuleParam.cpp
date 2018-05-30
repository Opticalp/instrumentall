/**
 * @file	src/modules/demo/DemoModuleParam.cpp
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

#include "DemoModuleParam.h"

#include "Poco/NumberFormatter.h"

size_t DemoModuleParam::refCount = 0;

DemoModuleParam::DemoModuleParam(ModuleFactory* parent, std::string customName):
    Module(parent, customName), iPar(421), fPar(3.14), sPar("Caramba")
{
    if (refCount)
        setInternalName("DemoModuleParam" + Poco::NumberFormatter::format(refCount));
    else
        setInternalName("DemoModuleParam");

    setCustomName(customName);
    setLogger("module." + name());

    // parameters
    setParameterCount(paramCnt);
    addParameter(paramInt, "intParam", "simple integer parameter", ParamItem::typeInteger, "1515");
    addParameter(paramFloat, "floatParam", "simple floating point parameter", ParamItem::typeFloat);
    addParameter(paramStr, "strParam", "simple character string parameter", ParamItem::typeString);

    // test the default values
    setIntParameterValue(paramInt, getIntParameterDefaultValue(paramInt));

    // another (global) way to use it
    setParametersDefaultValue();

    try
    {
        getFloatParameterDefaultValue(paramFloat);
        poco_bugcheck_msg("should have caught an exception...");
    }
    catch (Poco::NotFoundException& e)
    {
        poco_information(logger(),"ok to catch: " + e.displayText());
    }

    notifyCreation();

    // if nothing failed
    refCount++;
}


