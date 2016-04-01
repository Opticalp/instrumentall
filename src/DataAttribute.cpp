/**
 * @file	src/DataAttribute.cpp
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

#include "DataAttribute.h"

#include <algorithm> // swap

DataAttribute::~DataAttribute()
{
    // nothing to do
}

DataAttribute::DataAttribute(const DataAttribute& other)
{
    indexes = other.indexes;
    startSequenceTargets = other.startSequenceTargets;
    contSequenceTargets = other.contSequenceTargets;
    endSequenceTargets = other.endSequenceTargets;

    startCnt = other.startCnt;
    contCnt = other.contCnt;
    endCnt = other.endCnt;
}

DataAttribute& DataAttribute::operator =(const DataAttribute& other)
{
    DataAttribute tmp(other);
    swap(tmp);
    return *this;
}

DataAttribute& DataAttribute::operator +=(const DataAttribute& rhs)
{
    indexes.insert(rhs.indexes.begin(), rhs.indexes.end());
    startSequenceTargets.insert(rhs.startSequenceTargets.begin(), rhs.startSequenceTargets.end());
    contSequenceTargets.insert(rhs.contSequenceTargets.begin(), rhs.contSequenceTargets.end());
    endSequenceTargets.insert(rhs.endSequenceTargets.begin(), rhs.endSequenceTargets.end());

    throw Poco::NotImplementedException("Adding DataAttributes is not supported now");

    return *this; // return the result by reference
}

void DataAttribute::swap(DataAttribute& other)
{
    indexes.swap(other.indexes);
    startSequenceTargets.swap(other.startSequenceTargets);
    contSequenceTargets.swap(other.contSequenceTargets);
    endSequenceTargets.swap(other.endSequenceTargets);

    std::swap(startCnt, other.startCnt);
    std::swap(contCnt, other.contCnt);
    std::swap(endCnt, other.endCnt);

}

bool DataAttribute::isStartSequence(InPort* port)
{
    if (startCnt == 0)
        return false;

    bool ret = startSequenceTargets.erase(port) > 0;

    // -- safety guard --
    // if startCnt becomes null, it means that the seq flags
    // were all used on this data flow branch. The reminding
    // modules in startSequenceTargets are probably on other
    // data flow branches. It is better to clean, then.
    if (ret && (--startCnt == 0))
        startSequenceTargets.clear();

    return ret;
}

bool DataAttribute::isContinueSequence(InPort* port)
{
    if (contCnt == 0)
        return false;

    bool ret = contSequenceTargets.erase(port) > 0;

    // -- safety guard --
    // see isStartSequence
    if (ret && (--contCnt == 0))
        contSequenceTargets.clear();

    return ret;
}

bool DataAttribute::isEndSequence(InPort* port)
{
    if (endCnt == 0)
        return false;

    bool ret = endSequenceTargets.erase(port) > 0;

    // -- safety guard --
    // see isStartSequence
    if (ret && (--endCnt == 0))
        endSequenceTargets.clear();

    return ret;
}
