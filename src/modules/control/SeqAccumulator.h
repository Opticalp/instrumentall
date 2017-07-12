/**
 * @file	src/modules/control/SeqAccumulator.h
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

#ifndef SRC_MODULES_CONTROL_SEQACCUMULATOR_H_
#define SRC_MODULES_CONTROL_SEQACCUMULATOR_H_

#include "core/Module.h"

/**
 * SeqAccumulator
 *
 * Transform an array into a data sequence. This module is a sequence source.
 */
class SeqAccumulator: public Module
{
public:
	SeqAccumulator(ModuleFactory* parent, std::string customName, int dataType);

    std::string description();

private:
    static size_t refCount; ///< reference counter to generate a unique internal name

    /**
     * Main logic
     */
    void process(int startCond);

    /// Indexes of the input ports
    enum inPorts
    {
        arrayInPort,
        inPortCnt
    };

    /// Indexes of the output ports
    enum outPorts
    {
        dataOutPort,
        outPortCnt
    };

    /**
     * Send the input array, unstacked.
     *
     * Reserve the outport as much times as necessary,
     * set the data, and notify the outport.
     */
    template <typename T>
    void sendData(std::vector<T>& input, DataAttributeIn attr);

    int mDataType;
};

#include "SeqAccumulator.ipp"

#endif /* SRC_MODULES_CONTROL_SEQACCUMULATOR_H_ */
