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
        taskManager(threadPool),
		cancellingAll(false)
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

//    pNf->release();
}

void ThreadManager::onFailed(TaskFailedNotification* pNf)
{
    Poco::Exception e(pNf->reason());

    poco_error(logger(), pNf->task()->name()
    		+ ": " + e.displayText());

    ModuleTask* modTask = dynamic_cast<ModuleTask*>(pNf->task());
    if (modTask)
    {
        poco_information(logger(),
        		modTask->name() + ": module failed. Cancellation request");

    	modTask->moduleCancel();
    }

    // TODO:
    // - dispatch to a NotificationQueue

//    pNf->release();
}

void ThreadManager::onFailedOnCancellation(TaskFailedNotification* pNf)
{
    Poco::Exception e(pNf->reason());

    poco_information(logger(), pNf->task()->name()
            + ": failed on cancellation request. " + e.displayText());

    // TODO:
    // - dispatch to a NotificationQueue

//    pNf->release();
}

void ThreadManager::onFinished(TaskFinishedNotification* pNf)
{
    poco_information(logger(), pNf->task()->name() + " has stopped");

    ModuleTask* modTask = dynamic_cast<ModuleTask*>(pNf->task());
    if (modTask)
    {
        ModuleTaskPtr pTask(modTask,true);
    	unregisterModuleTask(pTask);

    	poco_information(logger(),
    	        pTask->name() + " unregistered... ref count is "
    	        + Poco::NumberFormatter::format(pTask->referenceCount()));

    	taskListLock.readLock();

		poco_information(logger(), "pending module tasks list size is: "
				+ Poco::NumberFormatter::format(pendingModTasks.size()));

//		if (pendingModTasks.size() == 1)
//		{
//			Poco::AutoPtr<ModuleTask> tmpTsk(*pendingModTasks.begin());
//			poco_information(logger(),
//					"remaining task is: " + tmpTsk->name());
//		}

		taskListLock.unlock();

		poco_information(logger(), "TaskFinishednotification treated. ");
    }
    // else   datalogger task ==> 06.06.16 datalogger does not run in a task any more?

	// TODO:
	// - dispatch to a NotificationQueue

//    pNf->release();
}

