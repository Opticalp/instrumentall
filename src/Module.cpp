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

void Module::run(ModuleTask* pTask)
{
	setRunningTask(pTask);

	taskMngtMutex.lock();
	if (startSyncPending)
	{
		startSyncPending = false;
		taskMngtMutex.unlock();
	}
	taskMngtMutex.unlock();

	try
	{
		setRunningState(ModuleTask::applyingParameters);
		waitParameters();

		setRunningState(ModuleTask::retrievingInDataLocks);

		// TODO: remove cancelDone? replaced by the cancelDoneEvent that is reset during Module::moduleReset?
		resetDone = false;
		cancelDone = false;

		int startCond = startCondition();

		// start condition has to be checked before any cancellation test
		// to allow releaseAllInPorts to be effective

		mergeTasks(inPortCoughts());

		if (isCancelled())
			throw Poco::RuntimeException(name() +
					": can not run a new task, "
					"the module is cancelling");

		setRunningState(ModuleTask::processing);
		process(startCond);
	}
	catch (...)
	{
		parametersTreated();
		safeReleaseAllInPorts(triggingPort());
		releaseAllOutPorts();
		throw;
	}

	parametersTreated();
	releaseAllInPorts();
	releaseAllOutPorts();
}

bool Module::sleep(long milliseconds)
{
	bool ret = false;

	if (*runningTask)
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

	if (*runningTask)
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
	if (*runningTask == NULL)
		return;

	(*runningTask)->setProgress(progress);
}

bool Module::isCancelled()
{
	if (immediateCancelling || cancelDone)
		return true;

	if (*runningTask == NULL)
		return false;

	return (*runningTask)->isCancelled();
}

InPort* Module::triggingPort()
{
	if (*runningTask)
		return (*runningTask)->triggingPort();

	throw Poco::RuntimeException(name(), "querying trigging port. not in a task");
}

void Module::setRunningState(ModuleTask::RunningStates state)
{
	if (*runningTask == NULL)
		return;

	(*runningTask)->setRunningState(state);
}

ModuleTask::RunningStates Module::getRunningState()
{
	if (*runningTask == NULL)
		return ModuleTask::processing;

	return (*runningTask)->getRunningState();
}

void Module::enqueueTask(ModuleTask* task)
{
	// take ownership of the task.
	Poco::Util::Application::instance()
				             .getSubsystem<ThreadManager>()
				             .registerNewModuleTask(task);

	if (cancelling || isCancelled() || reseting)
	{
		if (task->triggingPort())
			task->triggingPort()->releaseInputData();
		throw Poco::InvalidAccessException(name(),
				"enqueue task: module is cancelling");
	}

	Poco::Mutex::ScopedLock lock(taskMngtMutex);

	taskQueue.push_back(task);

	if (taskQueue.size()>1)
	{
		poco_information(logger(), task->name()
					+ ": tasks are already enqueued for " + name());
		return;
	}

	if (!taskIsRunning())
		popTask();
	else
		poco_information(logger(), name() + ": a task is already running");
}

