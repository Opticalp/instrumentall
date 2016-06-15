/**
 * @file	src/MergeableTask.h
 * @date	May 2016
 * @author	Applied Informatics Software Engineering GmbH and contributors
 *
 * Original Poco::Task code checked out from https://github.com/pocoproject/poco
 * commit d6ebf591 (May 2016)
 */

//
// Task.h
//
// $Id: //poco/1.4/Foundation/include/Poco/Task.h#2 $
//
// Library: Foundation
// Package: Tasks
// Module:  Tasks
//
// Definition of the Task class.
//
// Copyright (c) 2004-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//

#ifndef SRC_MERGEABLETASK_H_
#define SRC_MERGEABLETASK_H_

#include "Poco/Runnable.h"
#include "Poco/RefCountedObject.h"
#include "Poco/Mutex.h"
#include "Poco/RWLock.h"
#include "Poco/Event.h"
#include "Poco/Timestamp.h"

#include <set>

class TaskManager;

/**
 * MergeableTask
 *
 * Re-implementation of Poco::Task:
 * > A Task is a subclass of Runnable that has a name
 * > and supports progress reporting and cancellation.
 * > A TaskManager object can be used to take care of the
 * > lifecycle of a Task.
 *
 * The MergeableTask is a Poco::Task which derived class ModuleTask
 * will control a Module execution.
 * It expends the Poco::Task by allowing to virtually merge tasks
 * (see slavedTasks and masterTask).
 *
 * In the frame of InstrumentAll, those tasks are designed to be used once,
 * then deleted. run() is not supposed to be called simultaneously.
 * But as run() can produce multiple threads, the access to setProgress,
 * etc is thread safe.
 */
class MergeableTask: public Poco::Runnable, public Poco::RefCountedObject
{
public:
	enum TaskState
	{
		TASK_IDLE,
		TASK_FALSE_START,
		TASK_STARTING,
		TASK_RUNNING,
		TASK_CANCELLING,
		TASK_FINISHED
	};

	MergeableTask();

	virtual std::string name() = 0;

	/**
	 * Return the task's progress
	 *
	 * The value will be between 0.0 (just started)
	 * and 1.0 (completed).
	 */
	float getProgress() const;

	/**
	 * Request the task to cancel itself.
	 *
	 * For cancellation to work, the task's runTask() method
	 * must periodically call isCancelled() and react accordingly.
	 * Can be overridden to implement custom behavior,
	 * but the base class implementation of cancel() should
	 * be called to ensure proper behavior.
	 */
	virtual void cancel();

	/**
	 * Return true if cancellation of the task has been requested.
	 *
	 * A Task's runTask() method should periodically
	 * call this method and stop whatever it is doing in an
	 * orderly way when this method returns true.
	 */
	bool isCancelled() const;

	/**
	 * Return the task's current state.
	 */
	TaskState getState() const;

	/**
	 * Set the task's progress to zero and clear the cancel flag.
	 */
	void reset();

	/**
	 * Do whatever the task needs to do. Must
	 * be overridden by subclasses.
	 */
	virtual void runTask() = 0;

	/**
	 * Runnable::run method implementation
	 */
	void run();

	/**
	 * Retrieve task unique ID
	 */
	size_t id() { return taskIndex; }

	/**
	 * Add a slave in the slavedTasks list
	 *
	 * and call the slave setMaster()
	 */
	void merge(MergeableTask* slave);

	/**
	 * Check if this task is a slave task.
	 */
	bool isSlave();

protected:
	/**
	 * Suspend the current thread for the specified
	 * amount of time.
	 *
	 * If the task is cancelled while it is sleeping,
	 * sleep() will return immediately and the return
	 * value will be true. If the time interval
	 * passes without the task being cancelled, the
	 * return value is false.
	 *
	 * A Task should use this method in favor of Thread::sleep().
	 */
	bool sleep(long milliseconds);

	/**
	 * Yield cpu to other threads
	 *
	 * If the task is cancelled while it is suspended,
	 * yield() will return true. If the tasks resumes
	 * without being cancelled, the return value is false.
	 *
	 * A Task should use this method in favor of Thread::yield().
	 */
	bool yield();

	/**
	 * Set the task's progress.
	 *
	 * The value should be between 0.0 (just started)
	 * and 1.0 (completed).
	 */
	void setProgress(float progress);

//	/// Post a notification to the task manager's
//	/// notification center.
//	///
//	/// A task can use this method to post custom
//	/// notifications about its getProgress.
//	virtual void postNotification(Poco::Notification* pNf);

	/**
	 * Set the (optional) owner of the task.
	 */
	void setOwner(TaskManager* pOwner);

	/**
	 * Return the owner of the task, which may be NULL.
	 */
	TaskManager* getOwner() const;

	/**
	 * Set the task's state.
	 */
	void setState(TaskState state);

	virtual ~MergeableTask();

private:
	MergeableTask(const MergeableTask&);
	MergeableTask& operator = (const MergeableTask&);

	static size_t nextAvailableIndex; ///< next index that can be used
	size_t taskIndex;

	/// to be called by the master task
	void setMaster(MergeableTask* master);
	/// to be called by the slave task upon deletion
	void eraseSlave(MergeableTask* slave);

	std::set<MergeableTask*> slavedTasks;
	MergeableTask* masterTask;

	void taskFinishedBroadcast(TaskManager* pTm);

	Poco::Timestamp t0; /// creation time
	Poco::Timestamp tBegin; /// beginning of run time
	Poco::Timestamp tEnd; /// end of run time

	TaskManager*      pOwner;
	float             progress;
	TaskState         state;
	Poco::Event       cancelEvent;

	mutable Poco::FastMutex mainMutex;
	mutable Poco::RWLock mergeAccess;

	friend class TaskManager;
};

#endif /* SRC_MERGEABLETASK_H_ */
