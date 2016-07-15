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
	DataSource(int datatype = typeUndefined, OutPort* parent = NULL);
	virtual ~DataSource();

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
     * Get the parent port
     *
     * @warning the parent port can be NULL
     */
    OutPort* parentPort() { return mParentPort; }

    void expire() { expired = true; }
    bool isExpired() { return expired; }

private:
    bool expired;

    OutPort* mParentPort; ///< parent output data port.

    // friend class DataLogger;
};

#endif /* SRC_DATASOURCE_H_ */
