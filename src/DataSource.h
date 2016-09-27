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
	DataSource(DataSource* source):
		DataItem(*source), notifying(false),
		sourceCancelling(false),
		users(0)
	{
	}

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
     * Check the pendingDataTargets,
     * then forward to DataItem::tryWriteDataLock
     *
     * pendingDataTargets is supposed to have precedence...
     * Unless two threads try to access the same source together
     */
    bool tryWriteDataLock();

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
     *
     * @throw ExecutionAbortedException in case of
     * pending cancellation
     * @return true if the insertion succeeded, false
     * if the target was already inserted
     */
    bool registerPendingTarget(DataTarget* target);

    /**
     * Increment the user count
     *
     * Should be overloaded in inherited classes that implement
     * Poco::RefCountedObject.
     *
     * @see DataProxy
     * @see DataLogger
     */
    virtual void incUser() { ++users; }

    /**
     * Decrement the user count
     *
     * Should be overloaded in inherited classes that implement
     * Poco::RefCountedObject
     */
    virtual void decUser() { --users; }
    virtual size_t userCnt() { return users; }

    /**
     * Cancel self and dispatch cancellation to the targets
     *
     * To be called by the implementation in case of cancellation
     *
     * sourceCancel is not called here
     */
    void cancelWithTargets();

    /**
     * Wait for the targets to have their cancellation effective
     */
    void waitTargetsCancelled();

    /**
     * Reset self and dispatch reset to the targets
     *
     * To be called after the cancellation having been effective
     *
     * sourceReset is not called here
     */
    void resetWithTargets();

protected:
    /**
     * Implement the cancellation in the concerned entity
     *
     * Called by cancelFromTarget
     *
     * Should cancel the entity and forward the cancellation
     */
    virtual void sourceCancel() = 0;

    /**
     * Wait for the entity to be effectively cancelled
     */
    virtual void sourceWaitCancelled() = 0;

    /**
     * Implement the reseting in the concerned entity
     *
     * Called by resetFromTarget
     *
     * Should reset the entity and forward the cancellation
     */
    virtual void sourceReset() = 0;

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
     * Release output data
     *
     *  - Release the lock
     */
    void releaseWrite();

    /**
     * Release the read lock from the given target
     *
     * and remove the target from the pendingDataTargets
     * and reservedDataTargets.
     *
     * to be called by DataTarget::releaseInputData
     */
    void targetReleaseRead(DataTarget* target);

    /**
     * Check if the given target was notified for available data
     *
     * if true,
     *  - check if it is already reserved. Then return false.
     *  - if not, reserve (insertion in reservedDataTargets)
     *    and return true
     * if no available data (target not in pendingDataTargets),
     * return false.
     */
    bool tryReserveDataForTarget(DataTarget* target);

    /**
     * Lock the data for reading
     */
    void readLockDataForTarget(DataTarget* target);

    std::set<DataTarget*> dataTargets;
    Poco::FastMutex targetsLock; ///< non-recursive mutex for data target operations

    std::set<DataTarget*> pendingDataTargets; ///< targets that is not over with using the data
    std::set<DataTarget*> reservedDataTargets; ///< targets that reserved the use of the data
    std::set<DataTarget*> lockedDataTargets; ///< targets that read locked the data
    Poco::FastMutex pendingTargetsLock; ///< lock used for pendingDataTargets and reservedDataTargets

    size_t users;

    /**
     * Cheap safety
     *
     * Lock the data access betw. write lock release and
     * read lock acquisition
     */
    bool notifying;

    /**
     * Cheap safety
     *
     * Lock the targets registering during cancellation
     */
    bool sourceCancelling;

    /**
     * Cancellation coming from a target
     *
     * The cancellation request is dispatched to the other targets too
     *
     * call sourceCancel
     */
    void cancelFromTarget(DataTarget* target);

    /**
     * Reseting coming from a target
     *
     * The reseting request is dispatched to the other targets too
     *
     * call sourceReset
     */
    void resetFromTarget(DataTarget* target);

    friend class DataTarget;
};

#endif /* SRC_DATASOURCE_H_ */
