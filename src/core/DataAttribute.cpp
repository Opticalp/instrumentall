/**
 * @file	src/core/DataAttribute.cpp
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


DataAttribute::~DataAttribute()
{
    // nothing to do
}

DataAttribute::DataAttribute(const DataAttribute& other)
{
    indexes = other.indexes;

    seqTargets = other.seqTargets;

    startingSequences = other.startingSequences;
    allSequences = other.allSequences;
    endingSequences = other.endingSequences;
}

void DataAttribute::swap(DataAttribute& other)
{
    indexes.swap(other.indexes);

    seqTargets.swap(other.seqTargets);

    startingSequences.swap(other.startingSequences);
    allSequences.swap(other.allSequences);
    endingSequences.swap(other.endingSequences);
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

    if (!rhs.allSequences.empty())
    {
        if (allSequences.empty())
        {
            seqTargets = rhs.seqTargets;
            startingSequences = rhs.startingSequences;
            allSequences = rhs.allSequences;
            endingSequences = rhs.endingSequences;
        }
        else
        {
            switch (compareSeqLists(allSequences, rhs.allSequences))
            {
            case lhsInRhs:
            {
                // TODO: compare startingSequences and endingSequences

                seqTargets = rhs.seqTargets;

                startingSequences = rhs.startingSequences;
                allSequences = rhs.allSequences;
                endingSequences = rhs.endingSequences;

                break;
            }
            case rhsInLhs:
            case lhsEqRhs:
                // TODO: compare startingSequences and endingSequences
                break;
            case lhsNoRhs:
            default:
                throw Poco::NotImplementedException("merging DataAttributes",
                        "not supported for disjoint allSequences");
            }

        }
    }

    return *this; // return the result by reference
}

DataAttribute::allSeqComp DataAttribute::compareSeqLists(const std::vector<size_t>& lhs,
        const std::vector<size_t>& rhs)
{
    size_t sizeL = lhs.size();
    size_t sizeR = rhs.size();

    size_t indexL = sizeL;
    size_t indexR = sizeR;

    for (; indexL && indexR; indexR--, indexL--)
    {
        if (lhs[indexL-1] != rhs[indexR-1])
            return lhsNoRhs;
    }

    if (indexL < indexR)
        return lhsInRhs;

    if (indexR < indexL)
        return rhsInLhs;

    return lhsEqRhs;
}
