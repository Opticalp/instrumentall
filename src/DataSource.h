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

#include "Poco/Mutex.h"
#include "Poco/SharedPtr.h"

#include <set>

using Poco::SharedPtr;

class DataTarget;

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
	DataSource(int datatype = typeUndefined);

	/**
	 * Copy constructor
	 *
	 * Used by DuplicatedSource
	 */
	DataSource(DataSource* source): DataItem(*source) { }

	virtual ~DataSource();

	virtual std::string name() { return "emptyDataSource"; };
	virtual std::string description()
		{ return "empty data source to be used when a data source was just deleted"; } ;

    /**
     * Retrieve the data targets
     */
    std::set<DataTarget*> getDataTargets();

    /**
     * Try to lock the data to write
     *
     * Check the pendingDataTargets, then forward to DataItem::tryWriteDataLock
     */
    bool tryWriteDataLock();

    /**
     * Release output data
     *
     *  - Release the lock
     */
    void releaseWrite();

    /**
     * Release the output data lock
     *
     * To be used in case of failure.
     * Could disable the data reading.
     */
    void releaseWriteOnFailure();

    /**
     * Notify the dispatcher that the new data is ready
     *
     * With the given attributes,
     * and release the lock acquired with tryData
     */
    void notifyReady(DataAttribute attribute);

    /**
     * Lock (read) the data
     * and add the target to the pendingDataTargets
     */
    void registerPendingTarget(DataTarget* target);

    /**
     * Increment the user count
     *
     * Should be overloaded in inherited classes that implement
     * Poco::RefCountedObject.
     *
     * @see DataProxy
     * @see DataLogger
     */
    virtual size_t incUser() { return ++users; }

    /**
     * Decrement the user count
     *
     * Should be overloaded in inherited classes that implement
     * Poco::RefCountedObject
     */
    virtual size_t decUser() { return --users; }

private:
    /**
     * Add a data target
     *
     * This function should only be called by the target.
     */
    void addDataTarget(DataTarget* port);

    /**
     * Remove a data target
     *
     * Should not throw an exception if the target is not present
     * in the dataTargets
     */
    void detachDataTarget(DataTarget* target);

    /**
     * Release the read lock from the given target
     *
     * to be called by DataTarget::releaseRead
     */
    void targetReleaseRead(DataTarget* target);

    /**
     * Check if the source data is available for the given target
     */
    bool tryCatchRead(DataTarget* target);

    std::set<DataTarget*> dataTargets;
    Poco::FastMutex targetsLock; ///< non-recursive mutex for data target operations

    std::set<DataTarget*> pendingDataTargets;
    Poco::FastMutex pendingTargetsLock;

    size_t users;

    friend class DataTarget;
};

#endif /* SRC_DATASOURCE_H_ */
