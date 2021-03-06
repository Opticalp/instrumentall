/**
 * @file	src/modules/demo/DemoModule.cpp
 * @date	jan. 2016
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

#include "Poco/NumberFormatter.h"

#include "DemoModule.h"

size_t DemoModule::refCount = 0;

DemoModule::DemoModule(ModuleFactory* parent, std::string customName):
        Module(parent, customName)
{
    // poco_information(logger(),"Creating a new demo module");

    if (refCount)
        setInternalName("DemoModule" + Poco::NumberFormatter::format(refCount));
    else
        setInternalName("DemoModule");

    setCustomName(customName);

    setLogger("module." + name());

    poco_trace(logger(), name() + "'s logger is set (trace)");
    poco_debug(logger(), name() + "'s logger is set (debug)");
    poco_information(logger(), name() + "'s logger is set (info)");
    poco_notice(logger(), name() + "'s logger is set (notice)");
    poco_warning(logger(), name() + "'s logger is set (warning)");
    poco_error(logger(), name() + "'s logger is set (error)");
    poco_critical(logger(), name() + "'s logger is set (critical)");
    poco_fatal(logger(), name() + "'s logger is set (fatal)");

    notifyCreation();

    // if nothing failed
    refCount++;
}
