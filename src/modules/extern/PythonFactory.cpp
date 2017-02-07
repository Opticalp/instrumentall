/**
 * @file	src/modules/extern/PythonFactory.cpp
 * @date	Feb. 2017
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

#ifdef HAVE_PYTHON27

#include "PythonFactory.h"

#include "modules/GenericLeafFactory.h"
#include "modules/extern/PythonModule.h"

#include "Poco/RegularExpression.h"

std::vector<std::string> PythonFactory::selectValueList()
{
    std::vector<std::string> list;
    list.push_back("");
    return list;
}

ModuleFactoryBranch* PythonFactory::newChildFactory(std::string selector)
{
    return new GenericLeafFactory<PythonModule>(
            "PyModWithPortsFactory",
            "Factory to create modules that launch a python script "
            "given the ports in selector",
            this, selector);
}

std::string PythonFactory::validateSelector(std::string selector)
{
    if (selector.empty())
        return "";

    Poco::RegularExpression regex("^\\w+(\\s*;\\s*\\w+)*$");
    if (!regex.match(selector))
        throw Poco::InvalidArgumentException("Unrecognized selector: " + selector);
    else
        return selector;
}

#endif /* HAVE_PYTHON27 */
