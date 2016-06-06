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

#include "VerboseEntity.h"

#include "Poco/ThreadLocal.h"
#include "Poco/Thread.h"

#include <set>

class Module;
class InPort;
class DataAttributeIn;

class InPortUser: public virtual VerboseEntity
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
     */
    bool tryInPortLock(size_t portIndex);

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
     * Forward tryData for the given port
     */
    template <typename T>
    bool tryInPortData(size_t portIndex, T*& pData, DataAttributeIn* pAttr);

    /**
     * Forward tryDataAttribute to the given port
     */
    bool tryInPortDataAttribute(size_t portIndex, DataAttributeIn* pAttr);

    /**
     * Forward release to the given port
     */
    void releaseInPort(size_t portIndex);

    /**
     * Release the lock of all the input ports
     */
    void releaseAllInPorts();

    /**
     * Check if the given port was caught/locked
     */
    bool isInPortCaught(size_t index) { return caughts->at(index); }

    /**
     * Reset the caughts flags
     *
     * To be called before any action with the input ports.
     *
     * Be careful not to call it anywhere!
     */
    void resetInPortLockFlags() { caughts->assign(inPorts.size(), false); }

    /// Start states as to be returned by startCondition
    enum baseStartStates
	{
    	noDataStartState,
		unknownStartState,
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
	 *  - check if there is input ports. if not, return.
	 *  - check if the call is issued from incoming data
	 *    - if it does, wait for all the data to be available
	 *    - if it does not, check if the data is held. if not at all,
	 *    return "no data", if partial, return "unknown",
	 *    if all, return "all data".
	 */
	virtual int startCondition();

	virtual bool yield() { Poco::Thread::yield(); return false; }

	/**
	 * Used by startCondition to know which port trigged the user execution
	 */
	virtual InPort* triggingPort() = 0;

	std::set<size_t> portsWithNewData();

private:
	std::vector<InPort*> inPorts; ///< list of input ports
	Poco::ThreadLocal< std::vector<bool> > caughts; ///< store which ports are locked
};

#include "InPortUser.ipp"

#endif /* SRC_INPORTUSER_H_ */
