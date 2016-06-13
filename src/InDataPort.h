/**
 * @file	src/InDataPort.h
 * @date	feb. 2016
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

#ifndef SRC_INDATAPORT_H_
#define SRC_INDATAPORT_H_

#include "InPort.h"

#include "Poco/RWLock.h"
#include "Poco/SharedPtr.h"

using Poco::SharedPtr;

class Dispatcher;

/**
 * InDataPort
 *
 * Data input module port.
 * Contain link to the source port.
 */
class InDataPort: public InPort
{
public:
    InDataPort(Module* parent,
            std::string name,
            std::string description,
            int datatype,
            size_t index);

    /**
     * Special constructor for the empty InDataPort
     */
    InDataPort(OutPort* emptySourcePort);

    /**
     * Destructor
     *
     * Do not need to call releaseSourcePort.
     * Indeed, this destructor is called when the parent module
     * is deleted. On Module deletion, the ModuleManager::removeModule
     * is called, then the Dispatcher::removeModule is called,
     * then this input port is deleted from Dispatcher::allInDataPorts
     * after this releaseSourcePort has been called.
     */
    virtual ~InDataPort() { }

    /// Retrieve the data sequence source port
    SharedPtr<OutPort*> getSeqSourcePort();

    /**
     * Try to lock the input port to read its data and attribute
     *
     * call isUpToDate() to check if the data is valid
     *
     * @return false if the lock can not be acquired or if the data
     * is not up to date
     */
    bool tryLock();

    /**
     * Read the incoming data
     *
     * The port shall have been previously locked using
     * tryLock, with return value == true.
     */
    template<typename T> void readData(T*& pData);

    /**
     * Combination of tryLock, readDataAttribute, and readData
     */
    template<typename T> bool tryData(T*& pData, DataAttributeIn* pAttr);

    /**
     * Notify that the data has been used and can be released.
     *
     * release the corresponding locks, notify that the data is not new
     * any more, set expiration information...
     */
    void release();

    /**
     * Request to hold the data that came to this port
     *
     * The port will re-use the previous data if it is not expired.
     */
    void hold(bool status = true) { held = status; }

private:
    /**
     * Set the data sequence source port
     *
     * This function should only be called by the dispatcher.
     *
     * If the source port was not the empty port,
     * the port that was previously bound is notified
     */
    void setSeqSourcePort(SharedPtr<OutPort*> port);

    /**
     * Release the data sequence source port
     *
     * And replace it by the empty port.
     * If the source port was not the empty port,
     * the port that was previously bound is notified
     */
    void releaseSeqSourcePort();

    SharedPtr<OutPort*> mSeqSourcePort;

    /**
     * Determine if the data of this port are up to date
     *
     * To be used before getting the data to be sure not to retrieve
     * expired data.
     */
    bool isUpToDate();

    bool held;

    friend class Dispatcher;
};

#include "InDataPort.ipp"

#endif /* SRC_INDATAPORT_H_ */
