/**
 * @file	src/DemoModuleB.h
 * @date	Feb. 2016
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

#ifndef SRC_DEMOMODULEB_H_
#define SRC_DEMOMODULEB_H_

#include "core/Module.h"

/**
 * DemoModuleB
 *
 * Very simple demo module, but implementing some ports
 */
class DemoModuleB: public Module
{
public:
    DemoModuleB(ModuleFactory* parent, std::string customName);
    virtual ~DemoModuleB() { }

    std::string description()
    {
        return "Very basic demo Module exhibiting 1 output port. ";
    }

private:
    static size_t refCount; ///< reference counter to generate a unique internal name

    /// Indexes of the output ports
    enum outPorts
    {
        outPortA,
        outPortCnt
    };
};

#endif /* SRC_DEMOMODULEB_H_ */
