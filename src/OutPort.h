/**
 * @file	src/OutPort.h
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

#ifndef SRC_OUTPORT_H_
#define SRC_OUTPORT_H_

#include "Port.h"

#include "Poco/RWLock.h"
#include "Poco/SharedPtr.h"
#include <vector>

using Poco::RWLock;
using Poco::SharedPtr;

class InPort;

/**
 * OutPort
 *
 * Data output module port.
 * Contain links to the target ports
 */
class OutPort: public Port
{
public:
    OutPort(Module* parent,
            std::string name,
            std::string description,
            dataTypeEnum datatype,
            size_t index);

    /**
     * Special constructor for the empty OutPort
     */
    OutPort();

    virtual ~OutPort() { }

    /**
     * Retrieve the target ports
     */
    std::vector< SharedPtr<InPort*> > getTargetPorts();

    /**
     * Try to retrieve a pointer on the data to be written
     *
     * @return false if the lock cannot be acquired
     */
    template<typename T> bool tryData(T*& data);

    /**
     * Try to set a data sequence start.
     *
     *  - try to acquire the lock on the data
     *  - write a data sequence start
     *  - release the lock and notifies the dispatcher
     */
    bool tryStartDataSequence();

    /**
     * Try to set a data sequence end.
     *
     *  - try to acquire the lock on the data
     *  - write a data sequence end
     *  - release the lock and notifies the dispatcher
     */
    bool tryEndDataSequence();

    /**
     * Notify the dispatcher that the new data is ready
     *
     * And release the lock acquired with tryData
     */
    void notifyReady();

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

    friend class InPort;
};

#endif /* SRC_OUTPORT_H_ */
