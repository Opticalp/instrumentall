/**
 * @file	src/Module.cpp
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

#include "Module.h"
#include "ModuleFactory.h"
#include "ModuleManager.h"
#include "ThreadManager.h"
#include "ModuleTask.h"

#include "OutPort.h"

#include "Poco/NumberFormatter.h"
#include "Poco/NumberParser.h"

std::vector<std::string> Module::names;
Poco::RWLock Module::namesLock;

void Module::notifyCreation()
{
    // if not EmptyModule, add this to module manager
    if (mParent)
        Poco::Util::Application::instance().getSubsystem<ModuleManager>().addModule(this);
}

Module::~Module()
{
	// TODO: tasks?

    // notify parent factory
    if (mParent)
    {
        mParent->removeChildModule(this);

        // notify module manager
        Poco::Util::Application::instance().getSubsystem<ModuleManager>().removeModule(this);
    }
}

#include "Poco/Exception.h"

void Module::setInternalName(std::string internalName)
{
    namesLock.writeLock();

    switch (checkName(internalName))
    {
    case nameOk:
        mInternalName = internalName;
        names.push_back(mInternalName);
        namesLock.unlock();
        return;
    case nameExists:
        namesLock.unlock();
        throw Poco::ExistsException("setInternalName",
                internalName + " already in use");
    case nameBadSyntax:
        namesLock.unlock();
        throw Poco::SyntaxException("setInternalName",
                "The name should only contain alphanumeric characters "
                "or \"-\", \"_\", \".\"");
    }
}

void Module::setCustomName(std::string customName)
{
    if (customName.empty() || customName.compare(internalName())==0)
    {
        mName = internalName();

        // update conf file prefix key
        setPrefixKey("module." + mName);

        return;
    }

    namesLock.writeLock();

    switch (checkName(customName))
    {
    case nameOk:
        mName = customName;
        names.push_back(mName);
        namesLock.unlock();
        return;
    case nameExists:
        freeInternalName();
        namesLock.unlock();
        throw Poco::ExistsException("setCustomName",
                customName + " already in use");
    case nameBadSyntax:
        freeInternalName();
        namesLock.unlock();
        throw Poco::SyntaxException("setCustomName",
                "The name should only contain alphanumeric characters "
                "or \"-\", \"_\", \".\"");
    }
}

ModuleFactory* Module::parent()
{
    if (mParent)
        return mParent;
    else
        throw Poco::InvalidAccessException("parent",
                "This module has no valid parent factory");
}

#include "Poco/RegularExpression.h"

Module::NameStatus Module::checkName(std::string newName)
{
    // check syntax
    Poco::RegularExpression regex("^[0-9a-zA-Z\\._-]+$");
    if (!regex.match(newName))
        return nameBadSyntax;

    // check existance
    for (std::vector<std::string>::iterator it = names.begin(),
            ite = names.end(); it != ite; it++)
        if (it->compare(newName)==0)
            return nameExists;

    return nameOk;
}

void Module::freeInternalName()
{
    // verify that this is a module creation process
    if (!name().empty())
        return;

    for (std::vector<std::string>::reverse_iterator it = names.rbegin(),
            ite = names.rend(); it != ite; it++ )
    {
        if (it->compare(internalName())==0)
        {
            names.erase((it+1).base());
            return;
        }
    }
}

void Module::releaseProcessingMutex()
{
    if (processing)
    {
        processing = false;
        processingUnlock();
    }
}

void Module::prepareTaskStart(ModuleTask* pTask)
{
    taskMngtMutex.lock();
    if (startSyncPending)
    {
        startSyncPending = false;
        taskMngtMutex.unlock();
    }
    taskMngtMutex.unlock();

    setRunningTask(pTask);

    while (!taskProcessingMutex.tryLock())
    {
        if (yield()) // startingTask reset by moduleReset
            throw ExecutionAbortedException(pTask->name() + "::prepareTask",
                    "task cancellation during taskStartingMutex lock wait");

        if (pTask->isSlave())
        {
            Poco::Mutex::ScopedLock lock(taskMngtMutex);
            startingTask = NULL;

            throw TaskMergedException("Task merged. "
                    "Start skipped. ");
        }
    }

    taskMngtMutex.lock();
    startingTask = NULL;
    processing = true;
    taskMngtMutex.unlock();
}

void Module::run(ModuleTask* pTask)
{
	int startCond;

	try
	{
        if (isCancelled())
            throw ExecutionAbortedException(name() +
                    ": can not run a new task, "
                    "the module is cancelling");

        resetDone = false;

        setRunningState(ModuleTask::applyingParameters);
		waitParameters();

		setRunningState(ModuleTask::retrievingInDataLocks);

		startCond = startCondition();
	}
    catch (...)
    {
        parametersTreated();
        safeReleaseAllInPorts(triggingPort());
        releaseProcessingMutex();
        throw;
    }

    parametersTreated();


    try
    {
		if (isCancelled())
			throw ExecutionAbortedException(name() +
					": can not run a new task, "
					"the module is cancelling (2)");

		setRunningState(ModuleTask::processing);
		process(startCond);
	}
	catch (...)
	{
		releaseAllInPorts();
		releaseAllOutPorts();
        releaseProcessingMutex();
        releaseOutputMutex();
		throw;
	}

	releaseAllInPorts();
	releaseAllOutPorts();
    releaseProcessingMutex();
    releaseOutputMutex();
}

bool Module::sleep(long milliseconds)
{
	bool ret = false;

	if ((*runningTask) != NULL)
		ret = (*runningTask)->sleep(milliseconds);
	else
		Poco::Thread::sleep(milliseconds);

	if (immediateCancelling || cancelDoneEvent.tryWait(0))
		return true;
	else
		return ret;
}

bool Module::yield()
{
	bool ret = false;

    if ((*runningTask) != NULL)
		ret = (*runningTask)->yield();
	else
		Poco::Thread::yield();

	if (immediateCancelling || cancelDoneEvent.tryWait(0))
		return true;
	else
		return ret;
}

void Module::setProgress(float progress)
{
    if ((*runningTask) == NULL)
		return;

	(*runningTask)->setProgress(progress);
}

bool Module::isCancelled()
{
	if (immediateCancelling || cancelDoneEvent.tryWait(0))
		return true;

    if ((*runningTask) == NULL)
		return false;

	return (*runningTask)->isCancelled();
}

InPort* Module::triggingPort()
{
    if ((*runningTask) != NULL)
		return (*runningTask)->triggingPort();

	throw Poco::InvalidAccessException(name(), "querying trigging port. not in a task");
}

void Module::setRunningState(ModuleTask::RunningStates state)
{
    if ((*runningTask) == NULL)
		return;

	(*runningTask)->setRunningState(state);
}

ModuleTask::RunningStates Module::getRunningState()
{
    if ((*runningTask) == NULL)
		return ModuleTask::processing;

	return (*runningTask)->getRunningState();
}

void Module::enqueueTask(ModuleTaskPtr& pTask, bool syncAllowed)
{
	if (!moduleReady())
		throw ExecutionAbortedException(name(),
				"enqueue task " + pTask->name() + ": module is cancelling (or reseting)");

	Poco::ScopedLockWithUnlock<Poco::Mutex> lock(taskMngtMutex);
    // thread safety: no deadlock risk with ThreadManager::registerNewModuleTask

    // take ownership of the task.
	try
	{
	    Poco::Util::Application::instance()
                             .getSubsystem<ThreadManager>()
                             .registerNewModuleTask(pTask);
	}
	catch (Poco::RuntimeException& exc)
	{
	    poco_error(logger(), name() + "::enqueueTask: "
	            "ThreadManager::registerNewModuleTask failed: "
	            + exc.displayText());
        throw;
	}

	taskQueue.push_back(pTask);

	bool queueWithManyTasks = (taskQueue.size() > 1);
    lock.unlock();

	if (queueWithManyTasks)
	{
		poco_information(logger(), pTask->name()
					+ ": tasks are already enqueued for " + name());
		return;
	}

    if (syncAllowed)
        popTaskSync();
    else
        popTask();
}

bool Module::taskIsStarting()
{
    Poco::Mutex::ScopedLock lock(taskMngtMutex); // Ok: recursive mutex

    if (!startingTask.isNull())
    {
        poco_information(logger(), name() + ": a task is already starting ("
                + startingTask->name() + ")");
        return true;
    }

    for (std::set<ModuleTaskPtr>::iterator it = allLaunchedTasks.begin(),
            ite = allLaunchedTasks.end(); it != ite; it++)
    {
        switch ((*it)->getState())
        {
        case MergeableTask::TASK_STARTING:
            //poco_information(logger(), task->name()
            //      + ": " + (*it)->name()
            //      + " is already starting for " + name());
            return true;
        case MergeableTask::TASK_RUNNING:
        case MergeableTask::TASK_FALSE_START:
        case MergeableTask::TASK_IDLE: // probably self
        case MergeableTask::TASK_CANCELLING:
        case MergeableTask::TASK_FINISHED:
        case MergeableTask::TASK_MERGED:
            break;
        default:
            poco_bugcheck_msg("unknown task state");
        }
    }

    return false;
}

bool Module::taskIsRunning()
{
	Poco::Mutex::ScopedLock lock(taskMngtMutex); // Ok: recursive mutex

	for (std::set<ModuleTaskPtr>::iterator it = allLaunchedTasks.begin(),
			ite = allLaunchedTasks.end(); it != ite; it++)
	{
		switch ((*it)->getState())
		{
		case MergeableTask::TASK_STARTING:
		case MergeableTask::TASK_RUNNING:
			//poco_information(logger(), task->name()
			//		+ ": " + (*it)->name() 
			//		+ " is already running for " + name());
			return true;
		case MergeableTask::TASK_FALSE_START:
		case MergeableTask::TASK_IDLE: // probably self
		case MergeableTask::TASK_CANCELLING:
		case MergeableTask::TASK_FINISHED:
		case MergeableTask::TASK_MERGED:
			break;
		default:
			poco_bugcheck_msg("unknown task state");
		}
	}

	return false;
}

void Module::popTask()
{
	// n.b.: the taskMngtMutex is locked during immediateCancel.
	// when released, the taskQueue is empty.
    taskMngtMutex.lock();

    if (taskQueue.empty())
    {
        if (logger().information())
        {
            if ((*runningTask) == NULL)
                poco_information(logger(),
                        name() + ": empty task queue, nothing to pop (async)");
            else
                poco_information(logger(),
                        (*runningTask)->name() + ": empty task queue, nothing to pop (async)");
        }
        taskMngtMutex.unlock();
        return;
    }

    if (taskIsStarting()) // taskMngtMutex is recursive. this call is ok.
    {
        poco_information(logger(),"pop (async): a task is already starting. ");
        taskMngtMutex.unlock();
        return;
    }

	ModuleTaskPtr nextTask = taskQueue.front();
	poco_information(logger(), "poping out the next task: " + nextTask->name());
	taskQueue.pop_front();

	startingTask = nextTask;
	allLaunchedTasks.insert(nextTask);

	try
	{
	    Poco::Util::Application::instance()
					 .getSubsystem<ThreadManager>()
					 .startModuleTask(nextTask);
	}
	catch (...)
	{
        poco_error(logger(), "can not start (async) " + nextTask->name());
        taskMngtMutex.unlock();
	    throw;
	}

	taskMngtMutex.unlock();
}

void Module::popTaskSync()
{
	// n.b.: the taskMngtMutex is locked during immediateCancel.
	// when released, the taskQueue is empty.
	taskMngtMutex.lock();

    if (taskQueue.empty())
    {
        if (logger().information())
        {
            if ((*runningTask) == NULL)
                logger().information(
                        name() + ": empty task queue, nothing to sync pop");
            else
                logger().information(
                        (*runningTask)->name() + ": empty task queue, nothing to sync pop");
        }

        taskMngtMutex.unlock();
        return;
    }

	if (taskIsRunning()) // taskMngtMutex is recursive. this call is ok.
	{
		poco_information(logger(),"sync pop: a task is already running (or starting). ");
		taskMngtMutex.unlock();
		return;
	}

	ModuleTaskPtr nextTask = taskQueue.front();
	poco_information(logger(), "SYNC poping out the next task: " + nextTask->name());
	taskQueue.pop_front();

    startingTask = nextTask;
    allLaunchedTasks.insert(nextTask);

	startSyncPending = true;

	try
	{
		Poco::Util::Application::instance()
				 .getSubsystem<ThreadManager>()
				 .startSyncModuleTask(nextTask);

		// if startSyncModuleTask succeeds, 
		//       taskMngtMutex.unlock();
		// is called in Module::run according to startSyncPending value. 
	}
	catch (...)
	{
		poco_error(logger(), "can not sync start " + nextTask->name());
		startSyncPending = false;
		taskMngtMutex.unlock();
		throw;
	}
}

void Module::unregisterTask(ModuleTask* pTask)
{
	Poco::Mutex::ScopedLock lock(taskMngtMutex);
	allLaunchedTasks.erase(ModuleTaskPtr(pTask, true));
}

bool Module::tryCatchInPortFromQueue(InPort* trigPort)
{
    Poco::Mutex::ScopedLock lock(taskMngtMutex);

    // check the starting task first
    if (!startingTask.isNull())
    {
        poco_information(logger(), startingTask->name()
                + " is starting for " + name() );

        if (startingTask->triggingPort() == trigPort)
        {
            try
            {
                Poco::AutoPtr<MergeableTask> slave(startingTask);
                (*runningTask)->merge(slave);
            }
            catch (Poco::RuntimeException& e)
            {
                poco_information(logger(), "potential slave task found: "
                        + startingTask->name() + ", merging with master: "
                        + (*runningTask)->name() + " failed: "
                        + e.displayText());
                return false;
            }

            poco_information(logger(), "slave task found (starting): "
                    + startingTask->name() + ", merging with master: "
                    + (*runningTask)->name() + " OK");

            // FIXME: replace by something contained in the merged task.
            trigPort->tryCatchSource();

            return true;
        }
    }

    // seek in the pending tasks queue
    for (std::list<ModuleTaskPtr>::iterator qIt = taskQueue.begin(),
            qIte = taskQueue.end(); qIt != qIte; qIt++)
    {
        poco_information(logger(), name() + "'s taskQueue includes: " + (*qIt)->name());
        if ((*qIt)->triggingPort() == trigPort)
        {
            try
            {
                Poco::AutoPtr<MergeableTask> slave(*qIt);
                (*runningTask)->merge(slave);
            }
            catch (Poco::RuntimeException& e)
            {
                poco_information(logger(), "potential slave task found in queue: "
                        + (*qIt)->name() + ", merging with master: "
                        + (*runningTask)->name() + " failed: "
                        + e.displayText());
                return false;
            }

            poco_information(logger(), "slave task found (in queue): "
                    + (*qIt)->name() + ", merging with master: "
                    + (*runningTask)->name() + " OK");

            allLaunchedTasks.insert(*qIt);
            taskQueue.erase(qIt); // taskMngtMutex is locked. The order (with Task::merge) is not that important.

			// FIXME: replace by something contained in the merged task. 
			trigPort->tryCatchSource();

            return true;
        }
    }

    return false;
}

Poco::AutoPtr<ModuleTask> Module::runModule(bool syncAllowed)
{
    ModuleTaskPtr taskPtr(new ModuleTask(this));
    enqueueTask(taskPtr, syncAllowed);

    return taskPtr;
}

void Module::waitParameters()
{
	while (!tryAllParametersSet())
	{
		if (yield())
			throw ExecutionAbortedException(name(),
					"Apply parameters: Cancellation upon user request");
	}

	applyParameters();
}

void Module::cancelled()
{
	Poco::Mutex::ScopedLock lock(taskMngtMutex);
	if (taskQueue.size())
		poco_bugcheck_msg(
				(name() + ": try to set as cancelled "
 				 "although tasks remain enqueued").c_str() );

	cancelDoneEvent.set();
	cancelling = false;
	immediateCancelling = false;
}

void Module::waitCancelled(bool topCall)
{
    if (reseting || resetDone)
    {
        poco_information(logger(), name() + ".waitCancelled: a reset just occured. ");
        return;
    }

    if (cancelling)
        cancelDoneEvent.wait();

    if (topCall || (waitingCancelled == 0))
    {
        waitingCancelled++;
        poco_information(logger(), name() + ".waitCancelled: wait for sources... ");
        waitCancelSources();
        poco_information(logger(), name() + ".waitCancelled: wait for targets... ");
        waitCancelTargets();
        poco_information(logger(), name() + ".waitCancelled: done. ");
        waitingCancelled--;
    }
    else
        poco_information(logger(), name() + ".waitCancelled skipped");
}

bool Module::moduleReady()
{
    return !(immediateCancelling || cancelling || cancelDoneEvent.tryWait(0) || reseting);
}

bool Module::immediateCancel()
{
	poco_information(logger(), name() + ".immediateCancel request");

	if (immediateCancelling)
	{
		poco_information(logger(), name() + " already immediate cancelling... return");
		return false;
	}

    if (reseting)
    {
        poco_information(logger(), name() + " already reseting... return");
        return false;
    }

	bool previousCancelReq = cancelRequested;
    cancelRequested = true; // to be set before immediateCancelling. See cancellationListen
	immediateCancelling = true;

	if (cancelDoneEvent.tryWait(0))
	{
		poco_information(logger(), name() + " already cancelled... return");
		cancelRequested = previousCancelReq;
		immediateCancelling = false;
		return false;
	}

	if (cancelling)
		poco_information(logger(), name() + " already cancelling... "
				"Trig immediate cancelling then. ");
	else
	{
	    resetDone = false;
        cancelling = true;
	}

	cancelSources();
    poco_information(logger(), name() + ".immediateCancel: "
            "cancellation request dispatched to the sources");
	cancelTargets();
    poco_information(logger(), name() + ".immediateCancel: "
            "cancellation request dispatched to the targets");

	taskMngtMutex.lock();

	// delete pending tasks
	while (taskQueue.size())
	{
		InPort* port = taskQueue.front()->mTriggingPort;

		// FIXME: no need to releaseInputDataOnStartFailure?
		if (port)
			port->releaseInputData();

		Poco::Util::Application::instance()
	                             .getSubsystem<ThreadManager>()
	                             .unregisterModuleTask(taskQueue.front());
		taskQueue.pop_front();
	}

	// stop active tasks
	for (std::set<ModuleTaskPtr>::iterator it = allLaunchedTasks.begin(),
			ite = allLaunchedTasks.end(); it != ite; it++)
	{
	    if (*it == runningTask.get())
	        continue;

		switch ((*it)->getState())
		{
		case MergeableTask::TASK_IDLE:
		case MergeableTask::TASK_STARTING:
		case MergeableTask::TASK_RUNNING:
		    ModuleTaskPtr(*it)->cancel();
			break;
		case MergeableTask::TASK_FALSE_START:
		case MergeableTask::TASK_FINISHED:
		case MergeableTask::TASK_CANCELLING:
		default:
			break;
		}
	}

	taskMngtMutex.unlock();

	resetDone = false;

	try
	{
		cancel(); // shall trig Module::cancelled
        cancelRequested = false;
	}
	catch (...)
	{
        cancelRequested = false;
		poco_bugcheck_msg("Module::cancel shall not throw exceptions");
	}

    if (!cancellationListenerThread.isRunning())
        cancellationListenerThread.start(cancellationListenerRunnable);

	poco_information(logger(), name() + ".immediateCancel: listener started for end-of-cancellation...");
	return true;
}

void Module::lazyCancel()
{
	if (immediateCancelling || cancelling)
	{
		poco_information(logger(), name() + ".lazyCancel: already cancelling... return");
		return;
	}

    if (reseting)
    {
        poco_information(logger(), name() + " already reseting... return");
        return;
    }

    resetDone = false;
	cancelling = true;

	// check if the module was recently cancelled
	if (cancelDoneEvent.tryWait(0))
	{
		poco_information(logger(), name() + ".lazyCancel: already cancelled... return");
		cancelling = false;
		return;
	}

	cancelSources();
    poco_information(logger(), name() + ".lazyCancel: "
            "cancellation request dispatched to the sources");

    if (!cancellationListenerThread.isRunning())
        cancellationListenerThread.start(cancellationListenerRunnable);

	poco_information(logger(), name() + ".lazyCancel: listener started for end-of-cancellation...");
}

void Module::cancellationListen()
{
    poco_information(logger(), name() + ": wait for all tasks to terminate");

    int counter = 0;
    while (taskIsRunning())
    {
        Poco::Thread::yield();
        if (counter++ % 500 == 0)
            poco_information(logger(), "waiting for " + name() + " tasks to finish");
    }

    if (!immediateCancelling)
    {
        poco_information(logger(), name() + " lazyCancel listener: "
                "tasks stopped. propagate to targets. ");
        cancelTargets();
        poco_information(logger(), name() + " lazyCancel listener: "
                "cancellation request dispatched to the targets");
        cancelled();
        poco_information(logger(), name() + " lazyCancel: "
                "cancellation effective.");
    }
    else
    {
        poco_information(logger(), name() + " immediateCancel listener: "
                "tasks stopped.");

        while (cancelRequested)
        {
            Poco::Thread::yield();
            if (counter++ % 500 == 0)
                poco_information(logger(), "waiting for " + name() + ".cancel to return");
        }

        cancelled();
        poco_information(logger(), name() + " immediateCancel listener: "
                "cancellation effective.");
    }
}

void Module::moduleReset()
{
    poco_information(logger(), "entering " + name() + ".moduleReset");

	if (reseting)
	{
		poco_information(logger(), name() + " already reseting... return");
		return;
	}

	reseting = true;

	if (resetDone)
	{
		poco_information(logger(), name() + " already reset... return");
		reseting = false;
		return;
	}

	if (!cancelDoneEvent.tryWait(0))
	{
		poco_warning(logger(), name() + ".moduleReset: "
				"cancellation not over... "
				"Please check that waitCancelled was called first. "
				"Or the previous reset is over since a while, and a "
				"new task overriden it. " );
		reseting = false;
		return;
	}

	if (taskQueue.size())
		poco_bugcheck_msg(
				(name() + ": try to reset "
 				 "although tasks are enqueued").c_str() );

	resetTargets();
	try
	{
	    startingTask = NULL;
	    processing = false;
		reset();
	}
	catch (...)
	{
		poco_bugcheck_msg("Module::reset shall not throw exceptions");
	}
	resetSources();

    cancelDoneEvent.reset();
	resetDone = true;
	reseting = false;
}

void Module::processingTerminated()
{
    outputMutex.lock();
    outputLocked = true;
    popTask();
    releaseProcessingMutex();
}

void Module::releaseOutputMutex()
{
    if (outputLocked)
    {
        outputLocked = false;
        outputMutex.unlock();
    }
}
