/**
 * @file	src/core/ThreadManager.cpp
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
#include "Poco/NObserver.h"
#include "Poco/NumberFormatter.h"
#include "Poco/ThreadPool.h"
#include "Poco/AutoPtr.h"

#define CONF_KEY_WATCHDOG_TIMEOUT "watchdog.timeout"
#define TIMEOUT_DEFAULT 5000

using Poco::NObserver;

ThreadManager::ThreadManager():
        VerboseEntity(name()),
        threadPool(2,32), // TODO set maxCapacity from config file
        taskManager(threadPool), lastThreadCount(0),
		cancellingAll(false),
		cancelEvent(true), // autoreset
		moduleFailure(false),
		watchDog(this, TIMEOUT_DEFAULT)
{
    taskManager.addObserver(
            NObserver<ThreadManager, TaskStartedNotification>(
                    *this, &ThreadManager::onStarted ) );
    taskManager.addObserver(
            NObserver<ThreadManager, TaskFailedNotification>(
                    *this, &ThreadManager::onFailed ) );
    taskManager.addObserver(
            NObserver<ThreadManager, TaskFailedOnCancellationNotification>(
                    *this, &ThreadManager::onFailedOnCancellation ) );
    taskManager.addObserver(
                NObserver<ThreadManager, TaskFinishedNotification>(
                        *this, &ThreadManager::onFinished ) );
    taskManager.addObserver(
                NObserver<ThreadManager, TaskEnslavedNotification>(
                        *this, &ThreadManager::onEnslaved ) );

    threadPool.addCapacity(32);
}

ThreadManager::~ThreadManager()
{
    if (watchDog.isActive())
        poco_warning(logger(), "Watchdog active at ThreadManager deletion!");

    if (taskManager.taskList().size())
        poco_warning(logger(), "Task list not empty at ThreadManager deletion!");

    threadPool.collect();
    if (threadPool.used())
        poco_warning(logger(), "Thread pool busy at ThreadManager deletion!");

    if (pendingModTasks.size())
        poco_warning(logger(), "Pending tasks remain at ThreadManager deletion!");
}

#ifdef POCO_VERSION_H
#include "Poco/Version.h"
#endif

void ThreadManager::initialize(Poco::Util::Application& app)
{
    setLogger(name());

    if (app.config().hasProperty(CONF_KEY_WATCHDOG_TIMEOUT))
    {
        try
        {
#if ( POCO_VERSION >= 0x01050000 )
            watchDog.setTimeout(app.config().getInt64(CONF_KEY_WATCHDOG_TIMEOUT));
#else
            watchDog.setTimeout(app.config().getInt(CONF_KEY_WATCHDOG_TIMEOUT));
#endif
        }
        catch (Poco::SyntaxException&)
        {
            poco_information(logger(), "Using watchdog timeout hardcoded value");
        }

        poco_information(logger(), "starting watchdog...");
        startWatchDog();
    }
}

void ThreadManager::uninitialize()
{
    poco_information(logger(), "ThreadManager::uninitializing...");
    stopWatchDog();
    poco_information(logger(), "ThreadManager::uninitialized.");
}

void ThreadManager::onStarted(const AutoPtr<TaskStartedNotification>& pNf)
{
    poco_information(logger(), pNf->task()->name() + " was started");

    // TODO:
    // - dispatch to a NotificationQueue
}

void ThreadManager::onFailed(const AutoPtr<TaskFailedNotification>& pNf)
{
    Poco::Exception e(pNf->reason());

    poco_error(logger(), pNf->task()->name()
    		+ ": " + e.displayText());

    ModuleTask* modTask = dynamic_cast<ModuleTask*>(pNf->task());
    if (modTask)
    {
        poco_information(logger(),
        		modTask->name() + ": module failed. Cancellation request");

        if (!moduleFailure)
        {
            moduleFailure = true;
            cancelEvent.set();
            modTask->moduleCancel();
            moduleFailure = false;
        }
        else
            modTask->moduleCancel();
    }

    // TODO:
    // - dispatch to a NotificationQueue
}

void ThreadManager::onFailedOnCancellation(const AutoPtr<TaskFailedOnCancellationNotification>& pNf)
{
    Poco::Exception e(pNf->reason());

    poco_information(logger(), pNf->task()->name()
            + ": failed on cancellation request. " + e.displayText());

    // TODO:
    // - dispatch to a NotificationQueue
}

void ThreadManager::onFinished(const AutoPtr<TaskFinishedNotification>& pNf)
{
    poco_information(logger(), pNf->task()->name() + " has stopped");

    ModuleTask* modTask = dynamic_cast<ModuleTask*>(pNf->task());
    if (modTask)
    {
        ModuleTaskPtr pTask(modTask,true);
    	unregisterModuleTask(pTask);

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

	// TODO:
	// - dispatch to a NotificationQueue
}

void ThreadManager::onEnslaved(const AutoPtr<TaskEnslavedNotification>& pNf)
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
		// FIXME: threadPool.start > NoThreadAvailableException

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
        // FIXME: taskManager.start > NoThreadAvailableException

		poco_error(logger(), pTask->name() + " cannot be started, "
				+ e.displayText());

		poco_bugcheck_msg("Poco::NoThreadAvailableException treatment not supported");
	}
	catch (ExecutionAbortedException&)
	{
		poco_information(logger(), pTask->name()
				+ " failed to start: cancelled before starting");

		if (pTask->triggingPort())
           pTask->triggingPort()->releaseInputDataOnFailure();

        unregisterModuleTask(pTask);

        throw;
	}
	catch (TaskMergedException&)
	{
		poco_information(logger(), pTask->name()
				+ " failed to start: merged before starting");
		throw;
	}
	catch (...)
	{
		poco_error(logger(), pTask->name()
				+ " failed to start on unknown exception");

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

#define TIME_LAPSE_WAIT_ALL 50
#include "ModuleManager.h"

void ThreadManager::waitAll()
{
	bool stoppedOnCancel = false;
	bool stoppedOnFailure = false;
	cancelEvent.reset();

    // joinAll does not work here,
    // since it seems that it locks the recursive creation of new threads...
    // We use an event instead.
    while (count() || (threadPool.used() - watchDog.isActive()) || cancellingAll)
    {
        //Poco::TaskManager::TaskList list = taskManager.taskList();
        //std::string nameList("\n");
        //for (Poco::TaskManager::TaskList::iterator it = list.begin(),
        //        ite = list.end(); it != ite; it++)
        //    nameList += " - " + (*it)->name() + "\n";

        //nameList.erase(nameList.end()-1);

        //poco_information(logger(), "active threads are : " + nameList);

        if (stoppedOnCancel)
            Poco::Thread::sleep(TIME_LAPSE_WAIT_ALL);
        else if (cancelEvent.tryWait(TIME_LAPSE_WAIT_ALL))
        {
            if (moduleFailure)
                stoppedOnFailure = true;
            else // if (cancellingAll)
                stoppedOnCancel = true;
        }
    }

    if (stoppedOnCancel || stoppedOnFailure)
    {
        ModuleManager& modMan = Poco::Util::Application::instance().getSubsystem<ModuleManager>();

        poco_notice(logger(),"waitAll: execution stopped on cancellation or failure. "
                "Waiting till all the module are ready. ");

        while (!modMan.allModuleReady())
            Poco::Thread::sleep(TIME_LAPSE_WAIT_ALL);

        if (stoppedOnFailure)
            throw Poco::RuntimeException("waitAll","Stopped on module failure");
        else // stoppedOnCancel
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

	poco_information(logger(), pTask->name() + " erased from ThreadManager::pendingModTasks. ");
}

void ThreadManager::cancelAllNoWait()
{
	cancelEvent.set();

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
}

void ThreadManager::cancelAll()
{
    if (cancellingAll)
        return;

	cancellingAll = true;

	cancelAllNoWait();

	poco_information(logger(), "CancelAll: All active tasks cancelled. Wait for them to delete. ");

	while (count() || (threadPool.used() - watchDog.isActive()))
		Poco::Thread::sleep(TIME_LAPSE_WAIT_ALL);

	poco_information(logger(), "No more pending task. Wait for all modules being ready... ");

	ModuleManager& modMan = Poco::Util::Application::instance().getSubsystem<ModuleManager>();

	while (!modMan.allModuleReady())
        Poco::Thread::sleep(TIME_LAPSE_WAIT_ALL);

    poco_information(logger(), "All modules ready. CancelAll done. ");

	cancellingAll = false;
}

void ThreadManager::cancelAllFromWatchDog()
{
	poco_warning(logger(), "WatchDog signaled! cancelling all. ");
	cancelAll();
}

void ThreadManager::startRunnable(Poco::Runnable& runnable)
{
    threadPool.start(runnable);
}

void ThreadManager::startWatchDog()
{
//    if (!watchDog.isActive())
        startRunnable(watchDog);
}


using Poco::Util::Option;
using Poco::Util::OptionSet;

void ThreadManager::defineOptions(Poco::Util::OptionSet& options)
{
    options.addOption(
        Option(
                "watchdog", "w",
                "specify the watchdog timeout: TIMEOUT, "
                "time after which the application is considered as frozen "
                "if the tasks have not evolved. " )
            .required(false)
            .repeatable(false)
            .argument("TIMEOUT")
            .binding(CONF_KEY_WATCHDOG_TIMEOUT));
}

bool ThreadManager::taskListFrozen(bool init)
{
    TaskManager::TaskList newList = taskManager.taskList();

    bool frozen = true;

    if (init)
    {
        frozen = false;
    }
    else if (lastTaskList.size() != newList.size())
    {
        frozen = false;
    }
    else if (newList.size()==0)
    {
        lastTaskList.clear();
        return false;
    }
    else
    {
        for (TaskManager::TaskList::iterator it = newList.begin(),
                ite = newList.end(); it != ite; it++)
        {
            if (lastTaskList.count(*it) == 0)
            {
                frozen = false;
                break;
            }
        }
    }

    lastTaskList.clear();
    for (TaskManager::TaskList::iterator it = newList.begin(),
            ite = newList.end(); it != ite; it++)
        lastTaskList.insert(*it);

    return frozen;
}

bool ThreadManager::threadCountNotChanged(bool init)
{
    size_t newThreadCount = threadPool.used();

    if (init)
    {
        lastThreadCount = newThreadCount;
        return false;
    }

    bool frozen = false;
    if ((newThreadCount != 1) && (newThreadCount == lastThreadCount))
        frozen = true;

    lastThreadCount = newThreadCount;
    return frozen;
}
