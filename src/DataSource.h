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

#include "Poco/SharedPtr.h"

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
	DataSource(int datatype = typeUndefined);
	virtual ~DataSource();

    /**
     * Retrieve the target ports
     */
    std::vector< SharedPtr<InPort*> > getTargetPorts();

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

private:
    /**
     * Add a target port
     *
     * This function should only be called by the target InPort.
     *
     * The Dispatcher is requested to get the shared pointer
     * on the InPort.
     */
    void addTargetPort(InPort* port);

    /**
     * Remove a target port
     *
     * Should not throw an exception if the port is not present
     * in the targetPorts
     */
    void removeTargetPort(InPort* port);

    std::vector< SharedPtr<InPort*> > targetPorts;
    RWLock targetPortsLock; ///< lock for targetPorts operations

    bool expired;

    friend class InPort;
    // friend class DataLogger;
};

#endif /* SRC_DATASOURCE_H_ */
