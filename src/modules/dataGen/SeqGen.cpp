/**
 * @file	src/SeqGen.cpp
 * @date	9 mai 2017
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

#include "SeqGen.h"

#include "Poco/NumberFormatter.h"

size_t SeqGen::refCount = 0;

SeqGen::SeqGen(ModuleFactory* parent, std::string customName):
    Module(parent, customName),
    seqSize(0)
{
    setInternalName("SeqGen"
                        + Poco::NumberFormatter::format(refCount));
    setCustomName(customName);
    setLogger("module." + name());

    setInPortCount(inPortCnt);
    addTrigPort("trig", "Launch the seq generation", trigPort);

    setOutPortCount(outPortCnt);
    addOutPort("data", "Output the data sequence from 0", DataItem::typeInt64, outPortData);

    // parameters
    setParameterCount(paramCnt);
    addParameter(paramSeqSize,
            "seqSize",
            "Size of the sequence to be generated. "
            "If 0: endless sequence is generated, until abortion. "
            "If 1: only one value is exported (0), without sequence. ",
            ParamItem::typeInteger, "0");

    seqSize = getIntParameterDefaultValue(paramSeqSize);

    notifyCreation();

    // if nothing failed
    refCount++;
}

void SeqGen::process(int startCond)
{
    bool trigged;

    switch (startCond)
    {
    case noDataStartState:
        trigged = false;
        break;
    case allDataStartState:
        trigged = true;
        break;
    default:
        poco_bugcheck_msg("impossible start condition");
        throw Poco::BugcheckException();
    }

    DataAttributeOut* pOutAttr = NULL;

    if (trigged)
    {
        DataAttributeIn inAttr;

        readLockInPort(trigPort);
        readInPortDataAttribute(trigPort, &inAttr);
        releaseInPort(trigPort);

        pOutAttr = new DataAttributeOut(inAttr);
    }
    else
    {
        pOutAttr = new DataAttributeOut;
    }

    reserveOutPort(outPortData);
    Poco::Int64 *pData;
    getDataToWrite<Poco::Int64>(outPortData, pData);

    *pData = 0;

    if (seqSize == 1)
    {
        notifyOutPortReady(outPortData, *pOutAttr);
    }
    else
    {
        Poco::Int64 nextInd = 0;

        pOutAttr->startSequence();
        *pData = nextInd++;
        notifyOutPortReady(outPortData, (*pOutAttr)++);

        while ((seqSize == 0) || (nextInd < seqSize))
        {
            reserveOutPort(outPortData);
            getDataToWrite<Poco::Int64>(outPortData, pData);

            if (nextInd == seqSize-1)
                pOutAttr->endSequence();

            *pData = nextInd++;

            notifyOutPortReady(outPortData, (*pOutAttr)++);

            if (seqSize)
                setProgress(static_cast<float>(nextInd) / static_cast<float>(seqSize));

            if (yield())
                throw ExecutionAbortedException(name(), "Cancelled upon user request" );
        }
    }

    delete pOutAttr;
}

Poco::Int64 SeqGen::getIntParameterValue(size_t paramIndex)
{
    poco_assert(paramIndex == paramSeqSize);

    return seqSize;
}

void SeqGen::setIntParameterValue(size_t paramIndex, Poco::Int64 value)
{
    poco_assert(paramIndex == paramSeqSize);

    if (value<0)
        throw Poco::RangeException("setParameterValue",
                "seqSize has to be positive or null");

    seqSize = value;
}
