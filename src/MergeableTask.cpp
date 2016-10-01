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

POCO_IMPLEMENT_EXCEPTION( TaskMergedException, Poco::Exception, "The task was merged into a master")

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
		// set directly progress and state without verifications
		(*it)->progress = progress;
		(*it)->state = state;
		(*it)->setMaster(NULL);// skip verifications
	}
}

float MergeableTask::getProgress() const
{
	mergeAccess.readLock();
	Poco::AutoPtr<MergeableTask> master(masterTask, true);
	mergeAccess.unlock();

	if (!master.isNull())
		return master->getProgress();

	Poco::FastMutex::ScopedLock lock(mainMutex);
	return progress;
}

void MergeableTask::cancel()
{
	mergeAccess.readLock();
	Poco::AutoPtr<MergeableTask> master(masterTask, true);
	mergeAccess.unlock();

	if (master.isNull())
	{
	    state = TASK_CANCELLING;
	    cancelEvent.set();
	    if (pOwner)
	        pOwner->taskCancelled(this);
	}
	else
	    master->cancel();
}

bool MergeableTask::isCancelled() const
{
	return state == TASK_CANCELLING;
}

MergeableTask::TaskState MergeableTask::getState() const
{
	return state;
}

void MergeableTask::run()
{
	duplicate();

	TaskManager* pTm = getOwner();
	if (pTm)
		pTm->taskStarted(this);
	tBegin.update();
	try
	{
	    prepareTask();
		runTask();
	}
	catch (TaskMergedException&) // task merged, probably during prepareTask
	{
	    release();
	    return;
	}
	catch (ExecutionAbortedException& exc)
	{
        if (pTm)
            pTm->taskFailedOnCancellation(this, exc);
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
		slave->setState(TASK_FINISHED);
		TaskManager* slaveTm = slave->getOwner();
		if (slaveTm)
			slaveTm->taskFinished(slave);
	}
}

void MergeableTask::merge(MergeableTask* slave)
{
    mergeAccess.writeLock();
    try
    {
        slave->setMaster(this);
        slavedTasks.insert(slave);
    }
    catch (...)
    {
        mergeAccess.unlock();
        throw;
    }
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
    Poco::FastMutex::ScopedLock lock(mainMutex);

	if (taskState != TASK_FINISHED)
	{
	    if (state == TASK_CANCELLING)
            throw ExecutionAbortedException("task cancelling. "
				"Can not be changed to another state than \"finished\"");
        if (state == TASK_MERGED)
            throw TaskMergedException("slave task. "
                "Can not be changed to another state than \"finished\"");
	}

	switch (taskState)
	{
	case TASK_STARTING:
		if (state != TASK_IDLE)
			throw Poco::RuntimeException("trying to start a task that is not idle");
		break;
	case TASK_RUNNING:
		if (state != TASK_STARTING)
			throw Poco::RuntimeException("trying to run a task that is not started");
		break;
	case TASK_MERGED:
	    if (state != TASK_IDLE && state != TASK_STARTING)
	        throw Poco::RuntimeException("trying to merge: " +
	                name() + " that is not idle nor starting...");
	    break;
	default:
		break;
	}

	state = taskState;
}

void MergeableTask::setMaster(MergeableTask* master)
{
	Poco::ScopedWriteRWLock lock(mergeAccess);

    if (master)
        setState(TASK_MERGED);

    masterTask = master;
}

void MergeableTask::eraseSlave(MergeableTask* slave)
{
	Poco::ScopedWriteRWLock lock(mergeAccess);
	slavedTasks.erase(slave);
}
