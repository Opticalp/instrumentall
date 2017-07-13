/**
 * @file	src/modules/control/SeqAccumulatorFactory.h
 * @date	Jul. 2017
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

#ifndef SRC_MODULES_CONTROL_SEQACCUMULATORFACTORY_H_
#define SRC_MODULES_CONTROL_SEQACCUMULATORFACTORY_H_

#include "core/ModuleFactoryBranch.h"

/**
 * SeqAccumulatorFactory
 *
 * Select the data type of the array elements to be accumulated/stacked
 */
class SeqAccumulatorFactory: public ModuleFactoryBranch
{
public:
	SeqAccumulatorFactory(ModuleFactory* parent, std::string selector):
	        ModuleFactoryBranch(parent, selector, false) { setLogger(name()); }

    std::string name() { return "SeqAccumulatorFactory"; }
    std::string description()
        { return "Module factory to transform a data sequence "
                "into an array"; }

    std::string selectDescription()
        { return "Set the input elements type"; }

    std::vector<std::string> selectValueList();

private:
    ModuleFactoryBranch* newChildFactory(std::string selector);

};

#endif /* SRC_MODULES_CONTROL_SEQACCUMULATORFACTORY_H_ */
