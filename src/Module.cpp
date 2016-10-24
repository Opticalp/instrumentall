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
    if (startSyncPending->isSet())
    {
        startSyncPending->reset();
        taskMngtMutex.unlock();
    }

    setRunningTask(pTask);

    while (!taskProcessingMutex.tryLock())
    {
        if (yield())
        {
            // release trigging port
            if (pTask->triggingPort())
            	safeReleaseInPort(pTask->triggingPort()->index());

            Poco::Mutex::ScopedLock lock(taskMngtMutex);
            startingTask = NULL;

            throw ExecutionAbortedException(pTask->name() + "::prepareTask",
                    "task cancellation during taskStartingMutex lock wait");
        }

        if (pTask->isSlave())
        {
            Poco::Mutex::ScopedLock lock(taskMngtMutex);
            startingTask = NULL;

            throw TaskMergedException("Task merged. "
                    "Start skipped. ");
        }
    }
    processing = true;

    taskMngtMutex.lock();
    startingTask = NULL;
    taskMngtMutex.unlock();

    try
    {
        pTask->parentPrepareTask();
    }
    catch (ExecutionAbortedException&)
    {
        poco_warning(logger(), pTask->name() + " starting failed (prepareTask)"
        		" on user cancellation request");
        // release trigging port
        if (pTask->triggingPort())
        	safeReleaseInPort(pTask->triggingPort()->index());

        releaseProcessingMutex();
        throw;
    }
	catch (TaskMergedException&)
	{
        poco_warning(logger(), pTask->name() + " starting failed (prepareTask)"
        		" on task merged");
        releaseProcessingMutex();
        throw;
	}
	catch (...)
	{
        releaseProcessingMutex();

        if ((pTask->getState() == MergeableTask::TASK_FINISHED) && pTask->isSlave())
        {
            poco_information(logger(), pTask->name()
            		+ " starting failed (prepareTask):"
            		" was merged and is finished");
            throw TaskMergedException("Finished slave task. "
                    "Can not run");
        }
        else
        {
        	poco_error(logger(), pTask->name()
            		+ " starting failed (prepareTask) on unknown error...");
        }

        throw;
    }
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

		cancelEffective = false;
        resetDone = false;

        setRunningState(ModuleTask::applyingParameters);
		waitParameters();

		setRunningState(ModuleTask::retrievingInDataLocks);

		startCond = startCondition();
	}
    catch (...)
    {
        parametersTreated();
        try
        {
        	safeReleaseAllInPorts(triggingPort());
        }
        catch (Poco::Exception& e)
        {
        	poco_fatal(logger(),name()
        			+ ".run: safeReleaseAllInPorts: "
					+ e.displayText()
					+ " === = ==  = = = =  == = = == === = = = ===");
        	throw;
        }
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
		try
		{
			releaseAllInPorts();
			releaseAllOutPorts();
			releaseProcessingMutex();
			releaseOutputMutex();
		}
		catch (std::exception& e)
		{
			poco_fatal(logger(), name()
				+ ".run error: " + std::string(e.what())
				+ " == = = = = =  = = = = == ==  = = = = == =  == = =");
			throw;
		}
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

	if (immediateCancelling || cancelDone)
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

	if (immediateCancelling || cancelDone)
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
	if (immediateCancelling || cancelDone)
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

bool Module::taskIsStarting(bool orRunning)
{
//    Poco::Mutex::ScopedLock lock(taskMngtMutex); // Ok: recursive mutex

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
        	if (orRunning)
        		return true;
        	else
        		break;
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

bool Module::taskIsPending()
{
    Poco::Mutex::ScopedLock lock(taskMngtMutex);

    if (taskQueue.size())
        return true;

    for (std::set<ModuleTaskPtr>::iterator it = allLaunchedTasks.begin(),
            ite = allLaunchedTasks.end(); it != ite; it++)
    {
        switch ((*it)->getState())
        {
        case MergeableTask::TASK_STARTING:
        case MergeableTask::TASK_RUNNING:
            //poco_information(logger(), task->name()
            //      + ": " + (*it)->name()
            //      + " is already running for " + name());
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
	Poco::ScopedLockWithUnlock<Poco::Mutex> lock(taskMngtMutex);

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
        return;
    }

    if (taskIsStarting()) // taskMngtMutex is recursive. this call is ok.
    {
        poco_information(logger(),"pop (async): a task is already starting. ");
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
        allLaunchedTasks.erase(nextTask);
        startingTask = NULL;
        lock.unlock();

        // * do not re-throw. popTask is not due to launch a task.
        // * but try to launch the next task in queue, if any:
        popTask();
	}
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

	startSyncPending->set();

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
		startSyncPending->reset();
        allLaunchedTasks.erase(nextTask);
        startingTask = NULL;
        taskMngtMutex.unlock();

        // * do not re-throw. popTaskSync is not due to launch a task.
        // * but try to launch the next task in queue, if any:
        popTaskSync();
	}
}

bool Module::tryUnregisterTask(ModuleTask* pTask)
{
	if (taskMngtMutex.tryLock())
	{
		poco_information(logger(), "erasing " + pTask->name()
				+ " from allLaunchedTasks");
		allLaunchedTasks.erase(ModuleTaskPtr(pTask, true));
		taskMngtMutex.unlock();
		return true;
	}
	else 
		return false;
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
            catch (Poco::Exception& e)
            {
                poco_information(logger(), "potential slave task found: "
                        + startingTask->name() + ", merging with master: "
                        + (*runningTask)->name() + " failed: "
                        + e.displayText());

            	safeReleaseInPort(trigPort->index());

                poco_warning(logger(), (*runningTask)->name()
                		+ ": slave candidate probably cancelled. "
                				"Cancelling self. ");
                (*runningTask)->cancel();
                throw ExecutionAbortedException("slave candidate: "
                		+ e.displayText());
            }

            poco_information(logger(), "slave task found (starting): "
                    + startingTask->name() + ", merging with master: "
                    + (*runningTask)->name() + " OK");

            try
            {
            	trigPort->tryCatchSource();
            }
            catch (ExecutionAbortedException& e)
            {
            	poco_warning(logger(), (*runningTask)->name()
            			+ " merging " + startingTask->name()
						+ " => can not catch the source ("
						+ e.displayText() + ")");
            	safeReleaseInPort(trigPort->index());

            	e.rethrow();
            }

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
            catch (Poco::Exception& e)
            {
                poco_information(logger(), "potential slave task found in queue: "
                        + (*qIt)->name() + ", merging with master: "
                        + (*runningTask)->name() + " failed: "
                        + e.displayText());

                poco_warning(logger(), (*runningTask)->name()
                		+ ": slave candidate in queue, probably cancelled. "
                				"Cancelling self. ");
                (*runningTask)->cancel();
                throw ExecutionAbortedException("slave candidate: "
                		+ e.displayText());
            }

            poco_information(logger(), "slave task found (in queue): "
                    + (*qIt)->name() + ", merging with master: "
                    + (*runningTask)->name() + " OK");

            allLaunchedTasks.insert(*qIt);
            taskQueue.erase(qIt); // taskMngtMutex is locked. The order (with Task::merge) is not that important.

            try
            {
            	trigPort->tryCatchSource();
            }
            catch (ExecutionAbortedException& e)
            {
            	poco_warning(logger(), (*runningTask)->name()
            			+ " merging " + (*qIt)->name()
						+ " => can not catch the source ("
						+ e.displayText() + ")");
            	safeReleaseInPort(trigPort->index());

            	e.rethrow();
            }

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

bool Module::moduleReady()
{
    return !(immediateCancelling || lazyCancelling || cancelDone || reseting);
}

void Module::lazyCancel()
{
    if (!cancelMutex.tryLock())
    {
        if (immediateCancelling || lazyCancelling)
        {
            poco_information(logger(), name()
                    + ".lazyCancel: already cancelling (from another thread)... return");
            return;
        }
        else
            cancelMutex.lock();
    }
    else
    {
        if (immediateCancelling || lazyCancelling)
        {
            poco_information(logger(), name()
                    + ".lazyCancel: already cancelling (from the same thread)... return");
            cancelMutex.unlock();
            return;
        }
    }

    lazyCancelling = true;

    if (cancelDone)
    {
        poco_information(logger(), name() + " already cancelled (self)... return");
        lazyCancelling = false;
        cancelMutex.unlock();
        return;
    }

    if (reseting)
    {
        poco_information(logger(), name() + " already reseting... return");
        lazyCancelling = false;
        cancelMutex.unlock();
        return;
    }

    cancelEffective = false;
    resetDone = false;

    cancelSources();

    poco_information(logger(), name() + ".lazyCancel: "
            "cancellation request dispatched to the sources");

    cancellationListenerThread.start(cancellationListenerRunnable);

    while (!cancellationListenerThread.isRunning())
    {
        poco_information(logger(), name() + ".lazyCancel: "
                "waiting for the listener to be running");
        Poco::Thread::yield();
    }

    cancelMutex.unlock();

	poco_information(logger(), name() + ".lazyCancel: listener started for end-of-cancellation...");
}

bool Module::immediateCancel()
{
    poco_information(logger(), name() + ".immediateCancel request");

    if (!cancelMutex.tryLock())
    {
        if (immediateCancelling)
        {
            poco_information(logger(), name()
                    + " already immediate cancelling from another thread... "
                    "return");
            return false;
        }
        else
            cancelMutex.lock();
    }
    else
    {
        if (immediateCancelling)
        {
            poco_information(logger(), name()
                    + " already immediate cancelling from the same thread... "
                    "return");
            cancelMutex.unlock();
            return false;
        }
    }

    immediateCancelling = true;
    cancelRequested = true;

    if (cancelDone)
    {
        poco_information(logger(), name() + " already cancelled (self)... return");
        immediateCancelling = false;
        cancelRequested = false;
        cancelMutex.unlock();
        return false;
    }

    if (reseting)
    {
        poco_information(logger(), name() + " already reseting... return");
        immediateCancelling = false;
        cancelRequested = false;
        cancelMutex.unlock();
        return false;
    }

    cancelEffective = false;
    resetDone = false;

    if (lazyCancelling)
    {
        poco_information(logger(), name() + " already lazy cancelling... "
                "Trig immediate cancelling then. ");
    }
    else
    {
        cancelSources();
        poco_information(logger(), name() + ".immediateCancel: "
                "cancellation request dispatched to the sources");
    }

    taskMngtMutex.lock();

    // delete pending tasks
    while (taskQueue.size())
    {
        InPort* port = taskQueue.front()->mTriggingPort;

        if (port)
            port->releaseInputDataOnFailure();

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

        ModuleTaskPtr(*it)->cancel();
    }

    taskMngtMutex.unlock();

    cancelTargets();
    poco_information(logger(), name() + ".immediateCancel: "
            "cancellation request dispatched to the targets");

    try
    {
        cancel();
        cancelRequested = false;
    }
    catch (...)
    {
        cancelRequested = false;
        poco_bugcheck_msg("Module::cancel shall not throw exceptions");
    }

    if (!lazyCancelling)
    {
        cancellationListenerThread.start(cancellationListenerRunnable);

        while (!cancellationListenerThread.isRunning())
        {
            poco_information(logger(), name() + ".lazyCancel: "
                    "waiting for the listener to be running");
            Poco::Thread::yield();
        }

        poco_information(logger(), name()
                + ".immediateCancel: listener started for end-of-cancellation...");
    }
    else
    {
        poco_information(logger(), name()
                + ".immediateCancel: using lazyCancellation listener...");
    }

    cancelMutex.unlock();

    return true;
}

void Module::cancellationListen()
{
    poco_information(logger(), name() + " entering self cancel listener: "
            "wait for all tasks to terminate");

    int counter = 0;
    while (taskIsPending())
    {
        Poco::Thread::yield();
        if (counter++ % 50 == 0)
            poco_information(logger(), "waiting for " + name() + " tasks to finish");
    }

    poco_information(logger(), name() + " self cancel listener: "
            "tasks stopped. ");

    cancelMutex.lock();
    if (!immediateCancelling)
    {
        poco_information(logger(), name() + " lazyCancel listener: "
                "propagate to the targets... ");
        cancelTargets();
        poco_information(logger(), name() + " lazyCancel listener: "
                "cancellation request dispatched to the targets");
    }
    cancelMutex.unlock();

    while (cancelRequested) // not used if lazy cancelling
    {
        Poco::Thread::yield();
        if (counter++ % 500 == 0)
            poco_information(logger(), "waiting for " + name() + ".cancel to return");
    }

    cancelled();
    poco_information(logger(), name() + " self cancel listener: "
            "cancellation effective.");
}

void Module::cancelled()
{
    Poco::Mutex::ScopedLock lock(taskMngtMutex);
    if (taskQueue.size())
        poco_bugcheck_msg(
                (name() + ": try to set as cancelled "
                 "although tasks remain enqueued").c_str() );

    cancelMutex.lock();
    cancelDone = true;
    lazyCancelling = false;
    immediateCancelling = false;
    cancelMutex.unlock();
}

void Module::waitCancelled()
{
    if (reseting || resetDone)
    {
        poco_information(logger(), name() + ".waitCancelled: a reset just occured. ");
        return;
    }

    if (!cancelEffective && waiting->trySet())
    {
        poco_information(logger(), Poco::Thread::current()->getName() + "#" + name() + ".waitCancelled: wait for sources... ");
        waitCancelSources();

        poco_information(logger(), name() + ".waitCancelled: wait for self... ");

        cancelMutex.lock();
        while (lazyCancelling || immediateCancelling)
        {
            cancelMutex.unlock();
            Poco::Thread::yield();
            cancelMutex.lock();
        }
        cancelMutex.unlock();

        poco_information(logger(), name() + ".waitCancelled: wait for targets... ");
        waitCancelTargets();
        poco_information(logger(), name() + ".waitCancelled: done. ");

        cancelEffective = true;
        waiting->reset();
    }
    else
        poco_information(logger(), name() + ".waitCancelled skipped");

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
    cancelDone = false;

	if (resetDone)
	{
		poco_information(logger(), name() + " already reset... return");
		reseting = false;
		return;
	}

	if (!cancelEffective)
	{
		poco_warning(logger(), name() + ".moduleReset: "
				"cancellation not over... "
				"Please check that waitCancelled was called first. "
				"Or the previous reset is over since a while, and a "
				"new task overriden it. " );
		reseting = false;
		return;
	}

	cancelEffective = false;

	if (taskQueue.size())
		poco_bugcheck_msg(
				(name() + ": try to reset "
 				 "although tasks are enqueued").c_str() );

	resetTargets();
	try
	{
//		startingTask = NULL;
	    if (startingTask)
	    {
		    poco_error(logger(), name()
            + ": a task (" + startingTask->name() + ") is starting... "
            "It should not happen since cancel is done. "
			"== = = == = == == = == = = = = == == = = = == = ");
			poco_bugcheck_msg((name()
			+ ": a task is starting... "
			"It should not happen since cancel is done. ").c_str());
	    }
//	    processing = false;
		if (processing)
		{
		    poco_error(logger(), name()
            + ": a task is processing... "
            "It should not happen since cancel is done. "
			"== = = == = == == = == = = = = == == = = = == = ");
			poco_bugcheck_msg((name() 
			+ ": a task is processing... "
			"It should not happen since cancel is done. ").c_str());
		}
		reset();
	}
	catch (...)
	{
		poco_bugcheck_msg("Module::reset shall not throw exceptions");
	}
	resetSources();

	resetDone = true;
	reseting = false;
}

void Module::processingTerminated()
{
    outputMutex.lock();
    outputLocked = true;

    try
    {
        popTask();
    }
    catch (...) // cancelled, merged...
    {
        releaseProcessingMutex();
        throw;
    }

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
