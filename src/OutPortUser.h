/**
 * @file	src/OutPortUser.h
 * @date	June 2016
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

#ifndef SRC_OUTPORTUSER_H_
#define SRC_OUTPORTUSER_H_

#include "VerboseEntity.h"

#include "DataAttributeOut.h"
#include "ModuleTask.h"

#include "Poco/ThreadLocal.h"
#include "Poco/Thread.h"
#include "Poco/Mutex.h"

class Module;
class OutPort;

class OutPortUser: public virtual VerboseEntity
{
public:
	OutPortUser() { }
	virtual ~OutPortUser();

	virtual std::string name() = 0;

	/**
	 * Get output ports
	 *
	 * @see getInPorts
	 */
	std::vector<OutPort*> getOutPorts() { return outPorts; }
	OutPort* getOutPort(std::string portName);

    /**
     * Expire output data
     */
    void expireOutData();

protected:
    /**
     * Set the outPorts list size
     */
    void setOutPortCount(size_t cnt) { outPorts.resize(cnt, NULL); }

	/**
	 * Retrieve the output ports count
	 */
    size_t getOutPortCount() { return outPorts.size(); }

    /**
     * Add output data port
     *
     * @param name name of the port
     * @param description description of the port
     * @param dataType type of port data
     * @param index index of the port in the outPorts list. It allows
     * to use enums to access the ports.
     */
    void addOutPort(Module* parent,
            std::string name, std::string description,
            int dataType,
            size_t index );

	/**
	 * Retrieve an output port, given its index
	 */
	OutPort* getOutPort(size_t index) { return outPorts.at(index); }

    /**
     * Retrieve a pointer on the data to be written
     */
    template<typename T>
    void getDataToWrite(size_t portIndex, T*& pData);

    /**
     * Forward tryData for the given port
     */
    template <typename T>
    bool tryOutPortData(size_t portIndex, T*& pData);

    /**
     * Forward releaseData to the given port
     */
    void notifyOutPortReady(size_t portIndex, DataAttributeOut attribute);

    /**
     * Forward releaseData to all the ports, using the same DataAttributeOut
     */
    void notifyAllOutPortReady(DataAttributeOut attribute);

    /**
     * Release the lock of all the output ports
     */
    void releaseAllOutPorts();

    /**
     * Check if the given port was caught/locked
     *
     * No lock needed: caughts is ThreadLocal
     */
    bool isOutPortCaught(size_t index)
    	{ return (caughts->find(index) != caughts->end()); }

	/**
	 * Lock all output ports prior to their use.
	 */
	void reserveAllOutPorts();

	/**
	 * Specify a set of output ports to lock prior to their use.
	 */
	void reserveOutPorts(std::set<size_t> outputs);

	/**
	 * Specify an output port to lock prior to its use.
	 */
	void reserveOutPort(size_t output);

//	bool tryLockOut() { return outMutex.tryLock(); }
//	void lockOut() { outMutex.lock(); }
//	void unlockOut() { outMutex.unlock(); }

	virtual bool yield() { Poco::Thread::yield(); return false; }

    virtual ModuleTask::RunningStates getRunningState() = 0;
    virtual void setRunningState(ModuleTask::RunningStates state) = 0;

private:
    /**
     * Forward tryLock for the given port
     *
     * outMutex is managed by the caller.
     *
     * This method is private,
     * for a direct call (e.g. from a Module), use reserveOutPort
     */
    bool tryOutPortLock(size_t portIndex);

	std::vector<OutPort*> outPorts; ///< list of output data ports
	Poco::ThreadLocal< std::set<size_t> > caughts; ///< store locked ports

	Poco::FastMutex outMutex; ///< lock output ports operations
};

#include "OutPortUser.ipp"

#endif /* SRC_OUTPORTUSER_H_ */
