/**
 * @file	src/TaskNotification.cpp
 * @date	June 2016
 * @author	Applied Informatics Software Engineering GmbH and contributors
 *
 * Original Poco::Task code checked out from https://github.com/pocoproject/poco
 * commit d6ebf591 (May 2016)
 */

//
// TaskNotification.cpp
//
// $Id: //poco/1.4/Foundation/src/TaskNotification.cpp#1 $
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


#include "TaskNotification.h"
#include "MergeableTask.h"


TaskNotification::TaskNotification(MergeableTask* pTask):
	_pTask(pTask)
{
	if (_pTask) _pTask->duplicate();
}


TaskNotification::~TaskNotification()
{
	if (_pTask) _pTask->release();
}


TaskStartedNotification::TaskStartedNotification(MergeableTask* pTask):
	TaskNotification(pTask)
{
}

	
TaskStartedNotification::~TaskStartedNotification()
{
}


TaskCancelledNotification::TaskCancelledNotification(MergeableTask* pTask):
	TaskNotification(pTask)
{
}

	
TaskCancelledNotification::~TaskCancelledNotification()
{
}


TaskFinishedNotification::TaskFinishedNotification(MergeableTask* pTask):
	TaskNotification(pTask)
{
}

	
TaskFinishedNotification::~TaskFinishedNotification()
{
}


TaskFailedNotification::TaskFailedNotification(MergeableTask* pTask, const Poco::Exception& exc):
	TaskNotification(pTask),
	_pException(exc.clone())
{
}

	
TaskFailedNotification::~TaskFailedNotification()
{
	delete _pException;
}


TaskFailedOnCancellationNotification::TaskFailedOnCancellationNotification(MergeableTask* pTask, const Poco::Exception& exc):
    TaskNotification(pTask),
    _pException(exc.clone())
{
}


TaskFailedOnCancellationNotification::~TaskFailedOnCancellationNotification()
{
    delete _pException;
}


TaskProgressNotification::TaskProgressNotification(MergeableTask* pTask, float taskProgress):
	TaskNotification(pTask),
	_progress(taskProgress)
{
}

	
TaskProgressNotification::~TaskProgressNotification()
{
}

TaskEnslavedNotification::TaskEnslavedNotification(MergeableTask* pTask, MergeableTask* enslaved):
	TaskNotification(pTask),
	mSlave(enslaved)
{
}

TaskEnslavedNotification::~TaskEnslavedNotification()
{
}
