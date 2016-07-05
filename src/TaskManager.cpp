/**
 * @file	src/TaskManager.cpp
 * @date	June 2016
 * @author	Applied Informatics Software Engineering GmbH and contributors
 *
 * Original Poco::TaskManager code checked out from https://github.com/pocoproject/poco
 * commit d6ebf591 (May 2016)
 */

//
// TaskManager.cpp
//
// $Id: //poco/1.4/Foundation/src/TaskManager.cpp#1 $
//
// Library: Foundation
// Package: Tasks
// Module:  Tasks
//
// Copyright (c) 2004-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//


#include "TaskManager.h"
#include "TaskNotification.h"


const int TaskManager::MIN_PROGRESS_NOTIFICATION_INTERVAL = 100000; // 100 milliseconds


TaskManager::TaskManager():
	threadPool(Poco::ThreadPool::defaultPool())
{
}


TaskManager::TaskManager(Poco::ThreadPool& pool):
	threadPool(pool)
{
}


TaskManager::~TaskManager()
{
}


void TaskManager::start(TaskPtr pAutoTask)
{
	Poco::FastMutex::ScopedLock lock(mutex);

	pAutoTask->setOwner(this);
	pAutoTask->setState(MergeableTask::TASK_STARTING);
	mTaskList.push_back(pAutoTask);
	try
	{
		threadPool.start(*pAutoTask, pAutoTask->name());
	}
	catch (...)
	{
		// Make sure that we don't act like we own the task since
		// we never started it.  If we leave the task on our task
		// list, the size of the list is incorrect.
		mTaskList.pop_back();
		pAutoTask->setState(MergeableTask::TASK_FALSE_START);
		throw;
	}
}


void TaskManager::startSync(TaskPtr pAutoTask)
{
	mutex.lock();

	try
	{
		pAutoTask->setOwner(this);
		pAutoTask->setState(MergeableTask::TASK_STARTING);
	}
	catch (...)
	{
		mutex.unlock();
		throw;
	}

	mTaskList.push_back(pAutoTask);
	mutex.unlock();

	try
	{
		pAutoTask->run();
	}
	catch (...)
	{
		// Make sure that we don't act like we own the task since
		// we never started it.  If we leave the task on our task
		// list, the size of the list is incorrect.
		pAutoTask->setState(MergeableTask::TASK_FALSE_START);
		mutex.lock();
		mTaskList.pop_back();
		mutex.unlock();
		throw;
	}
}


void TaskManager::cancelAll()
{
	Poco::FastMutex::ScopedLock lock(mutex);

	for (TaskList::iterator it = mTaskList.begin(); it != mTaskList.end(); ++it)
	{
		(*it)->cancel();
	}
}


TaskManager::TaskList TaskManager::taskList() const
{
	Poco::FastMutex::ScopedLock lock(mutex);
	
	return mTaskList;
}


void TaskManager::addObserver(const Poco::AbstractObserver& observer)
{
	nc.addObserver(observer);
}


void TaskManager::removeObserver(const Poco::AbstractObserver& observer)
{
	nc.removeObserver(observer);
}


//void TaskManager::postNotification(const Poco::Notification::Ptr& pNf)
//{
//	_nc.postNotification(pNf);
//}


void TaskManager::taskStarted(MergeableTask* pTask)
{
	nc.postNotification(new TaskStartedNotification(pTask));
}


void TaskManager::taskProgress(MergeableTask* pTask, float progress)
{
	Poco::ScopedLockWithUnlock<Poco::FastMutex> lock(mutex);

	if (lastProgressNotification.isElapsed(MIN_PROGRESS_NOTIFICATION_INTERVAL))
	{
		lastProgressNotification.update();
		lock.unlock();
		nc.postNotification(new TaskProgressNotification(pTask, progress));
	}
}


void TaskManager::taskCancelled(MergeableTask* pTask)
{
	nc.postNotification(new TaskCancelledNotification(pTask));
}


void TaskManager::taskFinished(MergeableTask* pTask)
{
	TaskPtr currentTask;
	Poco::ScopedLockWithUnlock<Poco::FastMutex> lock(mutex);
	
	for (TaskList::iterator it = mTaskList.begin(); it != mTaskList.end(); ++it)
	{
		if (*it == pTask)
		{
			currentTask = *it;
			mTaskList.erase(it);
			break;
		}
	}
	lock.unlock();

	nc.postNotification(new TaskFinishedNotification(pTask));
}


void TaskManager::taskFailed(MergeableTask* pTask, const Poco::Exception& exc)
{
	nc.postNotification(new TaskFailedNotification(pTask, exc));
}

void TaskManager::taskEnslaved(MergeableTask* pTask, MergeableTask* enslaved)
{
	enslaved->setOwner(this);
	nc.postNotification(new TaskEnslavedNotification(pTask, enslaved));
}
