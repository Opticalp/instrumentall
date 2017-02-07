/**
 * @file	src/modules/extern/PythonFactory.h
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

#ifndef SRC_MODULES_EXTERN_PYTHONFACTORY_H_
#define SRC_MODULES_EXTERN_PYTHONFACTORY_H_

#ifdef HAVE_PYTHON27

#include "core/ModuleFactoryBranch.h"

/**
 * PythonFactory
 * 
 * Intermediate factory to create python modules.
 * The trig ports will be selected here.
 */
class PythonFactory: public ModuleFactoryBranch
{
public:
    PythonFactory(ModuleFactory* parent, std::string selector):
        ModuleFactoryBranch(parent, selector, false) { setLogger(name()); }

    std::string name() { return "PythonFactory"; }
    std::string description()
    {
        return "Intermediate (branch) factory to create python modules. "
                "the trig ports will be selected here. ";
    }

    std::string selectDescription()
    {
        return "trig ports described as trig port names separated by semicolons, "
                "e.g. \"trigA;trigB\". The names are trimmed "
                "so that you can write: \"trigA ; trigB\" ";
    }

    std::vector<std::string> selectValueList();

private:
    ModuleFactoryBranch* newChildFactory(std::string selector);

    /**
     * Check that the selector is a semicolon separated token of alpha num chars
     */
    std::string validateSelector(std::string selector);
};

#endif /* HAVE_PYTHON27 */
#endif /* SRC_MODULES_EXTERN_PYTHONFACTORY_H_ */
