/**
 * @file	src/ThreadManager.cpp
 * @date	Mar 2016
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

#include "ThreadManager.h"

#include "DataLogger.h"
#include "ModuleTask.h"
#include "Module.h"

#include "Poco/Observer.h"
#include "Poco/NumberFormatter.h"
#include "Poco/ThreadPool.h"
#include "Poco/AutoPtr.h"

using Poco::Observer;

ThreadManager::ThreadManager():
        VerboseEntity(name()),
        threadPool(2,32), // TODO set maxCapacity from config file
        taskManager(threadPool)
{
    taskManager.addObserver(
            Observer<ThreadManager, TaskStartedNotification>(
                    *this, &ThreadManager::onStarted ) );
    taskManager.addObserver(
            Observer<ThreadManager, TaskFailedNotification>(
                    *this, &ThreadManager::onFailed ) );
    taskManager.addObserver(
                Observer<ThreadManager, TaskFinishedNotification>(
                        *this, &ThreadManager::onFinished ) );
    taskManager.addObserver(
                Observer<ThreadManager, TaskEnslavedNotification>(
                        *this, &ThreadManager::onEnslaved ) );

    threadPool.addCapacity(32);
}

void ThreadManager::onStarted(TaskStartedNotification* pNf)
{
    poco_information(logger(), pNf->task()->name() + " was started");

    // TODO:
    // - dispatch to a NotificationQueue
}

void ThreadManager::onFailed(TaskFailedNotification* pNf)
{
    Poco::Exception e(pNf->reason());
    poco_error(logger(), e.displayText());

    // TODO:
    // - dispatch to a NotificationQueue
}

void ThreadManager::onFinished(TaskFinishedNotification* pNf)
{
    poco_information(logger(), pNf->task()->name() + " has stopped");

    ModuleTask* modTask = dynamic_cast<ModuleTask*>(pNf->task());

    if (modTask)
    {
    	// TODO:
		// - dispatch to a NotificationQueue

    	unregisterModuleTask(modTask);

    	taskListLock.readLock();

		poco_information(logger(), "pending module tasks list size is: "
				+ Poco::NumberFormatter::format(pendingModTasks.size()));

		taskListLock.unlock();

		if (!modTask->isSlave())
		{
			poco_information(logger(), modTask->name() + " is not a slave");
			modTask->module()->popTaskSync();
		}

//		poco_information(logger(), "TaskFinishednotification dispatched. ");
    }
    // else   datalogger task ==> 06.06.16 datalogger does not run in a task any more?
}

void ThreadManager::onEnslaved(TaskEnslavedNotification* pNf)
{
    poco_information(logger(), pNf->task()->name()
    		+ " enslaved " + pNf->slave()->name() );
}

void ThreadManager::startDataLogger(DataLogger* dataLogger)
{
	try
	{
		threadPool.start(*dataLogger);
	}
	catch (Poco::NoThreadAvailableException& e)
	{
		// FIXME

		poco_error(logger(), dataLogger->name() + " cannot be started, "
				+ e.displayText());
	}
}

void ThreadManager::startModuleTask(ModuleTask* pTask)
{
	Poco::AutoPtr<ModuleTask> taskPtr(pTask, true); // do not take ownership!

	// poco_information(logger(), "starting " + taskPtr->name());

	try
	{
		taskManager.start(taskPtr);
	}
	catch (Poco::NoThreadAvailableException& e)
	{
		// FIXME

		poco_error(logger(), pTask->name() + " cannot be started, "
				+ e.displayText());
	}
	catch (...)
	{
		unregisterModuleTask(pTask);
		throw;
	}
}

void ThreadManager::startSyncModuleTask(ModuleTask* pTask)
{
	Poco::AutoPtr<MergeableTask> taskPtr(pTask, true); // do not take ownership!

	try
	{
		taskManager.startSync(taskPtr);
	}
	catch (...)
	{
		unregisterModuleTask(pTask);
		throw;
	}
}

int ThreadManager::count()
{
	Poco::ScopedReadRWLock lock(taskListLock);

    return pendingModTasks.size();
}

#define TIME_LAPSE_WAIT_ALL 5

void ThreadManager::waitAll()
{
    // joinAll does not work here,
    // since it seems that it locks the recursive creation of new threads...
    // We use an event instead.
    while (count())
    {
        //Poco::TaskManager::TaskList list = taskManager.taskList();
        //std::string nameList("\n");
        //for (Poco::TaskManager::TaskList::iterator it = list.begin(),
        //        ite = list.end(); it != ite; it++)
        //    nameList += " - " + (*it)->name() + "\n";

        //nameList.erase(nameList.end()-1);

        //poco_information(logger(), "active threads are : " + nameList);

        Poco::Thread::sleep(TIME_LAPSE_WAIT_ALL);
    }

    poco_information(logger(), "All threads have stopped. ");
}

void ThreadManager::registerNewModuleTask(ModuleTask* pTask)
{
	Poco::ScopedWriteRWLock lock(taskListLock);

	Poco::AutoPtr<ModuleTask> taskPtr(pTask);
	pendingModTasks.insert(taskPtr);
}

void ThreadManager::unregisterModuleTask(ModuleTask* pTask)
{
//	poco_information(logger(), "unregistering " + pTask->name());
	Poco::ScopedWriteRWLock lock(taskListLock);

	Poco::AutoPtr<ModuleTask> taskPtr(pTask, true); // do not take ownership!
	if (pendingModTasks.erase(taskPtr) < 1)
		poco_warning(logger(), "Failed to erase the task " + pTask->name()
				+ " from the thread manager");
}
