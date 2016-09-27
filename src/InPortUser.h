/**
 * @file	src/InPortUser.h
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

#ifndef SRC_INPORTUSER_H_
#define SRC_INPORTUSER_H_

#include "Poco/Logger.h"
#include "Poco/ThreadLocal.h"
#include "Poco/Thread.h"
#include "Poco/Mutex.h"

#include <set>

class Module;
class InPort;
class DataSource;
class DataAttributeIn;

/**
 * Class managing multiple InPorts
 *
 * Deal with all the input-related operations of @ref Module.
 *
 *  - InPorts creation,
 *  - thread local access control: startCondition, tryCatch, read
 *  - release
 *
 * InPortUser::startCondition is a semi-automated method (virtual,
 * can be overloaded), that manages the possible module task starting
 * conditions.
 */
class InPortUser
{
public:
	InPortUser() { }
	virtual ~InPortUser();

	virtual std::string name() = 0;

	/**
	 * Get input ports
	 *
	 * The dispatcher should be requested to get shared pointer
	 * on the port items
	 * @return a copy of the input ports list
	 */
	std::vector<InPort*> getInPorts() { return inPorts; }

	/**
	 * Retrieve an input port given its name
	 */
	InPort* getInPort(std::string portName);

protected:
	/**
	 * Set the inPorts list size
	 */
	void setInPortCount(size_t cnt) { inPorts.resize(cnt, NULL); }

	/**
	 * Retrieve the input ports count
	 */
	size_t getInPortCount() { return inPorts.size(); }

	/**
	 * Add input data port
	 *
	 * @param name name of the port
	 * @param description description of the port
	 * @param dataType type of port data
	 * @param index index of the port in the inPorts list. It allows
	 * to use enums to access the ports.
	 */
	void addInPort(Module* parent,
	        std::string name, std::string description,
	        int dataType,
	        size_t index );

    /**
     * Add trig port
     *
     * To trig the running of the module
     * @param name name of the port
     * @param description description of the port
     * @param index index of the port in the inPorts list. It allows
     * to use enums to access the ports.
     */
	void addTrigPort(Module* parent,
            std::string name, std::string description,
            size_t index );

	/**
	 * Retrieve an input port, given its index
	 */
	InPort* getInPort(size_t index) { return inPorts.at(index); }

    /**
     * Forward tryLock for the given port
     *
     * To be called by startCondition.
     * The inMutex is managed by the caller.
     *
     * @throw ExecutionAbortedException if the input port requested a
     * lazy cancel of the InPortUser.
     */
    bool tryInPortCatchSource(size_t portIndex);

    /**
     * Forward readData for the given port
     */
    template <typename T>
    void readInPortData(size_t portIndex, T*& pData);

    /**
     * Forward readDataAttribute for the given port
     */
    void readInPortDataAttribute(size_t portIndex, DataAttributeIn* pAttr);

    /**
     * Forward release to the given port
     */
    void releaseInPort(size_t portIndex);

    /**
     * Release the lock of all the input ports
     */
    void releaseAllInPorts();

    /**
     * Release the lock of all the input ports with
     * releaseAllInPorts after having checked if the
     * trigging port is caught
     */
    void safeReleaseAllInPorts(InPort* triggingPort);

    /**
     * Check if the given port was caught/locked
     */
    bool isInPortCaught(size_t index)
    	{ return (caughts->find(index) != caughts->end()); }

    /**
     * Count how many input ports were caught/locked
     */
    size_t inPortCaughtsCount()
    	{ return caughts->size(); }

	std::set<size_t> inPortCoughts()
		{ return caughts.get(); }

    /// Start states as to be returned by startCondition
    enum baseStartStates
	{
    	noDataStartState,
//		unknownStartState,
		allDataStartState,
		firstUnusedBaseStartState // to be used to extend the start states with another enum
	};

	/**
	 * Define a start state regarding the input data
	 *
	 * This state is described by an index that should be defined
	 * as an enumeration in the implementation.
	 *
	 *     enum moreStartStates
	 *     {
	 *         firstStartState = firstUnusedBaseStartState,
	 *         secondStartState,
	 *         lastStartState
	 *     };
	 *
	 * Default implementation:
	 *  - check if there is input ports. if not, return noDataStartState.
	 *  - check if the call is issued from incoming data (check task's triggingPort).
	 *     - if not, return noDataStartState.
	 *     - if it does, wait for all the data to be available,
	 *       and then return allDataStartState
	 *
	 */
	virtual int startCondition();

	/**
	 * Reset the variable: starting,
	 * in order to handle the responsibility of the
	 * Module::taskStartingMutex release via startingUnlock
	 */
    void grabStartingMutex() { starting = true; }

    /// @see grabStartingMutex
	virtual void startingUnlock() = 0;

    /**
     * Release Module::taskStartingMutex via startingUnlock
     * if starting is set.
     */
    void releaseStartingMutex()
    {
        if (starting)
        {
            starting = false;
            startingUnlock();
        }
    }

    /**
     * Dispatch the cancellation to the sources
     */
    void cancelSources();

    /**
     * Wait for the sources being effectively cancelled
     */
    void waitCancelSources();

    /**
     * Dispatch the reseting to the sources
     */
    void resetSources();

    virtual bool yield() { Poco::Thread::yield(); return false; }

	/**
	 * Used by startCondition to know which port trigged the user execution
	 *
	 * Implemented in Module by forwarding the request to the running task.
	 */
	virtual InPort* triggingPort() = 0;

    virtual Poco::Logger& logger() = 0;

private:
	std::vector<InPort*> inPorts; ///< list of input ports
	Poco::ThreadLocal< std::set<size_t> > caughts; ///< store which ports are locked

    bool starting; ///< flag set to true is the ports release has to trig startingUnlock

};

#include "InPortUser.ipp"

#endif /* SRC_INPORTUSER_H_ */
