/**
 * @file	src/DataLogger.h
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
class DataLogger: public DataTarget, public Poco::Runnable, public Poco::RefCountedObject
{
public:
	/**
	 * Constructor
	 *
	 * In the implementations, the constructor could set
	 * the unique name of the logger.
	 */
    DataLogger() { }
    virtual ~DataLogger() { }

    /**
     * Lock the main mutex and launch log()
     *
     * This method release the data after the logging.
     */
    void run();

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
     * The data is released by the calling function.
     */
    virtual void log() = 0;

private:
    /**
     * Launch run() in a new thread using the ThreadManager
     */
    void runTarget();

	size_t incUser() { duplicate(); return referenceCount(); }
	size_t decUser() { release();   return referenceCount(); }

    Poco::FastMutex mutex; ///< data logger main mutex
};

#endif /* SRC_DATALOGGER_H_ */
