/**
 * @file	src/modules/dataGen/SeqGen.h
 * @date	May 2017
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

#ifndef SRC_MODULES_DATAGEN_SEQGEN_H_
#define SRC_MODULES_DATAGEN_SEQGEN_H_

#include "core/Module.h"

/**
 * Sequence generator
 *
 * Generate 64-bit integers from 0 to ... with a sequence start on the first
 * element and sequence end on the last one.
 * 
 * If the sequence is endless (seqSize param == 0), then the sequence has to
 * be ended with the module cancellation.
 *
 * @par v2.2.0-dev.1
 * Add parameter to manage clocked output
 */
class SeqGen: public Module
{
public:
    SeqGen(ModuleFactory* parent, std::string customName);

    std::string description()
    {
        return "Sequence generator module. ";
    }

private:
    SeqGen();

    static size_t refCount; ///< reference counter to generate a unique internal name

    void process(int startCond);

    /// Indexes of the input ports
    enum inPorts
    {
        trigPort,
        inPortCnt
    };

    /// Indexes of the output ports
    enum outPorts
    {
        outPortData,
        outPortCnt
    };

    enum params
    {
        paramSeqSize,
        paramDelay,
        paramCnt
    };

    Poco::Int64 getIntParameterValue(size_t paramIndex);
    void setIntParameterValue(size_t paramIndex, Poco::Int64 value);
    Poco::Int64 seqSize;
    Poco::Int64 delay;
};

#endif /* SRC_MODULES_DATAGEN_SEQGEN_H_ */
