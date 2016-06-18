/**
 * @file	src/MergeableTask.cpp
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

#include "MergeableTask.h"

#include "TaskManager.h"

#include "Poco/Exception.h"
#include "Poco/Format.h"
#include "Poco/Thread.h"

size_t MergeableTask::nextAvailableIndex = 0;

MergeableTask::MergeableTask():
		pOwner(NULL),
		progress(0.0),
		state(TASK_IDLE),
		cancelEvent(false), // manual reset
		masterTask(NULL)
{
	// creation time is set with the present time,
	// as it is for beginning of run time and end of run time.

	taskIndex = nextAvailableIndex++;
}

MergeableTask::~MergeableTask()
{
	Poco::ScopedReadRWLock lock(mergeAccess);

	if (masterTask)
		masterTask->eraseSlave(this);

	for (std::set<MergeableTask*>::iterator it = slavedTasks.begin(),
			ite = slavedTasks.end(); it != ite; it++)
	{
		(*it)->setProgress(progress);
		(*it)->setState(state);
		(*it)->setMaster(NULL);
	}
}

float MergeableTask::getProgress() const
{
	Poco::FastMutex::ScopedLock lock(mainMutex);

	mergeAccess.readLock();
	Poco::AutoPtr<MergeableTask> master(masterTask, true);
	mergeAccess.unlock();

	if (!master.isNull())
		return master->getProgress();

	return progress;
}

void MergeableTask::cancel()
{
	mergeAccess.readLock();
	Poco::AutoPtr<MergeableTask> master(masterTask, true);
	mergeAccess.unlock();

	if (!master.isNull())
		master->cancel();

	state = TASK_CANCELLING;
	cancelEvent.set();
	if (pOwner)
		pOwner->taskCancelled(this);
}

bool MergeableTask::isCancelled() const
{
	return state == TASK_CANCELLING;
}

MergeableTask::TaskState MergeableTask::getState() const
{
	mergeAccess.readLock();
	Poco::AutoPtr<MergeableTask> master(masterTask, true);
	mergeAccess.unlock();

	if (!master.isNull())
		return master->getState();

	return state;
}

void MergeableTask::reset()
{
	mergeAccess.readLock();
	if (masterTask)
		poco_bugcheck_msg("trying to reset a slave task");
	mergeAccess.unlock();

	progress = 0.0;
	state    = TASK_IDLE;
	cancelEvent.reset();
}

void MergeableTask::run()
{
	duplicate();

	mergeAccess.readLock();
	if (masterTask)
		throw Poco::InvalidAccessException("MergeableTask::run",
				Poco::format("Task#%?d is a slave from Task#%?d",
						id(), masterTask->id()));
	mergeAccess.unlock();

	TaskManager* pTm = getOwner();
	if (pTm)
		pTm->taskStarted(this);
	tBegin.update();
	try
	{
		state = TASK_RUNNING;
		runTask();
	}
	catch (Poco::Exception& exc)
	{
		if (pTm)
			pTm->taskFailed(this, exc);
	}
	catch (std::exception& exc)
	{
		if (pTm)
			pTm->taskFailed(this, Poco::SystemException(exc.what()));
	}
	catch (...)
	{
		if (pTm)
			pTm->taskFailed(this, Poco::SystemException("unknown exception"));
	}
	tEnd.update();

	taskFinishedBroadcast(pTm);

	leaveTask();

	release();
}

void MergeableTask::taskFinishedBroadcast(TaskManager* pTm)
{
	state = TASK_FINISHED;
	if (pTm)
		pTm->taskFinished(this);

	mergeAccess.readLock();
	std::set<MergeableTask*> slaves = slavedTasks;
	mergeAccess.unlock();

	for  (std::set<MergeableTask*>::iterator it = slaves.begin(),
			ite = slaves.end(); it != ite; it++)
	{
		Poco::AutoPtr<MergeableTask> slave(*it, true);
		slave->state = TASK_FINISHED;
		TaskManager* slaveTm = slave->getOwner();
		if (slaveTm)
			slaveTm->taskFinished(slave);
	}
}

void MergeableTask::merge(MergeableTask* slave)
{
	mergeAccess.writeLock();
	slavedTasks.insert(slave);
	slave->setMaster(this);
	mergeAccess.unlock();

	TaskManager* pTm = getOwner();
	if (pTm)
		pTm->taskEnslaved(this, slave);
}

bool MergeableTask::sleep(long milliseconds)
{
	return cancelEvent.tryWait(milliseconds);
}

bool MergeableTask::yield()
{
	Poco::Thread::yield();
	return isCancelled();
}

void MergeableTask::setProgress(float taskProgress)
{
	Poco::FastMutex::ScopedLock lock(mainMutex);

	if (progress != taskProgress)
	{
		progress = taskProgress;
		if (pOwner)
			pOwner->taskProgress(this, progress);
	}
}

void MergeableTask::setOwner(TaskManager* pOwner)
{
	Poco::FastMutex::ScopedLock lock(mainMutex);

	this->pOwner = pOwner;
}

TaskManager* MergeableTask::getOwner() const
{
	Poco::FastMutex::ScopedLock lock(mainMutex);

	return pOwner;
}

void MergeableTask::setState(TaskState taskState)
{
	state = taskState;
}

void MergeableTask::setMaster(MergeableTask* master)
{
	Poco::ScopedWriteRWLock lock(mergeAccess);
	masterTask = master;
}

bool MergeableTask::isSlave()
{
	Poco::ScopedReadRWLock lock(mergeAccess);
	return (masterTask != NULL);
}

void MergeableTask::eraseSlave(MergeableTask* slave)
{
	Poco::ScopedWriteRWLock lock(mergeAccess);
	slavedTasks.erase(slave);
}
