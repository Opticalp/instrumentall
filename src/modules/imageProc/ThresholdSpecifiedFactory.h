/**
 * @file	src/modules/imageProc/ThresholdSpecifiedFactory.h
 * @date	Jan. 2017
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

#ifndef SRC_MODULES_IMAGEPROC_THRESHOLDSPECIFIEDFACTORY_H_
#define SRC_MODULES_IMAGEPROC_THRESHOLDSPECIFIEDFACTORY_H_

#ifdef HAVE_OPENCV

#include "core/ModuleFactoryBranch.h"

/**
 * Leaf factory for the thresholding modules
 *
 * @par 2.1.0-dev.7
 * The previous leaf (with mask / no mask), that was one step deeper,
 * is removed. This is now the leaf factory.
 */
class ThresholdSpecifiedFactory: public ModuleFactoryBranch
{
public:
    ThresholdSpecifiedFactory(ModuleFactory* parent, std::string selector);

    std::string name();
    std::string description();

    size_t countRemain() { return 1; }

private:
    Module* newChildModule(std::string customName);

    enum ThresTypes
    {
        absoluteType,
        populationType,
        meanType
    };

    ThresTypes mType;
};

#endif /* HAVE_OPENCV */
#endif /* SRC_MODULES_IMAGEPROC_THRESHOLDSPECIFIEDFACTORY_H_ */
