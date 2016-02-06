/**
 * @file	src/DemoBranchFactory.cpp
 * @date	jan. 2016
 * @author	PhRG - opticalp.fr
 *
 * $Id$
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

#include "DemoBranchFactory.h"
#include "DemoLeafFactory.h"

ModuleFactoryBranch* DemoBranchFactory::newChildFactory(std::string selector)
{
    return new DemoLeafFactory(this, selector);
}

std::string DemoBranchFactory::validateSelector(std::string selector)
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
