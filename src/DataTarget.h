/**
 * @file	src/DataTarget.h
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

#ifndef SRC_DATATARGET_H_
#define SRC_DATATARGET_H_

#include "DataSource.h"

#include "Poco/Mutex.h"

#include <set>

class DataAttribute;
//class OutPort;
class Dispatcher;

/**
 * DataTarget
 *
 * Base class for any data target, e.g. :
 *  - InPort (InDataPort, TrigPort)
 *  - DataLogger
 *  - DataProxy (to be coming soon)
 *  - ParameterSetter (to be coming soon)
 *
 * The binding between source and target port is asymmetric:
 *  - the binding is requested via the DataTarget::setDataSource (by the dispatcher)
 *  - DataSource::AddDataTarget is then called by DataTarget::setDataSource
 *
 * By the same way, the bind release is achieved via DataTarget::releaseDataSource
 * that calls DataSource::releaseDataTarget
 */
class DataTarget
{
public:
	DataTarget();
	virtual ~DataTarget();

	virtual std::string name() { return "emptyDataTarget"; };
	virtual std::string description()
		{ return "empty data target to be used when a data target was just deleted"; } ;

	/**
	 * Retrieve the data source
	 */
	DataSource* getDataSource();

    /**
     * Check if data is available at this source
     *
     * Signal to the source that the data will be consumed
     *
     * @return true if data available
     */
    bool tryCatchSource();

    /**
     * Read the data attribute of the incoming data
     *
     * The port shall have been previously locked using
     * tryLock, with return value == true.
     *
     * The object is not a seq Target, it is not supposed
     * to interpret the attribute as a DataAttributeIn.
     *
     * This method should only be used to forward a data
     * attribute.
     *
     * @see SeqTarget::readInputDataAttribute
     */
    void readInputDataAttribute(DataAttribute* pAttr);

    /**
     * Store that the data has been used and can be released.
     *
     * Release the corresponding lock
     */
    void releaseInputData();

    /**
     * Return the data type of the data source
     * plugged to this target.
     */
    int sourceDataType();

protected:
    /**
     * Main logic to launch the target action
     *
     * This method is called synchronously, but it can launch
     * itself a thread.
     */
    virtual void runTarget() = 0;

	/**
	 * Register a new data source
	 *
	 * The data source has to be unique. Setting a new data source frees the
	 * previous data source.
	 *
	 * Call DataSource::AddDataTarget
	 */
	void setDataSource(DataSource* source);

	/**
	 * Release the data source
	 *
	 * and replace it by NULL
	 */
	void detachDataSource();

    /**
     * Check if the given data type is supported
     *
     * To be overridden in the implementations
     * @see DataItem for the data type definitions
     */
    virtual bool isSupportedInputDataType(int dataType) = 0;

    /**
     * Return the supported data types
     *
     * To be used when looking for a suited data proxy
     *
     * @see isSupportedDataType
     */
    virtual std::set<int> supportedInputDataType() = 0;

    bool isSourcePlugged() { return (dataSource != 0); }

private:
    DataSource* dataSource;
    Poco::FastMutex sourceLock; ///< lock for the dataSource operations

    friend class Dispatcher;
};

#endif /* SRC_DATATARGET_H_ */
