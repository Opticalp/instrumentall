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
    virtual ~OutPort();

    /// retrieve the target ports
    std::vector< SharedPtr<OutPort*> > getTargetPorts();

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
    RWLock lock; ///< lock for targetPorts operations

    friend class InPort;
};

#endif /* SRC_OUTPORT_H_ */
