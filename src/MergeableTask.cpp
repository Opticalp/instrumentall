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
	Poco::FastMutex::ScopedLock lock(mainMutex);

	if (masterTask)
		masterTask->eraseSlave(this);

	if (slavedTasks.size())
	{
		for (std::set<MergeableTask*>::iterator it = slavedTasks.begin(),
				ite = slavedTasks.end(); it != ite; it++)
			(*it)->setMaster(NULL);
	}
}

float MergeableTask::getProgress() const
{
	Poco::FastMutex::ScopedLock lock(mainMutex);

	if (masterTask)
		return masterTask->getProgress();

	return progress;
}

void MergeableTask::cancel()
{
	if (masterTask)
	{
		masterTask->cancel();
		if (pOwner)
			pOwner->taskCancelled(this);
		return;
	}

	state = TASK_CANCELLING;
	cancelEvent.set();
	if (pOwner)
		pOwner->taskCancelled(this);
}

bool MergeableTask::isCancelled() const
{
	if (masterTask)
		return masterTask->isCancelled();

	return state == TASK_CANCELLING;
}

MergeableTask::TaskState MergeableTask::getState() const
{
	if (masterTask)
		return masterTask->getState();

	return state;
}

void MergeableTask::reset()
{
	if (masterTask)
	{
		masterTask->reset();
		return;
	}

	progress = 0.0;
	state    = TASK_IDLE;
	cancelEvent.reset();
}

void MergeableTask::run()
{
	if (masterTask)
		throw Poco::InvalidAccessException("MergeableTask::run",
				Poco::format("Task#%?d is a slave from Task#%?d",
						id(), masterTask->id()));

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
	state = TASK_FINISHED;
	if (pTm)
		pTm->taskFinished(this);
}

void MergeableTask::merge(MergeableTask* slave)
{
	Poco::FastMutex::ScopedLock lock(mainMutex);

	slavedTasks.insert(slave);
	slave->setMaster(this);
}

bool MergeableTask::sleep(long milliseconds)
{
	return cancelEvent.tryWait(milliseconds);
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
	Poco::FastMutex::ScopedLock lock(mainMutex);

	masterTask = master;
}

void MergeableTask::eraseSlave(MergeableTask* slave)
{
	Poco::FastMutex::ScopedLock lock(mainMutex);
	slavedTasks.erase(slave);
}
