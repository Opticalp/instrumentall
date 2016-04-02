/**
 * @file	src/DataAttributeIn.cpp
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

#include "DataAttributeIn.h"
#include <algorithm>

DataAttributeIn& DataAttributeIn::operator =(const DataAttributeIn& other)
{
    DataAttributeIn tmp(other);
    swap(tmp);
    return *this;
}

void DataAttributeIn::swap(DataAttributeIn& other)
{
    DataAttribute::swap(other);
    std::swap(mParent, other.mParent);
}

DataAttribute DataAttributeIn::cleaned() const
{
    DataAttributeIn retAttr(*this);

    if (retAttr.seqTargets.erase(mParent))
    {
        if (retAttr.allSequences.empty())
            throw Poco::RuntimeException("DataAttributeIn::cleaned",
                    "no active sequence to clean for this target");

        size_t currentSeq = retAttr.allSequences.back();
        retAttr.allSequences.pop_back();

        if (!retAttr.startingSequences.empty()
                && (retAttr.startingSequences.back() == currentSeq) )
            retAttr.startingSequences.pop_back();

        if (!retAttr.endingSequences.empty()
                && (retAttr.endingSequences.back() == currentSeq) )
            retAttr.endingSequences.pop_back();

        // some safety cleaning:
        if (allSequences.empty())
            retAttr.seqTargets.clear();
        // we do not know what else to clean if allSequences is not empty
    }

    return retAttr; // implicit cast
}

bool DataAttributeIn::isStartSequence(size_t& seqIndex)
{
    checkSequence(seqIndex);

    if (seqIndex)
        return false;

    // seqIndex == 0
    if (seqTargets.count(mParent) == 0)
        return false;

    size_t currentSeq;
    currentSeq = allSequences.back();
    if (currentSeq == startingSequences.back())
    {
        seqIndex = currentSeq;
        return true;
    }
    else
        throw Poco::RuntimeException("isStartSequence",
                "The current sequence is not flagged as starting. "
                "Please, check the seq generating module");
}

bool DataAttributeIn::isInSequence(size_t seqIndex)
{
    checkSequence(seqIndex);

    if (seqIndex == 0)
        return false;

    if (seqIndex == allSequences.back())
        return true;

    throw Poco::RuntimeException("isInSequence",
            "The given seqIndex is not the current sequence");
}

bool DataAttributeIn::isEndSequence(size_t& seqIndex)
{
    // seqIndex should be != 0 since isInSequence should have been
    // already called and returned true

    if (!endingSequences.empty() && (seqIndex == endingSequences.back()))
    {
        seqIndex = 0;
        return true;
    }

    return false;
}

void DataAttributeIn::checkSequence(size_t seqIndex)
{
    if (seqIndex == 0)
        return;

    if (seqTargets.count(mParent) == 0)
        throw Poco::RuntimeException("checkSequence",
                "this port is not a seq target, "
                "but a seqIndex != 0 is given");

    if (allSequences.back() != seqIndex)
        throw Poco::RuntimeException("checkSequence",
                "the given seqIndex does not correspond "
                "to the last entered sequence");

}
