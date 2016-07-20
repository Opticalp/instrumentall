/**
 * @file	src/InPort.h
 * @date	Apr. 2016
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

#ifndef SRC_INPORT_H_
#define SRC_INPORT_H_

#include "Port.h"

#include "Poco/Mutex.h"
#include "Poco/SharedPtr.h"

using Poco::SharedPtr;

class DataAttributeIn;
class OutPort;
class Dispatcher;

/**
 * InPort
 *
 * Parent class for InDataPort and TrigPort
 */
class InPort: public Port
{
public:
    InPort(Module* parent,
            std::string name,
            std::string description,
            int datatype,
            size_t index, bool trig = false);

    /**
     * Constructor to be used to create empty input ports
     *
     * Either empty TrigPort or empty InDataPort
     */
    InPort(OutPort* emptySourcePort,
            std::string name,
            std::string description, bool trig = false);

    virtual ~InPort() { }

    /// get port data type
    int dataType() { return mType; }
    /// get port data type as a character string
    std::string dataTypeStr() { return DataItem::dataTypeStr(mType); }
    /// get port indexing at the parent module

    /// Retrieve the source port
    SharedPtr<OutPort*> getSourcePort()
        { return mSourcePort; }

    /**
     * Try to lock the input port to get the incoming data
     *
     * @return true if success
     */
    virtual bool tryLock() = 0;

    /**
     * Read the data attribute of the incoming data
     *
     * The port shall have been previously locked using
     * tryLock, with return value == true.
     */
    void readDataAttribute(DataAttributeIn* pAttr);

    /**
     * Store that the data has been used and can be released.
     *
     * Release the corresponding locks, record that the data is not new
     * any more
     */
    virtual void release();

    bool isNew() { return !used; }

    /**
     * Convenience function
     *
     * Allow not to check a dynamic_cast result.
     * Faster, simplier.
     */
    bool isTrig() { return isTrigFlag; }

    bool isPlugged() { return plugged; }

protected:
    /**
     * Set the source port
     *
     * This function should only be called by the dispatcher.
     *
     * If the source port was not the empty port,
     * the port that was previously bound is notified
     */
    void setSourcePort(SharedPtr<OutPort*> port);

    /**
     * Release the source port
     *
     * And replace it by the empty port.
     * If the source port was not the empty port,
     * the port that was previously bound is notified
     */
    void releaseSourcePort();

    /**
     * To be called by the dispatcher
     *
     * when new data is available, just before the push.
     *
     * Release the lock that was initiated by Dispatcher::lockInPorts
     */
    void setNew(bool value = true);

    /**
     * Lock the newDataMutex
     *
     * to insure that nobody tryLock the data during
     * the data update
     *
     * To be called by Dispatcher::lockInPorts
     * and to be released by `InPort::setNew(true)` called
     * by Dispatcher::setOutPortDataReady
     */
    void newDataLock() { newDataMutex.lock(); }

    /**
     * Unlock the newDataMutex
     *
     * @see newDataLock
     */
    void newDataUnlock() { newDataMutex.unlock(); }

private:
    int mType; ///< port data type

    SharedPtr<OutPort*> mSourcePort;

    bool used;
    Poco::FastMutex newDataMutex;

    bool isTrigFlag;

    bool plugged;

    friend class Dispatcher;
};

#endif /* SRC_INPORT_H_ */
