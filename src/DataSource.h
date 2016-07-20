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
	virtual ~DataSource();

	virtual std::string name() { return "emptyDataSource"; };
	virtual std::string description()
		{ return "empty data source to be used when a data source was just deleted"; } ;

    /**
     * Retrieve the data targets
     */
    std::set<DataTarget*> getDataTargets();

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

    void expire();
    bool isExpired() { return expired; }

    /**
     * Dispatch Module::resetWithSeqTargets
     */
    void resetTargets();

    /**
     * Notify the dispatcher that the new data is ready
     *
     * With the given attributes,
     * and release the lock acquired with tryData
     */
    void notifyReady(DataAttribute attribute);

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
    void removeDataTarget(DataTarget* target);

    std::set<DataTarget*> dataTargets;
    Poco::FastMutex targetLock; ///< non-recursive mutex for data target operations

    bool expired;

    friend class DataTarget;
};

#endif /* SRC_DATASOURCE_H_ */
