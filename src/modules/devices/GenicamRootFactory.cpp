/**
 * @file	src/modules/devices/GenicamRootFactory.cpp
 * @date	Janv. 2017
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

#include "GenicamRootFactory.h"
#include "genicam/GenicamLibFactory.h"

#include "Poco/Path.h"
#include "Poco/File.h"

std::vector<std::string> GenicamRootFactory::selectValueList()
{
    std::vector<std::string> list;
    list.push_back(""); // empty string means: free choice.
    return list;
}

std::string GenicamRootFactory::validateSelector(std::string selector)
{
    // various checks on .cti
     Poco::Path libTlPath(selector);

     if (!libTlPath.isFile())
         throw Poco::InvalidArgumentException(libTlPath.toString()
                 + " is not a file. ");

     if (!Poco::File(libTlPath).exists()
             || !Poco::File(libTlPath).canRead())
         throw Poco::InvalidArgumentException("unable to read the TL lib: "
                 + libTlPath.toString() );

     return libTlPath.absolute().toString();
}

ModuleFactoryBranch* GenicamRootFactory::newChildFactory(std::string selector)
{
    return new GenicamLibFactory(this, selector);
}
