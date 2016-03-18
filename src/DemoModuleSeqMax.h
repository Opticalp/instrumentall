/**
 * @file	src/DemoModuleSeqMax.h
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

#ifndef SRC_DEMOMODULESEQMAX_H_
#define SRC_DEMOMODULESEQMAX_H_

#include "Module.h"

#include "Poco/Mutex.h"

/**
 * DemoModuleSeqMax
 *
 * Simple demo module to retrieve the maximum of a data sequence.
 */
class DemoModuleSeqMax: public Module
{
public:
    DemoModuleSeqMax(ModuleFactory* parent, std::string customName);
    virtual ~DemoModuleSeqMax() { }

    const char * description() const
    {
        return "Demo Module to retrieve the max of a data sequence. ";
    }

    /**
     * Main logic
     *
     * Find the max of a data sequence:
     *  - reinit the temp max storage at each startSequence
     *  - send the max at each endSequence
     */
    void process();

private:
    static size_t refCount; ///< reference counter to generate a unique internal name

    /// Indexes of the input ports
    enum inPorts
    {
        inPortA,
        inPortCnt
    };

    /// Indexes of the output ports
    enum outPorts
    {
        outPortA,
        outPortCnt
    };

    int tmpMax;
};

#endif /* SRC_DEMOMODULESEQMAX_H_ */
