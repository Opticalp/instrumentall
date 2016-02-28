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


size_t nextToBeUsedIndex = 0;

DataAttribute DataAttribute::newDataAttribute()
{
    DataAttribute tmp;
    lock.lock();
    tmp.indexes.insert(nextToBeUsedIndex++);
    lock.unlock();

    return tmp;
}

DataAttribute::~DataAttribute()
{
    // nothing to do
}

DataAttribute::DataAttribute(DataAttribute& other)
{
    indexes = other.indexes;
    startSequences = other.startSequences;
    endSequences = other.endSequences;
}

DataAttribute& DataAttribute::operator =(DataAttribute& other)
{
    DataAttribute tmp(other);
    swap(tmp);
    return *this;
}

DataAttribute& DataAttribute::operator +=(DataAttribute& rhs)
{
    indexes.insert(rhs.indexes.begin(), rhs.indexes.end());
    startSequences.insert(rhs.startSequences.begin(), rhs.startSequences.end());
    endSequences.insert(rhs.endSequences.begin(), rhs.endSequences.end());

    return *this; // return the result by reference
}

void DataAttribute::swap(DataAttribute& other)
{
    indexes.swap(other.indexes);
    startSequences.swap(other.startSequences);
    endSequences.swap(other.endSequences);
}
