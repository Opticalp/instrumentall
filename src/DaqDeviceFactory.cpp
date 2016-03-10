/**
 * @file	src/DaqDeviceFactory.cpp
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

#include "DaqDeviceFactory.h"


std::vector<std::string> DaqDeviceFactory::selectValueList()
{
    std::vector<std::string> list;
    // list.push_back("NiDaq");
    return list;
}

ModuleFactoryBranch* DaqDeviceFactory::newChildFactory(
		std::string selector)
{
//  if (selector.compare("NiDaq") == 0)
//  {
//      return new NiDaqFactory(this, selector);
//  }
//  else
    {
        poco_bugcheck_msg("Create: unknown selector");
        throw Poco::BugcheckException();
    }
}

std::string DaqDeviceFactory::validateSelector(std::string selector)
{
    std::vector<std::string> selectors = selectValueList();

    for (std::vector<std::string>::iterator it=selectors.begin(),ite=selectors.end();
            it != ite; it++)
    {
        if (selector.compare(*it)==0)
            return selector;
    }

    throw ModuleFactoryException("Unrecognized selector: " + selector);
}