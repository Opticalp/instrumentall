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

void Module::run()
{
	expireOutData();

	setRunningState(ModuleTask::retrievingInDataLocks);
	int startCond = startCondition();

	mergeTasks(portsWithNewData());

	setRunningState(ModuleTask::processing);
	process(startCond);
}

bool Module::sleep(long milliseconds)
{
	if (*runningTask)
		return (*runningTask)->sleep(milliseconds);

	Poco::Thread::sleep(milliseconds);
	return false;
}

bool Module::yield()
{
	if (*runningTask)
		return (*runningTask)->yield();

	Poco::Thread::yield();
	return false;
}

void Module::setProgress(float progress)
{
	if (*runningTask == NULL)
		return;

	(*runningTask)->setProgress(progress);
}

bool Module::isCancelled()
{
	if (*runningTask == NULL)
		return false;

	return (*runningTask)->isCancelled();
}

void Module::cancel()
{
	if (*runningTask)
		(*runningTask)->cancel();

	throw Poco::RuntimeException(name(), "cancel. not in a task");
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

bool Module::enqueueTask(ModuleTask* task)
{
	Poco::FastMutex::ScopedLock lock(taskMngtMutex);

	Poco::Util::Application::instance()
				             .getSubsystem<ThreadManager>()
				             .registerNewModuleTask(task);

	allTasks.insert(task);
	taskQueue.push_back(task);

	// check in allTasks if there is a task that is running
	for (std::set<ModuleTask*>::iterator it = allTasks.begin(),
			ite = allTasks.end(); it != ite; it++)
	{
		switch ((*it)->getState())
		{
		case MergeableTask::TASK_STARTING:
		case MergeableTask::TASK_RUNNING:
			poco_information(logger(), task->name()
					+ ": a task is already running for " + name());
			return false;
		case MergeableTask::TASK_FALSE_START:
		case MergeableTask::TASK_IDLE: // probably self
		case MergeableTask::TASK_CANCELLING:
		case MergeableTask::TASK_FINISHED:
			break;
		default:
			poco_bugcheck_msg("unknown task state");
		}
	}

	return true;
}

void Module::popTask()
{
	Poco::ScopedLockWithUnlock<Poco::FastMutex> lock(taskMngtMutex);

	if (taskQueue.empty())
		return;

	ModuleTask* nextTask = taskQueue.front();
	poco_information(logger(), "poping out the next task: " + nextTask->name());
	taskQueue.pop_front();

	lock.unlock();

	Poco::Util::Application::instance()
			             .getSubsystem<ThreadManager>()
			             .startModuleTask(nextTask);
}

void Module::popTaskSync()
{
	Poco::ScopedLockWithUnlock<Poco::FastMutex> lock(taskMngtMutex);

	if (taskQueue.empty())
		return;

	ModuleTask* nextTask = taskQueue.front();
	poco_information(logger(), "SYNC poping out the next task: " + nextTask->name());
	taskQueue.pop_front();

	lock.unlock();

	Poco::Util::Application::instance()
			             .getSubsystem<ThreadManager>()
			             .startSyncModuleTask(nextTask);
}

void Module::unregisterTask(ModuleTask* task)
{
	Poco::FastMutex::ScopedLock lock(taskMngtMutex);

	allTasks.erase(task);
}

void Module::mergeTasks(std::set<size_t> inPortIndexes)
{
	Poco::FastMutex::ScopedLock lock(taskMngtMutex);

	for (std::set<size_t>::iterator it = inPortIndexes.begin(),
			ite = inPortIndexes.end(); it != ite; it++)
	{
		InPort* trigPort = getInPort(*it);

		if (triggingPort() == trigPort)
			continue; // current task

		bool found = false;

		// seek
		for (std::list<ModuleTask*>::iterator qIt = taskQueue.begin(),
				qIte = taskQueue.end(); qIt != qIte; qIt++)
		{
			if ((*qIt)->triggingPort() == trigPort)
			{
				found = true;
				(*runningTask)->merge(*qIt);
				taskQueue.erase(qIt);
				break;
			}
		}

		if (!found)
			poco_warning(logger(),
					"Unable to merge the task for " + trigPort->name());
	}
}
