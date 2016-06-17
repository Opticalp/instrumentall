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
#include "InPortUser.h"
#include "OutPortUser.h"
#include "ModuleTask.h"

#include "Poco/ThreadLocal.h"
#include "Poco/RWLock.h"
#include "Poco/Mutex.h"

#include <map>
#include <set>
#include <list>

class ModuleFactory;

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
class Module: public virtual VerboseEntity,
	public ParameterizedEntity,
	public InPortUser, public OutPortUser
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
	      mParent(parent), ParameterizedEntity("module." + name),
		  procMode(fullBufferedProcessing),
		  startSyncPending(false),
		  enqueuing(false)
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
     * Enqueue a new task
     *
     * for this module. and pop it, if no task is running for this module.
     * To be called by the disptacher.
     * @return true if the module is not running, and thus,
     * the new enqueued task should be started.
     */
    void enqueueTask(ModuleTask* task);

    /**
     * Unregister a task
     *
     * called by the task destructor
     */
    void unregisterTask(ModuleTask* task);

    /**
     * Launch the next task of the queue in the current thread
     *
     * and dequeue it.
     */
    void popTaskSync();

    /**
     * Processing modes:
     *
     * - direct: the input ports are locked, the output ports are locked,
     * and the processing occurs directly from the input data into the output
     * data
     * - buffered in: the input data is duplicated, the input port is released,
     * and then the processing occurs.
     * - buffered out: the processing occurs into a buffer. Then the output
     * port is locked and the data is copied into the outport.
     * - full buffered: combination of buffered in and buffered out
     * - other: any other processing way.
     *
     * Not all the processing modes are relevant... Mainly depending on the presence
     * of input/output ports and of course depending on the processing itself.
     *
     * The main difference between those modes is not really the memory footprint
     * (it can be, in some cases), but the time during which the ports are locked.
     */
    enum ProcessingMode
	{
    	directProcessing,
		bufferedInProcessing = 1,
		bufferedOutProcessing = 1 << 1,
		fullBufferedProcessing = bufferedInProcessing | bufferedOutProcessing,
		otherProcessing
	};

    /**
     * Set the processing mode
     *
     * can be overloaded to execute some checks or restrictions.
     * The parent method should be called, though.
     */
    virtual void setProcMode(ProcessingMode mode) { procMode = mode; }
    ProcessingMode getProcMode() { return procMode; }

    /**
     * Parameterized entity needs this definition to be able
     * to use OutPortUser::expireOutData
     */
    void expireOutData() { OutPortUser::expireOutData(); }

protected:
	void addInPort(
			std::string name, std::string description,
	        int dataType,
	        size_t index )
	{ InPortUser::addInPort(this, name, description, dataType, index); }

	void addTrigPort(
            std::string name, std::string description,
            size_t index )
	{ InPortUser::addTrigPort(this, name, description, index); }

    void addOutPort(
            std::string name, std::string description,
            int dataType,
            size_t index )
	{ OutPortUser::addOutPort(this, name, description, dataType, index); }

    Poco::ThreadLocal<ModuleTask*> runningTask;

	/**
	 * Set the running task thread local pointer. 
	 *
	 * To be set to NULL in case of need to call the task forwarding functions. 
	 * 
	 * e.g. when calling reserveOutPorts() from setParameter
	 */
    void setRunningTask(ModuleTask* pTask) { *runningTask = pTask; }

    /// @name forwarding methods to thread local: runningTask
    ///@{
    bool sleep(long milliseconds);
    bool yield();
	void setProgress(float progress);
    bool isCancelled();
	void cancel();
    InPort* triggingPort();
    void setRunningState(ModuleTask::RunningStates state);
    ModuleTask::RunningStates getRunningState();
    ///}

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
    void mergeTasks(std::set<size_t> inPortIndexes);

    /**
     * Launch the next task of the queue
     *
     * and dequeue it.
     *
     * Do nothing if the queue is empty.
     */
    void popTask();

	/**
	 * Method called by the task.
	 *
	 * Expire the output data, check the start condition,
	 * and call process().
	 *
	 * There is no mutex locked here. Concurrence running should be OK.
	 */
	void run();

	/**
	 * Main logic called by run
	 *
	 * Depending on the processing mode, the data should be buffered or
	 * not.
	 *
	 * Access to input ports data should be done via readInPort and
	 * readInPortDataAttribute. Call releaseAllInPorts when over.
	 *
	 * Access to output ports should be granted via reserveOutPorts
	 * and then data pointer should be accessed with getDataToWrite.
	 * Call notifyAllOutPortReady when over.
	 *
	 * @param startCond start condition as defined in virtual method
	 * startCondition. You should consider implementing your own
	 * startCondition method for non-standard cases.
	 *
	 * It could be wised to use a lock in this method if there is a
	 * chance (popTask()?) that it is called twice at the same time.
	 * No thread-safety is given by the caller.
	 *
	 * @see startCondition
	 * @see setRunningState
	 * @see getProcMode
	 */
	virtual void process(int startCond)
		{ poco_warning(logger(), name() + ": nothing to be done"); }

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

	ProcessingMode procMode;

	static std::vector<std::string> names; ///< list of names of all modules
	static Poco::RWLock namesLock; ///< read write lock to access the list of names

	/**
	 * Check if a task is already running (or at least started)
	 * for this module
	 *
	 * taskMngtMutex should be locked prior to calling this method
	 */
	bool taskIsRunning();

	/// Store the tasks assigned to this module. See registerTask(), unregisterTask()
	std::set<ModuleTask*> allTasks;
	std::list<ModuleTask*> taskQueue;
	Poco::Mutex taskMngtMutex; ///< recursive mutex. lock the task management. Recursive because of its use in Module::enqueueTask
	bool startSyncPending; ///< flag used by start sync to know that the tasMngLock is kept locked
	bool enqueuing; ///< flag used during task enqueuing @see Dispatcher::enqueueModuleTask

    friend class ModuleTask; // access to setRunningTask, releaseAll
};

#endif /* SRC_MODULE_H_ */
