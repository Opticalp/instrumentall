/**
 * @file	src/DataAttributeOut.cpp
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

#include "DataAttributeOut.h"

size_t DataAttributeOut::nextToBeUsedIndex = 1;
size_t DataAttributeOut::nextToBeUsedSeqIndex = 1;
Poco::Mutex DataAttributeOut::lock;

DataAttributeOut::DataAttributeOut(): seqManaging(0)
{
    appendNewIndex();
}

DataAttributeOut::DataAttributeOut(const DataAttributeOut& other):
        DataAttribute(other),
        newIndex(other.newIndex), seqManaging(other.seqManaging)
{
    // nothing to do
}

DataAttributeOut::DataAttributeOut(const DataAttributeIn& other):
        DataAttribute(other.cleaned()),
        newIndex(0), seqManaging(0)
{
    // nothing to do
}

DataAttributeOut::DataAttributeOut(const DataAttribute& other):
        DataAttribute(other),
        newIndex(0), seqManaging(0)
{
    // nothing to do
}

void DataAttributeOut::swap(DataAttributeOut& other)
{
    DataAttribute::swap(other);
//    std::swap(newIndex, other.newIndex);
//    std::swap(seqManaging, other.seqManaging);
}

DataAttributeOut& DataAttributeOut::operator =(const DataAttribute& other)
{
    DataAttributeOut tmp(other);
    swap(tmp);
    return *this;
}

DataAttributeOut& DataAttributeOut::operator =(const DataAttributeIn& other)
{
	DataAttributeOut tmp(other);
	swap(tmp);
	return *this;
}

DataAttributeOut& DataAttributeOut::operator =(const DataAttributeOut& other)
{
	DataAttributeOut tmp(other);
	swap(tmp);
	return *this;
}

DataAttributeOut& DataAttributeOut::operator ++()
{
    // remove previous index, add a new one.
    if (newIndex) // verify who last produced the index: importing, or generating.
        indexes.erase(newIndex);

    appendNewIndex();

    if (seqManaging)
    {
        if (allSequences.empty())
            poco_bugcheck_msg("seqManaging is set but allSequences is empty");

        // update sequence info
        size_t currentSeq = allSequences.back();

        if (!startingSequences.empty() && (startingSequences.back() == currentSeq))
            startingSequences.pop_back();

        if (!endingSequences.empty() && (endingSequences.back() == currentSeq))
        {
            endingSequences.pop_back();
            allSequences.pop_back();
            seqManaging--;
        }
    }

    return *this;
}

DataAttributeOut DataAttributeOut::operator ++(int int1)
{
    DataAttributeOut tmp(*this);
    operator++();
    return tmp;
}

void DataAttributeOut::startSequence()
{
    seqManaging++;

    size_t newSeqIndex;
    lock.lock();
    newSeqIndex = nextToBeUsedSeqIndex++;
    lock.unlock();

    startingSequences.push_back(newSeqIndex);
    allSequences.push_back(newSeqIndex);
}

void DataAttributeOut::endSequence()
{
    if (seqManaging == 0)
        throw Poco::RuntimeException("endSequence",
                "can not end a sequence that was not previously started "
                "using the same data attribute");

    if (allSequences.empty())
        poco_bugcheck_msg("no sequence active?!");

    endingSequences.push_back(allSequences.back());
}

void DataAttributeOut::appendNewIndex()
{
    lock.lock();
    newIndex = nextToBeUsedIndex++;
    lock.unlock();
    indexes.insert(newIndex);
}
