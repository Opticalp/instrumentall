/**
 * @file	src/TaskManager.h
 * @date	June 2016
 * @author	Applied Informatics Software Engineering GmbH and contributors
 *
 * Original Poco::TaskManager code checked out from https://github.com/pocoproject/poco
 * commit d6ebf591 (May 2016)
 */

//
// TaskManager.h
//
// $Id: //poco/1.4/Foundation/include/Poco/TaskManager.h#2 $
//
// Library: Foundation
// Package: Tasks
// Module:  Tasks
//
// Definition of the TaskManager class.
//
// Copyright (c) 2004-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//


#ifndef CUSTOM_Foundation_TaskManager_INCLUDED
#define CUSTOM_Foundation_TaskManager_INCLUDED

#include "MergeableTask.h"
#include "TaskNotification.h"

#include "Poco/Mutex.h"
#include "Poco/AutoPtr.h"
#include "Poco/Notification.h"
#include "Poco/NotificationCenter.h"
#include "Poco/Timestamp.h"
#include "Poco/ThreadPool.h"
#include "Poco/Exception.h"

#include <list>

/**
 * Manage a collection of tasks and monitor their lifetime
 *
 * A TaskManager has a built-in NotificationCenter that
 * is used to send out notifications on task progress
 * and task states. See the TaskNotification class and its
 * subclasses for the various events that result in a notification.
 * To keep the number of notifications small, a TaskProgressNotification
 * will only be sent out once in 100 milliseconds.
 */
class TaskManager
{
public:
	typedef Poco::AutoPtr<MergeableTask>  TaskPtr;
	typedef std::list<TaskPtr> TaskList;

	/// Create the TaskManager, using the default ThreadPool.
	TaskManager();

	/// Create the TaskManager, using the given ThreadPool.
	TaskManager(Poco::ThreadPool& pool);

	~TaskManager();

	/// Start the given task in a thread obtained from the thread pool
	void start(TaskPtr pAutoTask);

	/// Start the given task in the current thread.
	void startSync(TaskPtr pAutoTask);

	/// Request cancellation of all tasks.
	void cancelAll();
		
	/// Return a copy of the internal task list.
	TaskList taskList() const;

	/// Return the number of tasks in the internal task list.
	std::size_t count() const;

	/**
	 * Register an observer with the NotificationCenter.
	 *
	 * Usage:
	 *     Observer<MyClass, MyNotification> obs(*this, &MyClass::handleNotification);
	 *     notificationCenter.addObserver(obs);
	 */
	void addObserver(const Poco::AbstractObserver& observer);

	/// Unregister an observer with the NotificationCenter.
	void removeObserver(const Poco::AbstractObserver& observer);

	static const int MIN_PROGRESS_NOTIFICATION_INTERVAL;

protected:
//	void postNotification(const Poco::Notification::Ptr& pNf);
//		/// Posts a notification to the task manager's
//		/// notification center.

	void taskStarted(MergeableTask* pTask);
	void taskProgress(MergeableTask* pTask, float progress);
	void taskCancelled(MergeableTask* pTask);
	void taskFinished(MergeableTask* pTask);
	void taskFailed(MergeableTask* pTask, const Poco::Exception& exc);

private:
	Poco::ThreadPool&        threadPool;
	TaskList           		 mTaskList;
	Poco::Timestamp          lastProgressNotification;
	Poco::NotificationCenter nc;
	mutable Poco::FastMutex  mutex;

	friend class MergeableTask;
};


//
// inlines
//
inline std::size_t TaskManager::count() const
{
	Poco::FastMutex::ScopedLock lock(mutex);

	return mTaskList.size();
}


#endif // CUSTOM_Foundation_TaskManager_INCLUDED
