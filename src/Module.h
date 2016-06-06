/**
 * @file	src/Module.h
 * @date	dec. 2015
 * @author	PhRG - opticalp.fr
 */

/*
 Copyright (c) 2015 Ph. Renaud-Goud / Opticalp

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

#ifndef SRC_MODULE_H_
#define SRC_MODULE_H_

#include "VerboseEntity.h"
#include "ParameterizedEntity.h"

#include "Port.h"

#include "InPortLockUnlock.h"
#include "OutPortLockUnlock.h"

#include "Poco/ThreadLocal.h"
#include "Poco/RWLock.h"
#include "Poco/Mutex.h"

#include <map>
#include <set>
#include <list>

class InDataPort;
class OutPort;
class ModuleFactory;
class ModuleTask;

/// Time lapse in milliseconds between 2 tries in multithread lock case
#define TIME_LAPSE 10

/**
 * Module
 *
 * Base class for any module.
 * A module is a single cell that processes data. It can be:
 *  - bound to a physical device: camera generating images,
 *  led controller, motion stage controller, ... i.e. either
 *  actuators or sensors
 *  - a computing cell: image processing, signal processing,
 *  ...
 *
 * The modules are closely bound to their ModuleTask s.
 * The module lists the tasks to be launched, and even can launch
 * the tasks when needed. The tasks are the only one to run the
 * modules (after having set runningTask).
 */
class Module: public virtual VerboseEntity, public ParameterizedEntity
{
public:
	/**
	 * Constructor
	 *
	 * The implementation should at least do the following:
	 *  - explicitly call this constructor
	 *  - generate a name that will be returned by internalName()
	 *  - set the logger
	 *  - call @ref setCustomName after the internalName is set
	 *  - call @ref notifyCreation to let the module being registered
	 *  in the managers
	 *
	 * @param parent Parent module factory
	 * @param name Name to be used as task name. E.g. use the custom name
	 *
	 * @throw Poco::Exception forwarded from setCustomName or setInternalName
	 * if customName or internalName is already in use.
	 */
	Module(ModuleFactory* parent, std::string name = ""):
	      mParent(parent), ParameterizedEntity("module." + name)
	{
	}

	/**
	 * Destructor
	 *
	 * Notify any entity that could store a pointer on this.
	 *  - parent factory
	 *  - module manager
	 *  - dispatcher
	 *  - tasks
	 */
	virtual ~Module();

	/**
	 * Custom name of the module
	 *
	 * @return Name defined by the user at creation time
	 * @see setCustomName()
	 */
	std::string name() { return mName; }

	/**
	 * Internal name of the module
	 *
	 * This name may include dynamic content (e.g. index number...)
	 * since no duplicates are allowed.
	 * @return Name computed by the module itself regarding internal
	 * considerations
	 * @see setInternalName()
	 */
	std::string internalName() { return mInternalName; }

	/**
	 * Description of the module
	 *
	 * This description does not include dynamic content bound to
	 * parameters (factory parameters or module parameters).
	 * It should be static in the derived classes,
	 * so it can be queried by the factory
	 * before module creation.
	 * @return full description of the module: functionalities, usage
	 */
	virtual std::string description() = 0;

	/**
	 * Get parent ModuleFactory
	 *
	 * e.g. to get factory parameters
	 */
	ModuleFactory* parent();

	/**
	 * Get input ports
	 *
	 * The dispatcher should be requested to get shared pointer
	 * on the port items
	 * @return a copy of the input ports list
	 */
	std::vector<InPort*> getInPorts() { return inPorts; }
	InPort* getInPort(std::string portName);

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

    /**
     * Enqueue a new task
     *
     * for this module.
     * To be called by the disptacher.
     * @return true if the module is not running, and thus,
     * the new enqueued task should be started.
     */
    bool enqueueTask(ModuleTask* task);

    /**
     * Unregister a task
     *
     * called by the task destructor
     */
    void unregisterTask(ModuleTask* task);

    /**
     * Launch the next task of the queue
     *
     * and dequeue it.
     */
    void popTask();

    /**
     * Launch the next task of the queue in the current thread
     *
     * and dequeue it.
     */
    void popTaskSync();

protected:
    Poco::ThreadLocal<ModuleTask*> runningTask;

    void setRunningTask(ModuleTask* pTask) { *runningTask = pTask; }

    /// @name forwarding methods to thread local: runningTask
    ///@{
    bool sleep(long milliseconds);
    bool yield();
	void setProgress(float progress);
    bool isCancelled();
    ///}

    // TODO: method to forward the running state to the runningTask (collectingInData, ... etc)