bool Module::taskIsRunning()
{
	Poco::Mutex::ScopedLock lock(taskMngtMutex); // Ok: recursive mutex

	for (std::set<ModuleTask*>::iterator it = allLaunchedTasks.begin(),
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
	Poco::Mutex::ScopedLock lock(taskMngtMutex);

	if (taskQueue.empty())
		return;

	ModuleTask* nextTask = taskQueue.front();
	poco_information(logger(), "poping out the next task: " + nextTask->name());
	taskQueue.pop_front();

	allLaunchedTasks.insert(nextTask);
	Poco::Util::Application::instance()
					 .getSubsystem<ThreadManager>()
					 .startModuleTask(nextTask);
}

void Module::popTaskSync()
{
	// n.b.: the taskMngtMutex is locked during immediateCancel.
	// when released, the taskQueue is empty.
	taskMngtMutex.lock();

	if (taskQueue.empty())
	{
		poco_information(logger(), 
			(*runningTask)->name() + ": empty task queue, nothing to sync pop");
		taskMngtMutex.unlock();
		return;
	}

	if (taskIsRunning())
	{
		poco_information(logger(),"sync pop: a task is already running. ");
		taskMngtMutex.unlock();
		return;
	}

	Poco::AutoPtr<ModuleTask> nextTask(taskQueue.front(), true);
	poco_information(logger(), "SYNC poping out the next task: " + nextTask->name());
	taskQueue.pop_front();

	startSyncPending = true;

	try
	{
		allLaunchedTasks.insert(nextTask);
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

void Module::unregisterTask(ModuleTask* task)
{
	Poco::Mutex::ScopedLock lock(taskMngtMutex);
	allLaunchedTasks.erase(task);
}

void Module::mergeTasks(std::set<size_t> inPortIndexes)
{
	// Poco::FastMutex::ScopedLock lock(taskMngtMutex);
	taskMngtMutex.lock();

	for (std::set<size_t>::iterator it = inPortIndexes.begin(),
			ite = inPortIndexes.end(); it != ite; )
	{
		bool found = false;
		InPort* trigPort;

		try
		{
			trigPort = getInPort(*it);

			if (triggingPort() == trigPort)
			{
				it++;
				continue; // current task
			}

			// seek
			for (std::list<ModuleTask*>::iterator qIt = taskQueue.begin(),
					qIte = taskQueue.end(); qIt != qIte; qIt++)
			{
				poco_information(logger(), "taskQueue includes: " + (*qIt)->name());
				if ((*qIt)->triggingPort() == trigPort)
				{
					found = true;
					(*runningTask)->merge(*qIt);
					taskQueue.erase(qIt);
					poco_information(logger(), "slave task found, merging OK");
					break;
				}
			}
		}
		catch (...)
		{
			taskMngtMutex.unlock();
			throw;
		}

		if (!found)
		{
			taskMngtMutex.unlock();
			poco_warning(logger(),
					"Unable to merge the task for " + trigPort->name()
					+ ". Retry.");

			if (yield() || cancelling)
				throw Poco::RuntimeException(name(), "Cancellation upon user request");
			taskMngtMutex.lock();
		}
		else
			it++;
	}

	poco_information(logger(),
		"remaining tasks: " + Poco::NumberFormatter::format(taskQueue.size()));

	taskMngtMutex.unlock();
}

Poco::AutoPtr<ModuleTask> Module::runModule()
{
    Poco::AutoPtr<ModuleTask> taskPtr(new ModuleTask(this), true);
    enqueueTask(taskPtr);

    return taskPtr;
}

void Module::waitParameters()
{
	while (!tryAllParametersSet())
	{
		if (yield() || cancelling)
			throw Poco::RuntimeException(name(),
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
	cancellingInPort.clear();
	cancelling = false;
	immediateCancelling = false;
}

void Module::immediateCancel()
{
	poco_information(logger(), name() + " immediate cancel request");

	if (immediateCancelling)
	{
		poco_information(logger(), "already immediate cancelling... return");
		return;
	}

	immediateCancelling = true;

	if (cancelDoneEvent.tryWait(0))
	{
		poco_information(logger(), "already cancelled... return");
		immediateCancelling = false;
		return;
	}


	if (cancelling)
		poco_information(logger(), "already cancelling... "
				"Trig immediate cancelling then. ");
	else
		cancelling = true;

	cancelSources();
	cancelTargets();

	taskMngtMutex.lock();

	// delete pending tasks
	while (taskQueue.size())
	{
		InPort* port = taskQueue.front()->mTriggingPort;
		if (port)
			port->releaseInputDataOnStartFailure();
		taskQueue.pop_front();
	}

	// stop active tasks
	for (std::set<ModuleTask*>::iterator it = allLaunchedTasks.begin(),
			ite = allLaunchedTasks.end(); it != ite; it++)
	{
		switch ((*it)->getState())
		{
		case MergeableTask::TASK_IDLE:
		case MergeableTask::TASK_STARTING:
		case MergeableTask::TASK_RUNNING:
			(*it)->cancel();
			break;
		case MergeableTask::TASK_FALSE_START:
		case MergeableTask::TASK_FINISHED:
		case MergeableTask::TASK_CANCELLING:
		default:
			break;
		}
	}

	taskMngtMutex.unlock();

	try
	{
		cancel(); // shall trig Module::cancelled
	}
	catch (...)
	{
		poco_bugcheck_msg("Module::cancel shall not throw exceptions");
	}

	poco_information(logger(), "immediateCancel: cancellation request dispatched...");
}

void Module::lazyCancel(InPort* canceller)
{
    cancellingInPort.insert(canceller);

	if (immediateCancelling || cancelling)
	{
		poco_information(logger(), "lazyCancel: already cancelling... return");
		return;
	}

	cancelling = true;

	// check if the module was recently cancelled
	if (cancelDoneEvent.tryWait(0))
	{
		poco_information(logger(), "lazyCancel: already cancelled... return");
		cancellingInPort.erase(canceller);
		poco_assert(cancellingInPort.empty());
		cancelling = false;
		return;
	}

	cancelSources();

	// TODO: async ?

	// TODO
	// wait for the current run to be terminated (no more tasks?)
	// care should be taken if input can fail (see startCondition)

	cancelled();

	cancelTargets();

	poco_information(logger(), "lazyCancel: cancellation request dispatched...");
}

void Module::moduleReset()
{
	if (reseting)
	{
		poco_information(logger(), "already reseting... return");
		return;
	}

	reseting = true;

	if (resetDone)
	{
		poco_information(logger(), "already reset... return");
		reseting = false;
		return;
	}

	if (!cancelDoneEvent.tryWait(0))
		poco_bugcheck_msg( (name() + ".moduleReset: "
				"cancellation not over... "
				"Please check that waitCancelled was called first").c_str() );

	if (taskQueue.size())
		poco_bugcheck_msg(
				(name() + ": try to reset "
 				 "although tasks remain enqueued").c_str() );

	cancelDoneEvent.reset();

	resetTargets();
	try
	{
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

bool Module::isCancelling(InPort* canceller)
{
    if (immediateCancelling)
        return true;

    return (cancellingInPort.count(canceller) > 0);
}
