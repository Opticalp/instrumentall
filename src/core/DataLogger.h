/**
 * @file	src/core/DataLogger.h
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

#ifndef SRC_DATALOGGER_H_
#define SRC_DATALOGGER_H_

#include "DataTarget.h"
#include "VerboseEntity.h"
#include "ParameterizedEntity.h"

#include "Poco/Runnable.h"
#include "Poco/Mutex.h"
#include "Poco/RefCountedObject.h"

using Poco::Mutex;

/**
 * DataLogger
 *
 * Base class to implement data loggers that are called when receiving
 * new data.
 *
 * The derived classes shall implement a classDescription
 * static method. DataTarget::description can be implemented
 * as linking to this method.
 */
class DataLogger: public DataTarget,
    public ParameterizedEntity,
    public Poco::Runnable, public Poco::RefCountedObject,
    public VerboseEntity
{
public:
	/**
	 * Constructor
	 *
	 * In the implementations, the constructor could set
	 * the unique name of the data logger.
	 */
    DataLogger(std::string implementationName):
        className(implementationName),
        ParameterizedEntity("dataLogger." + implementationName),
        VerboseEntity("dataLogger." + implementationName) // startup logger name
        {   }

    virtual ~DataLogger() { poco_information(logger(), "deleting " + name()); }

    /**
     * Lock the input and launch log()
     *
     * This method release the data after the logging.
     */
    void run();

    virtual std::string description() = 0;

    /**
     * Get the data logger implementation class name
     *
     * @see DataLogger::DataLogger(std::string implementationName)
     */
    const std::string& getClassName() const { return className; }

    std::string name() { return mName; }

    /**
     * Set a new name for the data logger
     *
     *  - change logger()
     *  - change ParameterizedEntity prefix key
     *  - reload the default parameters if any
     */
    void setName(std::string newName);

protected:
    /**
     * Log the data
     *
     * Function to override in implementations.
     * This function is called by the default implementation of run()
     *
     * The data to log should be accessed using
     *
     *     T* getDataSource()->getData<T>();
     *
     * The input data is already locked.
     * The data is released by the calling function.
     */
    virtual void log() = 0;

    /**
     * Set data logger internal name
     *
     * This function shall be called in the constructor implementation,
     * using a static ref counter.
     */
    void setName(size_t refCount);

    Poco::Logger& logger() { return VerboseEntity::logger(); }

private:
    DataLogger();

    /**
     * Launch run() in a new thread using the ThreadManager
     */
    void runTarget();

	void incUser() { duplicate(); }
	void decUser() { release();   }
	size_t userCnt() { return referenceCount(); }

	void targetCancel() { }
	void targetWaitCancelled() { }
	void targetReset() { }

	std::string className; ///< data logger implementation class name
	std::string mName;

    Poco::FastMutex mutex; ///< data logger main mutex
};

#endif /* SRC_DATALOGGER_H_ */
