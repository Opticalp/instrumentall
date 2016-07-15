/**
 * @file	src/DataSource.h
 * @date	Jul. 2016
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

#ifndef SRC_DATASOURCE_H_
#define SRC_DATASOURCE_H_

#include "DataItem.h"

#include "Poco/RWLock.h"
#include "Poco/SharedPtr.h"

using Poco::RWLock;
using Poco::SharedPtr;

class OutPort;
class DataLogger;

/**
 * DataSource
 *
 * Any data producer should derive from this class to trig data targets
 * 
 * Foreseen data sources could be:
 *  - module output ports
 *  - data proxies (data type translation)
 *  - UI data generators
 */
class DataSource: public DataItem
{
public:
	DataSource(int datatype = typeUndefined, OutPort* parent = NULL);
	virtual ~DataSource();

    /**
     * Locking part of tryGetDataToWrite
     *
     * @see getDataToWrite
     */
    bool tryLockToWrite() { return dataLock.tryWriteLock(); }

    /**
     * Retrieving pointer part of tryGetDataToWrite
     *
     * @see tryLockToWrite
     */
    template<typename T> void getDataToWrite(T*& pData)
    {
        checkType<T>();
        pData = reinterpret_cast<T*>(dataStore);
    }

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
            pData = reinterpret_cast<T*>(dataStore);
            return true;
        }
        else
        {
            return false;
        }
    }

    /**
     * Retrieve a pointer on the data to read it
     *
     * @warning The lock has to have been previously acquired
     * @throw Poco::DataFormatException forwarded from checkType()
     */
    template <typename T> T* getDataToRead()
    {
        checkType<T>();

        return reinterpret_cast<T*>(dataStore);
    }

    /**
     * Forward the tryReadLock() call to the data RWLock
     */
    bool tryReadLock()
        { return dataLock.tryReadLock(); }

    /**
     * Forward the readLock() call to the data RWLock
     */
    void readLock()
        { dataLock.readLock(); }

    /**
     * Release newly created data
     *
     *  - Release the lock
     *  - Notify the data manager that will acquire a read lock if necessary
     */
    void releaseNewData();

    /**
     * Release data that failed to be created
     *
     *  - Release the lock
     *  - expire the data
     */
    void releaseBrokenData();

    /**
     * Unlock the data
     *
     * unlock the data that was previously locked using tryGetDataToWrite
     * or tryReadLock()
     */
    void releaseData()
        { dataLock.unlock(); }

    /**
     * Get the parent port
     *
     * @warning the parent port can be NULL
     */
    OutPort* parentPort() { return mParentPort; }

    void expire() { expired = true; }
    bool isExpired() { return expired; }

private:
    bool expired;

    RWLock dataLock; ///< lock to manage the access to the dataStore

    OutPort* mParentPort; ///< parent output data port.

    // friend class DataLogger;
};

#endif /* SRC_DATASOURCE_H_ */
