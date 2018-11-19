/**
 * @file	src/core/DataProxy.h
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

#ifndef SRC_DATAPROXY_H_
#define SRC_DATAPROXY_H_

#include "DataSource.h"
#include "DataTarget.h"
#include "VerboseEntity.h"
#include "ParameterizedEntity.h"
#include "UniqueNameEntity.h"

#include "Poco/Thread.h"
#include "Poco/RefCountedObject.h"

/**
 * DataProxy
 *
 * Data converter used between a data source and a data target of incompatible
 * type. The conversion is done synchronously (no new thread).
 *
 * The derived classes shall implement a classDescription
 * static method. DataTarget::description can be implemented
 * as linking to this method.
 *
 * @par 2.1.0-dev.6
 * Add parameters support
 *
 * The implementations overload:
 *
 *     virtual std::set<int> supportedOutputDataType();
 *
 */
class DataProxy: public DataTarget, public DataSource,
	public ParameterizedEntity,
    public UniqueNameEntity,
	public VerboseEntity,
	public Poco::RefCountedObject
{
public:
	/**
	 * Constructor
	 *
	 * In the implementations, the constructor could set
	 * the unique name of the data proxy.
	 */
	DataProxy(std::string newClassName):
		className(newClassName),
		ParameterizedEntity("dataProxy." + newClassName),
		VerboseEntity("dataProxy." + newClassName) // startup logger name
		{   }

	virtual ~DataProxy() { poco_information(logger(), "deleting " + name()); }

    /**
     * Get the data logger implementation class name
     *
     * @see DataLogger::DataLogger(std::string implementationName)
     */
    const std::string& getClassName() const { return className; }

    /**
     * Implement ParameterizedEntity::name(),  DataTarget::name()
     * and DataSource::name()
     */
    std::string name() { return UniqueNameEntity::name(); }

    /**
     * Set a new name for the data proxy
     *
     *  - change logger()
     *  - change ParameterizedEntity prefix key
     *  - reload the default parameters if any
     */
    void setName(std::string newName);

protected:
	/**
	 * Convert the input data into the desired data type
	 *
	 * The input data is already locked.
     * The input data is released by the calling function.
	 *
	 * The output is already locked.
	 * The output notification is achieved by the calling function.
	 */
	virtual void convert() = 0;

	bool yield() { Poco::Thread::yield(); return false; }

    /**
     * Set data proxy internal name
     *
     * This function shall be called in the constructor implementation,
     * using a static ref counter.
     */
    void setName(size_t refCount);

    Poco::Logger& logger() { return VerboseEntity::logger(); }

private:
	DataProxy();

	/**
	 * Launch the conversion (sync run)
	 *
	 *  - lock the outputs, tryCatch the input, lock the input
	 *  - launch the conversion
	 *  - forward the data attributes
	 *  - release the inputs and notify the outputs
	 */
	void runTarget();

	void incUser() { duplicate(); }
	void decUser() { release();   }
	size_t userCnt() { return referenceCount(); }

	void targetCancel() { cancelWithTargets(); }
	void targetWaitCancelled() { waitTargetsCancelled(); }
	void targetReset() { resetWithTargets(); }

	void sourceCancel() { cancelWithSource(); }
	void sourceWaitCancelled() { waitSourceCancelled(); }
	void sourceReset() { resetWithSource(); }

	std::string className; ///< data logger implementation class name
};

#include "Poco/DynamicFactory.h"

#endif /* SRC_DATAPROXY_H_ */
