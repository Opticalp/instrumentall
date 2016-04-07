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

#include "Poco/Task.h"
#include "Poco/Mutex.h"

using Poco::Mutex;

class DataItem;

/**
 * DataLogger
 *
 * Base class to implement data loggers that are called when receiving
 * new data.
 *
 * The derived classes should implement the static methods name() and
 * description().
 */
class DataLogger: public Poco::Task
{
public:
    DataLogger(std::string name = ""):
        pData(NULL), Task(name),
        empty(false) { }
    virtual ~DataLogger();

    /**
     * Get the name of the derived DataLogger class
     *
     * This name should be the C++ class name of the derived class
     */
    virtual std::string name() = 0;

    // could have been implemented with CRTP
//    /**
//     * Description of the data logger
//     *
//     * Should be static too.
//     * @see name()
//     */
//    virtual std::string description() = 0;

    /**
     * Detach the logger from its parent data
     *
     * Lock the dataLock and call detachNoLock()
     */
    void detach();

    /**
     * Acquire the read lock of the data
     *
     * To be called before launching the thread
     * with runTask()
     */
    void acquireLock();

    /**
     * Main logic
     *
     * Log the data.
     * The implementation must release the data lock.
     */
    virtual void runTask();

    /**
     * Log the data
     *
     * Function to override in implementations.
     * This function is called by runTask() if
     * runTask is not overridden.
     */
    virtual void log() { }

protected:
    /**
     * Retrieve parent data item
     */
    DataItem* data();

    /**
     * Bind the logger to its data item
     */
    void registerData(DataItem* data);

    /**
     * Detach the data logger from its data
     *
     * Without locking the dataLock.
     * @warning: do not notify the parent data.
     */
    void detachNoLock();

    void setEmpty();

    friend class DataManager;

private:
    DataItem* pData; ///< registered data
    Mutex dataLock;

    bool empty; ///< check if the data logger is activable
};

#endif /* SRC_DATALOGGER_H_ */
