/**
 * @file	src/EmptyModuleFactory.h
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

#ifndef SRC_EMPTYMODULEFACTORY_H_
#define SRC_EMPTYMODULEFACTORY_H_

#include "core/ModuleFactory.h"

/**
 * EmptyModuleFactory
 *
 * This module factory just throws exception.
 * It has to be used in place of a previous module factory
 * when this one expired.
 *
 * @see ModuleManager::allFactories
 * @see ModuleManager::removeFactory
 */
class EmptyModuleFactory: public ModuleFactory
{
public:
    EmptyModuleFactory() { }

    std::string name() { return "emptyFactory"; }
    std::string description() { return "replaces an expired factory. "; }

    std::string selectDescription()
    {
        throw Poco::NotImplementedException("selectDescription()",
                "Empty factory ");
    }

    std::vector<std::string> selectValueList()
    {
        throw Poco::NotImplementedException("selectValueList()",
                "Empty factory ");
    }

    size_t countRemain()
    {
        throw Poco::NotImplementedException("countRemain()",
                "Empty factory ");
    }
};

#endif /* SRC_EMPTYMODULEFACTORY_H_ */