    /**
     * Merge the enqueued tasks of the present Module
     *
     * given by their triggering inPort index.
     *
     *  - The current task can be listed in this set.
     *  - a test should be made, so that only the inPorts
     *  giving new data are listed (they should have been
     *  trigged simultaneously)
     *  - but a held port can be sent too. a simple warning
     *  would be emitted.
     *
     * @inPortIndexes indexes of the inPorts which data will be used
     */
    void mergeTasks(std::set<int> inPortIndexes);

    /// Start states as to be returned by startCondition
    enum baseStartStates
	{
    	cancelledStartState = -1,
    	noDataStartState,
		unknownStartState,
		allDataStartState,
		firstUnusedBaseStartState // to be used to extend the start states with another enum
	};

	/**
	 * Implementation of Poco::Task::runTask()
	 *
	 * Lock the runTaskMutex, expire the output data,
	 * and call process().
	 *
	 * The arguments of process allow to automatically release
	 * the data of the ports when exiting (even on exception thrown)
	 *
	 * This function is virtual. it can be overloaded by
	 * the derived class if the runTaskMutex lock is not wanted.
	 */
	virtual void run();

	/**
	 * Main logic called by runTask
	 *
	 * with a lock on runTaskMutex.
	 * inPortsAccess and outPortsAccess should be used to
	 * access the ports data, since it will handle the
	 * unlocking of the ports in case of un-clean stop
	 *
	 * The enqueued tasks trigged by the input data that
	 * are used here shall be merged.
	 */
	virtual void process(InPortLockUnlock& inPortsAccess,
	        OutPortLockUnlock& outPortsAccess)
	    { poco_warning(logger(), name() + ": nothing to be done"); }

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
	virtual int startCondition(InPortLockUnlock& inPortsAccess);

    /**
     * Set the internal name of the module
     *
     * This method has to be called by the constructor
     * before setCustomName()
     * @throw Poco::ExistsException If internalName is already in use
     * @throw Poco::SyntaxException If internalName contains bad characters
     */
    void setInternalName(std::string internalName);

	/**
	 * Set the custom name of the module
	 *
	 * This method has to be called by the constructor.
	 * If customName is empty, use internalName().
     * @throw Poco::ExistsException If customName is already in use
     * @throw Poco::SyntaxException If customName contains bad characters
	 */
	void setCustomName(std::string customName);

	/**
	 * Notify the managers about the module creation
	 *
	 * Has to be called at the end of the constructors implementation
	 * in order to be registered in the ModuleManager and
	 * in the Dispatcher
	 */
	void notifyCreation();

	/**
	 * Set the inPorts list size
	 */
	void setInPortCount(size_t cnt) { inPorts.resize(cnt, NULL); }
	/**
	 * Add input data port
	 *
	 * @param name name of the port
	 * @param description description of the port
	 * @param dataType type of port data
	 * @param index index of the port in the inPorts list. It allows
	 * to use enums to access the ports.
	 */
	void addInPort(
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
	void addTrigPort(
            std::string name, std::string description,
            size_t index );

    /**
     * Set the outPorts list size
     */
    void setOutPortCount(size_t cnt) { outPorts.resize(cnt, NULL); }
    /**
     * Add output data port
     *
     * @param name name of the port
     * @param description description of the port
     * @param dataType type of port data
     * @param index index of the port in the outPorts list. It allows
     * to use enums to access the ports.
     */
    void addOutPort(
            std::string name, std::string description,
            int dataType,
            size_t index );

    Poco::Mutex runTaskMutex; ///< mutex used to block runTask(), and then, process()
private:
    /// enum to be returned by checkName
    enum NameStatus
    {
        nameOk,
        nameExists,
        nameBadSyntax
    };

    /**
     * Check if the given name is allowed
     *
     *  - verify that the syntax is ok
     *  - verify that the name is not already in use
     */
    NameStatus checkName(std::string newName);

    /**
     * Remove the internal name of the names list
     *
     * To be called by setCustomName in case of failing module creation
     * process due to malformed custom name, or name already in use.
     */
    void freeInternalName();

	/**
	 * Parent module factory
	 *
	 * - to be notified when the module is deleted
	 * - to get the factory parameters (selectors, evtl parameters)
	 */
	ModuleFactory* mParent;

	std::string mInternalName; ///< internal name of the module
	std::string mName; ///< custom name of the module

	std::vector<InPort*> inPorts; ///< list of input ports
	std::vector<OutPort*> outPorts; ///< list of output data ports

	static std::vector<std::string> names; ///< list of names of all modules
	static Poco::RWLock namesLock; ///< read write lock to access the list of names

	/// Store the tasks assigned to this module. See registerTask(), unregisterTask()
	std::set<ModuleTask*> allTasks;
	std::list<ModuleTask*> taskQueue;
	Poco::FastMutex taskMngtMutex;

    friend class ModuleTask; // access to setRunningTask
};

#endif /* SRC_MODULE_H_ */
