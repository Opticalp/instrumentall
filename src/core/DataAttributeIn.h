/**
 * @file	src/core/DataAttributeIn.h
 * @date	Mar 2016
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

#ifndef SRC_DATAATTRIBUTEIN_H_
#define SRC_DATAATTRIBUTEIN_H_

#include "DataAttribute.h"

/**
 * DataAttributeIn
 *
 * DataAttribute specialized by the input port in the tryData query
 * to leave the ability to this DataAttribute to query itself about
 * the startSequence and endSequence, comparing its internal lists.
 *
 * Typical use case in a Module:
 *
 *     class Mod1: public Module
 *     {
 *     public:
 *       Mod1(): seqIndex(0) { }
 *
 *       process(inPortsAccess, outPortsAccess)
 *       {
 *         DataAttributeIn attr;
 *
 *         DataTypeName* pData;
 *
 *         inPortsAccess.tryData<DataTypeName>(inPortA, pData, &attr);
 *         // we assume the the tryData works at the first try
 *
 *         if (attr.isStartSequence(seqIndex))
 *         // sequence initialization stuff
 *
 *         if (attr.isInSequence(seqIndex))
 *         {
 *           // do things with the data in sequence: processing
 *
 *           if (attr.isEndSequence(seqIndex))
 *               // sequence un-initialization stuff (writing output, ...)
 *           else
 *               keepParamLocked();
 *         }
 *         else
 *         {
 *           // stuff that can be made if the input data is not in a seq
 *         }
 *       }
 *
 *     private:
 *       size_t seqIndex;
 *     };
 *
 */
class DataAttributeIn: public DataAttribute
{
public:
    DataAttributeIn(DataAttribute attribute, SeqTarget* parent):
        DataAttribute(attribute), mParent(parent) { }

    DataAttributeIn(const DataAttributeIn& other):
        DataAttribute(other), mParent(other.mParent) { }

    DataAttributeIn(): mParent(NULL) { }

    virtual ~DataAttributeIn() { }

    DataAttributeIn& operator =(const DataAttributeIn& other);

    /**
     * Clean the present DataAttribute from the sequence info
     * that should have been used here.
     */
    DataAttribute cleaned() const;

    /**
     * Check if the InPort is concerned by a startSequence
     *
     * Perform check and modification in seqIndex
     */
    bool isStartSequence(size_t& seqIndex);
    bool isInSequence(size_t seqIndex);
    /**
     * To be used in a loop where isInSequence was already called
     * with the same argument.
     */
    bool isEndSequence(size_t& seqIndex);

private:
    void swap(DataAttributeIn& other);

    /**
     * Routine check
     *
     * @throw Poco::RuntimeException if seqIndex not null
     * and mParent not in seqTargets
     * @throw Poco::RuntimeException if seqIndex not null
     * and not present in allSequences
     */
    void checkSequence(size_t seqIndex);

    SeqTarget* mParent;
};

#endif /* SRC_DATAATTRIBUTEIN_H_ */