void ThreadManager::onEnslaved(TaskEnslavedNotification* pNf)
{
    poco_information(logger(), pNf->task()->name()
    		+ " enslaved " + pNf->slave()->name() );

//    pNf->release();
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

void ThreadManager::startModuleTask(ModuleTaskPtr& pTask)
{
	poco_information(logger(), "starting " + pTask->name());

	try
	{
		if (cancellingAll)
		{
		    // FIXME: cancel or not?
//			pTask->cancel();

			// directly throw exception, in order to not be relying on
			// taskMan.start exception throw, based on task.setState while
			// cancelling the task
	        throw Poco::RuntimeException("Cancelling all. "
	                "Can not start " + pTask->name());
		}

		taskManager.start(pTask);
	}
	catch (Poco::NoThreadAvailableException& e)
	{
		// FIXME

		poco_error(logger(), pTask->name() + " cannot be started, "
				+ e.displayText());

		poco_bugcheck_msg("Poco::NoThreadAvailableException treatment not supported");
	}
	catch (...)
	{
		poco_information(logger(), pTask->name()
				+ " failed to start");

        if (pTask->triggingPort())
           pTask->triggingPort()->releaseInputDataOnFailure();

        unregisterModuleTask(pTask);
		throw;
	}
}

void ThreadManager::startSyncModuleTask(ModuleTaskPtr& pTask)
{
	poco_information(logger(), "SYNC starting " + pTask->name());

	try
	{
		if (cancellingAll)
		{
            // FIXME: cancel or not?
//          pTask->cancel();

		    throw Poco::RuntimeException("Cancelling all. "
		            "Can not sync start " + pTask->name());
		}

		taskManager.startSync(pTask);
	}
	catch (...)
	{
		poco_information(logger(), pTask->name()
				+ " failed to sync start");

		if (pTask->triggingPort())
			pTask->triggingPort()->releaseInputDataOnFailure();

		unregisterModuleTask(pTask);
		throw;
	}
}

size_t ThreadManager::count()
{
	Poco::ScopedReadRWLock lock(taskListLock);

    return pendingModTasks.size();
}

#define TIME_LAPSE_WAIT_ALL 5
#include "ModuleManager.h"

void ThreadManager::waitAll()
{
	bool stoppedOnCancel = false;

    // joinAll does not work here,
    // since it seems that it locks the recursive creation of new threads...
    // We use an event instead.
    while (count() || threadPool.used() || cancellingAll)
    {
        //Poco::TaskManager::TaskList list = taskManager.taskList();
        //std::string nameList("\n");
        //for (Poco::TaskManager::TaskList::iterator it = list.begin(),
        //        ite = list.end(); it != ite; it++)
        //    nameList += " - " + (*it)->name() + "\n";

        //nameList.erase(nameList.end()-1);

        //poco_information(logger(), "active threads are : " + nameList);

        Poco::Thread::sleep(TIME_LAPSE_WAIT_ALL);

        if (cancellingAll)
        	stoppedOnCancel = true;
    }

    if (stoppedOnCancel)
    {
        ModuleManager& modMan = Poco::Util::Application::instance().getSubsystem<ModuleManager>();

        while (!modMan.allModuleReady())
            Poco::Thread::sleep(TIME_LAPSE_WAIT_ALL);

    	throw Poco::RuntimeException("waitAll","Cancellation upon user request");
    }

    poco_information(logger(), "All threads have stopped. ");
}

void ThreadManager::registerNewModuleTask(ModuleTaskPtr& pTask)
{
	if (cancellingAll)
		throw Poco::RuntimeException("Cancelling, "
				"can not register the new task: "
				+ pTask->name());

	Poco::ScopedWriteRWLock lock(taskListLock);

	pendingModTasks.insert(pTask);
}

void ThreadManager::unregisterModuleTask(ModuleTaskPtr& pTask)
{
    pTask->taskFinished();

	Poco::ScopedWriteRWLock lock(taskListLock);

	if (pendingModTasks.erase(pTask) < 1)
		poco_warning(logger(), "Failed to erase the task " + pTask->name()
				+ " from the thread manager");

	poco_information(logger(), pTask->name() + " erased from the thread manager. "
	        "ref cnt is now: "
	        + Poco::NumberFormatter::format(pTask->referenceCount()));
}

void ThreadManager::cancelAll()
{
    if (cancellingAll)
        return;

	cancellingAll = true;

	taskListLock.readLock();
	std::set<ModuleTaskPtr> tempModTasks = pendingModTasks;
	taskListLock.unlock();

	poco_notice(logger(), "CancelAll: Dispatching cancel() to all active tasks");

	for (std::set<ModuleTaskPtr>::iterator it = tempModTasks.begin(),
			ite = tempModTasks.end(); it != ite; it++)
	{
		ModuleTaskPtr tsk(*it);
		poco_information(logger(), "CancelAll: cancelling " + tsk->name());
		tsk->cancel();
	}

	poco_information(logger(), "CancelAll: All active tasks cancelled. Wait for them to delete. ");

	while (count() || threadPool.used())
		Poco::Thread::sleep(TIME_LAPSE_WAIT_ALL);

	poco_information(logger(), "No more pending task. Wait for all modules being ready... ");

	ModuleManager& modMan = Poco::Util::Application::instance().getSubsystem<ModuleManager>();

	while (!modMan.allModuleReady())
        Poco::Thread::sleep(TIME_LAPSE_WAIT_ALL);

    poco_information(logger(), "All modules ready. CancelAll done. ");

	cancellingAll = false;
}

void ThreadManager::startModuleCancellationListener(Poco::Runnable& runnable)
{
    threadPool.start(runnable);
}
