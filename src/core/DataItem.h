/**
 * @file	src/core/DataItem.h
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

#ifndef SRC_DATAITEM_H_
#define SRC_DATAITEM_H_

#include "DataAttribute.h"
#include "TypeNeutralData.h"

#include "Poco/RWLock.h"

using Poco::RWLock;

/**
 * DataItem
 *
 * Container for TypeNeutralData and a DataAttribute.
 *
 * Manage the locks about the data access.
 *
 * The public methods are the read methods to be accessed from the data targets
 *
 * The protected methods are the write methods to be accessed by the
 * inherited data generating classes.
 *
 */
class DataItem: public TypeNeutralData
{
public:
    DataItem(int datatype = typeUndefined): TypeNeutralData(datatype) { }

    /**
     * Copy constructor
     *
     * Used by DuplicatedSource.
     * The duplication can not be made when a module is running.
     * No lock verification is requested, then...
     */
    DataItem(DataItem& other):
    	attribute(other.attribute),
		TypeNeutralData(other)
    {
    }

    virtual ~DataItem() { }

    DataAttribute getDataAttribute() { return attribute; }

    /**
     * Forward the tryReadLock() call to the data RWLock
     */
    bool tryReadDataLock()
        { return dataLock.tryReadLock(); }

    /**
     * Forward the readLock() call to the data RWLock
     */
    void readDataLock()
        { dataLock.readLock(); }

    /**
     * Unlock the data
     *
     * unlock the data that was previously locked using tryGetDataToWrite
     * or tryReadLock()
     */
    void unlockData()
        { dataLock.unlock(); }

protected:
    void setDataAttribute(DataAttribute attr) { attribute = attr; }

    /**
     * Locking part of tryGetDataToWrite
     *
     * @see getDataToWrite
     */
    virtual bool tryWriteDataLock()
    	{ return dataLock.tryWriteLock(); }

    /**
     * Retrieve a write reference on the data
     *
     * Lock the data
     * @throw Poco::DataFormatException forwarded from checkType()
     */
    template <typename T> bool tryGetDataToWrite(T*& pData)
    {
        checkType<T>();

        if (dataLock.tryWriteLock())
        {
            pData = getDataNoTypeCheck<T>();
            return true;
        }
        else
        {
            return false;
        }
    }

private:
    DataAttribute attribute; ///< data attribute
    RWLock dataLock; ///< lock to manage the access to the data
};

#endif /* SRC_DATAITEM_H_ */
