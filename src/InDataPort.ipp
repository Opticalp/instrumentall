/**
 * @file	src/InDataPort.ipp
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

#include "InDataPort.h"
#include "OutPort.h"
#include "DataAttributeIn.h"

template<typename T> inline bool InDataPort::tryData(T*& pData, DataAttributeIn* pAttr)
{
    if (!isNew())
    {
        // try to get the lock
        if (!(*getSourcePort())->dataItem()->tryReadLock())
            return false;

        // check if the data is up to date
        if (!isUpToDate())
        {
            (*getSourcePort())->dataItem()->releaseData();
            return false;
        }
    }
    //else: nothing to do, the lock is already activated (by the dispatcher)

    // transform the data attribute into a DataAttributeIn
    *pAttr = DataAttributeIn(
        (*getSourcePort())->dataItem()->getDataAttribute(), this);

    pData = (*getSourcePort())->dataItem()->getDataToRead<T>();

    return true;
}




