/**
 * @file	src/DemoLeafFactory.cpp
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

#include "DemoLeafFactory.h"
#include "DemoModule.h"
#include "DemoModuleA.h"
#include "DemoModuleB.h"
#include "DemoModuleDataSeq.h"
#include "DemoModuleSeqAccu.h"
#include "DemoModuleSeqMax.h"
#include "DemoModuleForwarder.h"
#include "DemoModuleParam.h"
#include "DemoModuleTwoInputs.h"

Module* DemoLeafFactory::newChildModule(std::string customName)
{
    if (getSelector().compare("leaf")==0)
    {
        poco_trace(logger(), name() + " is logging trace");
        poco_debug(logger(), name() + " is logging debug");
        poco_information(logger(), name() + " is logging info");
        poco_notice(logger(), name() + " is logging notice");
        poco_warning(logger(), name() + " is logging warning");
        poco_error(logger(), name() + " is logging error");
        poco_critical(logger(), name() + " is logging critical");
        poco_fatal(logger(), name() + " is logging fatal");

        return new DemoModule(this, customName);
    }

    if (getSelector().compare("leafA")==0)
        return new DemoModuleA(this, customName);

    if (getSelector().compare("leafB")==0)
        return new DemoModuleB(this, customName);

    if (getSelector().compare("leafDataSeq")==0)
        return new DemoModuleDataSeq(this, customName);

    if (getSelector().compare("leafSeqAccu")==0)
        return new DemoModuleSeqAccu(this, customName);

    if (getSelector().compare("leafSeqMax")==0)
        return new DemoModuleSeqMax(this, customName);

    if (getSelector().compare("leafForwarder")==0)
        return new DemoModuleForwarder(this, customName);

    if (getSelector().compare("leafParam")==0)
        return new DemoModuleParam(this, customName);

    if (getSelector().compare("leafTwoInputs")==0)
    	return new DemoModuleTwoInputs(this, customName);

    throw Poco::InvalidArgumentException("newChildModule","Impossible selector value");
}
