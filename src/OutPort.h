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

#include "DataSource.h"
#include "DataAttributeOut.h"
#include "Port.h"

#include "Poco/RWLock.h"
#include "Poco/SharedPtr.h"
#include <vector>

using Poco::RWLock;
using Poco::SharedPtr;

class InDataPort;
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
            int datatype,
            size_t index);

    /**
     * Special constructor for the empty OutPort
     */
    OutPort();

    /**
     * Destructor
     *
     * Do not need to call removeTargetPort.
     * Indeed, this destructor is called when the parent module
     * is deleted. On Module deletion, the ModuleManager::removeModule
     * is called, then the Dispatcher::removeModule is called,
     * then this output port is deleted from Dispatcher::allOutPorts
     * after this removeTargetPort has been called.
     *
     * Detach loggers
     */
    virtual ~OutPort();

    /**
     * Retrieve the target ports
     */
    std::vector< SharedPtr<InPort*> > getTargetPorts();

    /**
     * Retrieve the data sequence target ports
     */
    std::vector< SharedPtr<InPort*> > getSeqTargetPorts();

    /**
     * Try to lock the output port to retrieve a pointer on
     * the data to be written
     *
     * @return true if success
     */
    bool tryLock()
    	{ return dataSource()->tryWriteDataLock(); }

    /**
     * Retrieve a pointer on the data to be written
     *
     * The port shall have been previously locked using
     * tryLock, with return value == true.
     */
    template<typename T> void getDataToWrite(T*& pData)
    	{ pData = dataSource()->getData<T>(); }

    /**
     * Try to retrieve a pointer on the data to be written
     *
     * @return false if the lock cannot be acquired
     */
    template<typename T> bool tryData(T*& pData)
        { return dataSource()->tryGetDataToWrite<T>(pData); }

    /**
     * Notify the dispatcher that the new data is ready
     *
     * With the given attributes,
     * and release the lock acquired with tryData
     */
    void notifyReady(DataAttributeOut attribute);

    /**
     * Release the output port data lock
     *
     * Without notifying the dispatcher.
     * To be used in case of failure.
     */
    void releaseOnFailure() { dataSource()->releaseBrokenData(); }

    /**
     * Dispatch Module::resetWithSeqTargets
     */
    void resetSeqTargets();

    /**
     * Get the DataSource for this OutPort
     */
    DataSource* dataSource() { return &data; }

    /**
     * Expire port data
     */
    void expire();

    /**
     * Retrieve the data loggers
     *
     * This function calls the DataManager::getDataLogger
     * to retrieve the shared pointers
     */
    std::set< SharedPtr<DataLogger*> > loggers();

    /**
     * Check if loggers are registered
     */
    bool hasLoggers() { return (allLoggers.size()>0); }

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

    /**
     * Add a sequence re-combiner target port
     *
     * This function should only be called by the seq target InDataPort.
     *
     * The Dispatcher is requested to get the shared pointer
     * on the InDataPort.
     */
    void addSeqTargetPort(InDataPort* port);

    /**
     * Remove a sequence re-combiner target port
     *
     * Should not throw an exception if the port is not present
     * in the seqTargetPorts
     */
    void removeSeqTargetPort(InDataPort* port);

    /// list of target ports for data sequence re-combination
    std::vector< SharedPtr<InPort*> > seqTargetPorts;
    RWLock seqTargetPortsLock; ///< lock for seqTargetPorts operations

    void registerLogger(DataLogger* logger);
    void detachLogger(DataLogger* logger);

    std::set<DataLogger*> allLoggers;
    RWLock loggersLock;

    friend class InPort;
    friend class InDataPort;
    friend class DataLogger;

    DataSource data;
};

#endif /* SRC_OUTPORT_H_ */
